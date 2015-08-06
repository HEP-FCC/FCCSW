//
//  PrintHits.cxx
//  
//
//  Created by Julia Hrdinka on 29/04/15.
//
//

#include "TestTools/PrintHits.h"

#include "RecoGeometry/PlaneSurface.h"

DECLARE_COMPONENT(PrintHits)

PrintHits::PrintHits(const std::string& type, const std::string& name, const IInterface* parent) :
AlgTool(type,name,parent),
m_log(msgSvc(), name),
m_tInX0(0.),
m_pathlength(0.),
m_pathlength1(0.),
m_counter(0),
m_hits(0)
{}

StatusCode PrintHits::initialize()
{
    declareInterface<IPrintHits>(this);

    StatusCode sc = service("THistSvc", m_ths);
    if (sc.isFailure()) throw GaudiException("Service [THistSvc] not found", name(), sc);
    
    file = new TFile("hitproperies.root", "RECREATE");
    if(!file->IsOpen()) m_log << MSG::WARNING << ("Could not open Root file") << endmsg;
    
    StatusCode sc1 = service( "RndmGenSvc", m_RGS, true);
    if( sc1.isFailure() )
        throw GaudiException("Service [RndmGenSvc] not found", name(), sc1);
    
    m_out.open("printhits1.dat");
    
    return StatusCode::SUCCESS;
}

StatusCode PrintHits::finalize()
{
    file->Print();
    m_out.close();
    return StatusCode::SUCCESS;
}

double PrintHits::flatrand(double min, double max) const {
    std::uniform_real_distribution<double> unif(min,max);
    std::random_device rd;
    std::mt19937 gen(rd());
    
    return (unif(gen));
}

const Alg::Vector3D PrintHits::randomdir()
{
 //   StatusCode sc = m_flat.initialize(m_RGS, Rndm::Flat(0.,1.));
//    if (sc.isFailure())
//        throw GaudiException("Couldn't initialize falt random numbers", name(), sc);
//    Alg::Vector3D dir(m_flat(),m_flat(),m_flat());
    double phi   = flatrand(0.,2.*M_PI);
    double theta = flatrand(0.3282,2.8133);
 //double theta = flatrand(0.,2.4850);
 //   double theta = flatrand(-1.2425,1.2425);
  //  double theta = flatrand(-0.4282,0.4282);
//    double theta = flatrand(0.,M_PI);
    Alg::Vector3D dir(cos(phi)*sin(theta),sin(phi)*sin(theta),cos(theta));
    return (dir);
}


const Reco::Volume* PrintHits::scanVolumes(const Reco::Volume* volume, const Alg::Point3D& glopos) const
{
    if(volume->type()==Reco::Volume::ContainerVolume) {
        const Reco::ContainerVolume* contvol = dynamic_cast<const Reco::ContainerVolume*>(volume);
        if (contvol) {
            contvol->getVolume(glopos);
            scanVolumes(contvol,glopos);
        }
    }
    return (volume);
}

StatusCode PrintHits::intersect(const Reco::Volume* volume, Alg::Point3D& glopos, const Alg::Vector3D& dir) const
{
    if (volume) {
        std::vector<const Reco::Layer*> materialLayers = volume->materialLayersOrdered(glopos,dir,0.);
        if (materialLayers.empty()) {
            std::vector<std::weak_ptr<const Reco::BoundarySurface>> boundarySurfaces(volume->boundarySurfaces());
            if (!boundarySurfaces.empty()) {
                bool success = true;
                for (auto boundary_it : boundarySurfaces) {
                    std::shared_ptr<const Reco::BoundarySurface> surface = boundary_it.lock();
                }
                for (auto boundary_it : boundarySurfaces) {
                    if (success) {
                        intersect(nextVolume(boundary_it,glopos,dir,success), glopos ,dir);
                    }
                }
            }
        }
        else {
            for (auto layer_it : materialLayers) {
                Trk::Intersection layerintersect(layer_it->surfaceRepresentation()->straightLineIntersection(glopos,dir,true));
                if (layerintersect.onSurface) {
                    Alg::Point3D layerpoint = layerintersect.position;
                    std::vector<Reco::SurfaceVector> csurfaces = layer_it->compatibleSurfaces(layerpoint);
                    for (auto vector_it : csurfaces)
                    {
                        for (auto surf_it : vector_it)
                        {
                            if (surf_it) {
                                intersectSurface(*surf_it, glopos, dir);
                            }
                        }
                    } // for surfaces */
                }
            }//for layers
            std::vector<std::weak_ptr<const Reco::BoundarySurface>> boundarySurfaces(volume->boundarySurfaces());
            if (!boundarySurfaces.empty()) {
                bool success = true;
                for (auto boundary_it : boundarySurfaces) {
                    std::shared_ptr<const Reco::BoundarySurface> surface = boundary_it.lock();
                    if (success) {
                        intersect(nextVolume(boundary_it,glopos,dir,success), glopos ,dir);
                    }
                }
            }
        }//if materiallayers
    }
    
    return StatusCode::SUCCESS;
}

StatusCode PrintHits::intersectSurface(const Reco::Surface& surface, Alg::Point3D& glopos, const Alg::Vector3D& dir) const
{
    Trk::Intersection intersection(surface.straightLineIntersection(glopos,dir,true));
    if (intersection.onSurface) {
        const Alg::Point3D intersectPoint = intersection.position;
        Alg::Point2D locpos(0.,0.);
        if(surface.globalToLocal(intersectPoint, dir, locpos)) {
            double t = surface.material(locpos)->tInX0();
  //          if (t < 0.) std::cout << "tinx0 negativ" << std::endl;
  //          if (surface.halfThickness() < 0.) std::cout << "halfThickness negativ" << std::endl;
            m_tInX0 += fabs((surface.material(locpos)->tInX0())*((surface.pathlength(intersectPoint,dir))/(2.*surface.halfThickness())));
            double pathlength = surface.pathlength(dir);
            double pathlength1= surface.pathlength(intersectPoint,dir);
   //         if (pathlength1<0.) std::cout << "pathlength negativ" << std::endl;
            m_pathlength += pathlength;
            m_pathlength1+= pathlength1;
            m_hits++;
  //          std::cout <<"Radius:" << sqrt(intersectPoint.Perp2()) << " Printhits:pathlength " << pathlength << " pathlengt1: "  << surface.pathlength(intersectPoint,dir) << std::endl;
  //          m_out << " dir: " << dir << "tInX0surf: " << surface.material(locpos)->tInX0() << " pathlength: " << pathlength << " new pathlength: "<< pathlength1 << " thickness: " << 2.*surface.halfThickness() << " p/t: " << (surface.pathlength(dir))/(2.*surface.halfThickness()) << " tInX0: " << m_tInX0 << std::endl;
 //           std::cout << "old pathlength: " << pathlength << std::endl;
 //           std::cout << "new pathlength: " << pathlength1 << std::endl;
        }
    }
    return StatusCode::SUCCESS;
}

const Reco::Volume* PrintHits::nextVolume(std::weak_ptr<const Reco::BoundarySurface> surf, Alg::Point3D& glopos , const Alg::Vector3D& dir, bool& success) const
{
    if (std::shared_ptr<const Reco::BoundarySurface> surface = surf.lock()) {
        if (surface->surfaceRepresentation()) {
            glopos = glopos+0.000000000001*dir;
            Trk::Intersection intersection(surface->surfaceRepresentation()->straightLineIntersection(glopos,dir,true));
            if (intersection.onSurface) {
                success = false;
                glopos = intersection.position;
                return(surface->nextVolume(intersection.position));
            } //if onSurface
        } //if Surface
    } //ifBoundarySurface
    return (0);
}


StatusCode PrintHits::printMaterial(std::shared_ptr<const Reco::ContainerVolume> worldVolume, const Alg::Point3D& start, size_t Nevents)
{
    const Reco::Volume* volume(worldVolume->getVolume(start));
    if (volume) {
        if (volume->type()==Reco::Volume::ContainerVolume) {
            const Reco::Volume* subvolume(scanVolumes(volume, start));
            volume = subvolume;
        }
    }

    m_tInX0Prof = new TProfile("tInX0", "tInX0 over Eta", 3000, -10., 10.);
    if (m_ths->regHist("tInX0", m_tInX0Prof).isFailure()) {
        m_log << MSG::ERROR << "Couldn't register Histogram" << endmsg;
    }
    
    m_pathProf = new TProfile("Pathlength", "Pathlength over Eta", 3000, -10., 10.);
    if (m_ths->regHist("Pathlength", m_pathProf).isFailure()) {
        m_log << MSG::ERROR << "Couldn't register Histogram" << endmsg;
    }
    
    m_hitsProf = new TProfile("Hits", "Hits over Eta", 3000, -10.,10.);
    if (m_ths->regHist("Hits", m_hitsProf).isFailure()) {
        m_log << MSG::ERROR << "Couldn't register Histogram" << endmsg;
    }
    
    for (size_t i = 0; i < Nevents; i++) {
        Alg::Point3D startpoint = start;
        m_tInX0 = 0;
        m_pathlength = 0.;
        m_pathlength1 = 0.;
        m_counter = 0;
        m_hits = 0;
        Alg::Vector3D dir = randomdir();
        double eta = dir.Eta();
        intersect(volume,startpoint,dir);
 //       m_out << "final: " << "mean: " << m_meanPath << " t: " << m_tInX0 << std::endl;
        m_tInX0Prof->Fill(eta,m_tInX0);
        m_pathProf->Fill(eta,m_pathlength1);
        m_hitsProf->Fill(eta,m_hits);
    }
  /*  Alg::Point3D point (0.,0.,0.);
    Alg::Vector3D dir1(0.167491,-0.182156,-0.968899);
    intersect(volume,point,dir1);
    point = start;
    Alg::Vector3D dir2(0.0102265,0.154088,-0.988004);
    intersect(volume,point,dir2);
    point = start;
    Alg::Vector3D dir3(-0.0715089,0.347588,-0.934917);
    intersect(volume,point,dir3);*/
    
    m_tInX0Prof->Write();
    m_pathProf->Write();
    m_hitsProf->Write();
    
    return StatusCode::SUCCESS;
}
StatusCode PrintHits::printMaterial(std::shared_ptr<const Reco::ContainerVolume> worldVolume, const Alg::Point3D& start, std::vector<Alg::Vector3D> directions)
{
    const Reco::Volume* volume(worldVolume->getVolume(start));
    if (volume) {
        if (volume->type()==Reco::Volume::ContainerVolume) {
            const Reco::Volume* subvolume(scanVolumes(volume, start));
            volume = subvolume;
        }
    }
    
    m_tInX0Prof1 = new TProfile("tInX01", "Thickness in x0 over Eta", 100, -4., 4.);
    if (m_ths->regHist("tInX01", m_tInX0Prof1).isFailure()) {
        m_log << MSG::ERROR << "Couldn't register Histogram" << endmsg;
    }
    
    m_pathProf1 = new TProfile("Pathlength1", "Pathlength in Eta", 100, -4., 4.);
    if (m_ths->regHist("Pathlength1", m_pathProf1).isFailure()) {
        m_log << MSG::ERROR << "Couldn't register Histogram" << endmsg;
    }
    
    m_hitsProf = new TProfile("Hits", "Hits over Eta", 100, -4.,4.);
    if (m_ths->regHist("Hits", m_hitsProf).isFailure()) {
        m_log << MSG::ERROR << "Couldn't register Histogram" << endmsg;
    }
    
    for (auto& dir : directions) {
        
        Alg::Point3D startpoint = start;
        m_tInX0 = 0;
        m_pathlength = 0.;
        m_pathlength1 = 0.;
        m_counter = 0;
        m_hits = 0;
        double eta = dir.eta();
 /*       std::cout << "---------------------------------------------" << std::endl;
        std::cout << "-----------------new direction---------------" << std::endl;
        std::cout << dir << std::endl;
        std::cout << "---------------------------------------------" << std::endl;*/
        intersect(volume,startpoint,dir);
        //       m_out << "final: " << "mean: " << m_meanPath << " t: " << m_tInX0 << std::endl;
        m_tInX0Prof1->Fill(eta,m_tInX0);
        m_pathProf1->Fill(eta,m_pathlength);
        m_hitsProf->Fill(eta,m_hits);
    }
/*    Alg::Point3D point (0.,0.,0.);
    Alg::Vector3D vector(0.086478,-0.205382,-0.974854);
    intersect(volume,point,vector);*/
    
    std::cout << "Test8" << std::endl;
    
    m_tInX0Prof1->Write();
    m_pathProf1->Write();
    m_hitsProf->Write();
    
    std::cout << "Test9" << std::endl;
    
    return StatusCode::SUCCESS;
}

StatusCode PrintHits::printHits(std::vector<std::pair<double, const Alg::Vector3D>>& hits)
{
    return StatusCode::SUCCESS;
}
