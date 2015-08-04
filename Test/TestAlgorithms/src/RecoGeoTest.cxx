//
//  RecoGeoTest.cxx
//
//
//  Created by Julia Hrdinka on 26/02/15.
//
//

#include "TestAlgorithms/RecoGeoTest.h"
#include "RecoGeometry/CylinderVolume.h"

DECLARE_COMPONENT(RecoGeoTest)

RecoGeoTest::RecoGeoTest(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc),
m_recoGeoSvc(nullptr),
m_worldVolume(0),
m_graph(new TGraph2D()),
m_counter(0),
m_particlecoll(new ParticleCollection()),
m_toolsvc(0),
m_printhits(0),
//m_hits(),
m_tInX0(0.),
m_pathlength(0.)
//m_hitcoll(new TrackHitCollection()),
{
    m_out.open ("recogeo.dat", std::ofstream::out);
    m_layers.open("layers.dat", std::ofstream::out);
    m_boundaries.open("boundaries.dat", std::ofstream::out);
    m_modules.open("modules.dat", std::ofstream::out);
    m_sens.open("sens.dat", std::ofstream::out);
 //   declareOutput("trackHits", m_trackhits, "in");
//    declareOutput("particles", m_particles, "in");
}

StatusCode RecoGeoTest::initialize() {
    
    if (GaudiAlgorithm::initialize().isFailure()){
        return StatusCode::FAILURE;
    }
    
    if (service("ClassicalRecoGeoSvc", m_recoGeoSvc, true).isFailure()) {
        error() << "Couldn't get RecoGeoSvc" << endmsg;
        return StatusCode::FAILURE;
    }

    std::shared_ptr<const Reco::ContainerVolume> worldVolume(m_recoGeoSvc->getRecoGeo()->clone());
    std::cout << "worldvolume" << std::endl;
    m_worldVolume = worldVolume;
    if (m_worldVolume) {
        std::cout << "retrieved WorldVolume!!!" << std::endl; //b
    }
    else std::cout << "no worldvolume" << std::endl;
    
    if (service("ToolSvc", m_toolsvc).isFailure()) {
        error() << "Couldn't get ToolSvc" << endmsg;
        return StatusCode::FAILURE;
    }
    
    if (m_toolsvc->retrieveTool("PrintHits", m_printhits).isFailure()) {
        error() << "Couldn't get PrintHits Tool" << endmsg;
    }

    StatusCode sc = service("THistSvc", m_ths);
    if (sc.isFailure()) throw GaudiException("Service [THistSvc] not found", name(), sc);
    return StatusCode::SUCCESS;
}

double RecoGeoTest::flatrand(double min, double max) const {
    std::uniform_real_distribution<double> unif(min,max);
    std::random_device rd;
    std::mt19937 gen(rd());
    
    return (unif(gen));
}

const Reco::Volume* RecoGeoTest::scanVolumes(const Reco::Volume* volume, const Alg::Point3D& glopos) const
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

const Alg::Vector3D RecoGeoTest::randomdir() const
{
    double phi   = 0.;//flatrand(0.,2.*M_PI);
//    double theta = flatrand(-0.4282,0.4282);
//    double theta = flatrand(0.,M_PI);
    double theta = 0.4;
//    double theta = flatrand(0.5237,2.6874);
  //  double theta = flatrand(2.69,M_PI);
//    double theta = flatrand(-1.24253,1.24253);
//    double theta = flatrand(1.68,1.9);
    
//    double theta = flatrand(0.,0.3282);
    Alg::Vector3D dir(cos(phi)*sin(theta),sin(phi)*sin(theta),cos(theta));
    return (dir);
}

StatusCode RecoGeoTest::intersect(const Reco::Volume* volume, Alg::Point3D& glopos, const Alg::Vector3D& dir) const
{
    if (volume) {
        std::cout << "############# entered volume: #############" << volume->center() << " RMax: " << volume->getCoordinate(Reco::CylinderVolume::Rmax) << std::endl; //b
        std::vector<const Reco::Layer*> materialLayers = volume->materialLayersOrdered(glopos,dir,0.);
        if (materialLayers.empty()) {
            //std::cout << "no material try go to next volume" << std::endl; //b
            std::vector<std::weak_ptr<const Reco::BoundarySurface>> boundarySurfaces(volume->boundarySurfaces());
            if (!boundarySurfaces.empty()) {
                bool success = true;
                int surf_count = 0;
                for (auto boundary_it : boundarySurfaces) {
                    std::shared_ptr<const Reco::BoundarySurface> surface = boundary_it.lock();
                    if(surface) ++surf_count;
                }
                for (auto boundary_it : boundarySurfaces) {
                    if (success) {
                        intersect(nextVolume(boundary_it,glopos,dir,success), glopos ,dir);
                    }
                }
            }
        }
        else {
          std::cout << "$$$$$$$$$$$$$ materiallayers, try to intersect with surfaces $$$$$$$$$$$$$" << std::endl; //b
            for (auto layer_it : materialLayers) {
                    Trk::Intersection layerintersect(layer_it->surfaceRepresentation()->straightLineIntersection(glopos,dir,true));
                    if (layerintersect.onSurface) {
                        Alg::Point3D layerpoint = layerintersect.position;
                        m_layers << layerpoint.X() << " " << layerpoint.Y() << " " << layerpoint.Z() << std::endl;
                        std::cout << "layerpoint: " << layerpoint << std::endl;
                        std::vector<Reco::SurfaceVector> csurfaces = layer_it->compatibleSurfaces(layerpoint);
                       std::cout << "try to intersect surfaces: " << csurfaces.size() << std::endl; //b
                        for (auto vector_it : csurfaces)
                        {
                            std::cout << " with: " << vector_it.size() <<std::endl; //b
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
                int boundarycount = 0;
                for (auto boundary_it : boundarySurfaces) {
                    std::shared_ptr<const Reco::BoundarySurface> surface = boundary_it.lock();
                   if(surface) ++boundarycount;
                    if (success) {
                        intersect(nextVolume(boundary_it,glopos,dir,success), glopos ,dir);
                    }
                }
            }
        }//if materiallayers
        
    }
    
    return StatusCode::SUCCESS;
}

bool RecoGeoTest::intersectSurface(const Reco::Surface& surface, Alg::Point3D& glopos, const Alg::Vector3D& dir) const
{
    Trk::Intersection intersection(surface.straightLineIntersection(glopos,dir,true));
    bool result = false;
    std::cout << "surface center: " << surface.center() << std::endl;
    if (intersection.onSurface) {
        result = true;
        const Alg::Point3D intersectPoint = intersection.position;
        
    
        m_modules << intersectPoint.X() << " " << intersectPoint.Y() << " " << intersectPoint.Z() << std::endl;
//        m_sumh++;
//t        m_hits.push_back(std::make_tuple(&surface, intersectPoint, dir));
        
 //h       m_modules << 0.5*M_PI-acos(dir.Dot(surface.normal())) << std::endl;
        Alg::Point2D locpos(0.,0.);
        if(surface.globalToLocal(intersectPoint, dir, locpos)) {
            m_tInX0 += (surface.material(locpos)->tInX0());//*(surface.pathlength(intersectPoint,dir)/(2.*surface.halfThickness()));
            m_pathlength += surface.pathlength(intersectPoint,dir);
            std::cout << "@@@@@@@@ INTERSECTED " << "R:" << sqrt(intersectPoint.X()*intersectPoint.X()+intersectPoint.Y()*intersectPoint.Y()) << " surface: " << surface.center() << " SUCCESSFULL: " << intersectPoint << std::endl;//b
            std::cout << "halfThickness: " << surface.halfThickness() << " pathlength1: " << surface.pathlength(intersectPoint,dir) << " pathlength2: " << m_pathlength << " tInX0: " << m_tInX0 << std::endl;//b
 //v           std::cout << " tInX0: " << m_tInX0 << std::endl;
        }
 /*       ParticleHandle& part = m_particlecoll->create();
        part.mod().Core.Vertex.X =  intersectPoint.X();
        part.mod().Core.Vertex.Y =  intersectPoint.Y();
        part.mod().Core.Vertex.Z =  intersectPoint.Z();*/
 /*       if (surface.isSensitive()) {
            Alg::Point2D locpos;
            //const Reco::SensitiveSurface sens(dynamic_cast<const Reco::SensitiveSurface&>(surface));
            //surface representation machen
            if(surface.globalToLocal(intersectPoint, dir, locpos)) m_sens << (dynamic_cast<const Reco::SensitiveSurface&>(surface)).cellID(locpos) << std::endl;
            TrackHitHandle& hit = m_hitcoll->create();
            hit.mod().Core.Cellid = (dynamic_cast<const Reco::SensitiveSurface&>(surface)).cellID(locpos);
        }*/
    }
        ++m_counter;
    return result;
  //  return StatusCode::SUCCESS;
}

const Reco::Volume* RecoGeoTest::nextVolume(std::weak_ptr<const Reco::BoundarySurface> surf, Alg::Point3D& glopos , const Alg::Vector3D& dir, bool& success) const
{
    std::cout << "next volume" <<  std::endl; //b
    if (std::shared_ptr<const Reco::BoundarySurface> surface = surf.lock()) {
        if (surface->surfaceRepresentation()) {
            glopos = glopos+0.0000000001*dir;
            std::cout << "at: " << glopos << std::endl; //b
            Trk::Intersection intersection(surface->surfaceRepresentation()->straightLineIntersection(glopos,dir,true));
            if (intersection.onSurface) {
                success = false;
                glopos = intersection.position;
      //          std::cout << "intersected boudnary surface: " << surface.get()->center() << " at: " << glopos << std::endl; //b
                m_boundaries << glopos.X() << " " << glopos.Y() << " " << glopos.Z() << " " << std::endl;
                return(surface->nextVolume(intersection.position));
            } //if onSurface
        } //if Surface
    } //ifBoundarySurface
    return (0);
}

StatusCode RecoGeoTest::execute() {
    
    Alg::Point3D start(0.,0.,0.);
 /*   Alg::Vector3D dir1(0.5542,0.,0.8324);
    Alg::Vector3D dir2(0.5542,0.,-0.8324);
    Alg::Vector3D dir3(1.,0.,0.);
    Alg::Vector3D dir4(47.,0.,15.);
    Alg::Vector3D dir5(47.,0.,9.);*/
 //   std::vector<std::pair<double, const Alg::Vector3D>> sumh;
    
    const Reco::Volume* volume (m_worldVolume->getVolume(start));
    if (volume) {
        //scan through volumes if it is a container volume
        if (volume->type()==Reco::Volume::ContainerVolume) {
         const Reco::Volume* subvolume(scanVolumes(volume, start));
         volume = subvolume;
         }
 
     //   (0.195152,-0.265202,0.944237)
      //  Alg::Vector3D dir(-0.146748,0.11085,0.982943);
 //       Alg::Vector3D dir(1.,0.,0.);
 //       start.SetCoordinates(0.,0.,0.);
 //       std::cout << "new direction: " << dir << std::endl;
  //      intersect(volume,start,dir);

        
        m_profile = new TProfile("tInX0_1", "tInX0 over Eta", N_REPEAT, -10., 10.);
        if (m_ths->regHist("tInX0_1", m_profile).isFailure()) {
            error() << "Couldn't register Histogram" << endmsg;
        }
        
        Alg::Vector3D dir = randomdir();
        std::cout << "___________________new direction" << dir << " theta: " << dir.Theta()  <<  "___________________: " << std::endl;
        intersect(volume, start, dir);
        
  /*      for (int i = 0; i < 150000; i++) {
            start.SetCoordinates(0.,0.,0.);
            m_tInX0 = 0.;
            m_pathlength = 0.;
            Alg::Vector3D dir = randomdir();
            std::cout << "___________________new direction" << dir << " theta: " << dir.Theta()  <<  "___________________: " << std::endl;
            intersect(volume, start, dir);
            double eta = dir.Eta();
            m_profile->Fill(eta,m_tInX0);
        }
        */
   /*     start.SetCoordinates(0.,0.,0.);
        std::cout << "new direction: " << dir1 << std::endl;
        intersect(volume,start,dir1);
        start.SetCoordinates(0.,0.,0.);
        std::cout << "___________________new direction___________________: " << dir2 << std::endl;
        intersect(volume,start,dir2);
        start.SetCoordinates(0.,0.,0.);
        std::cout << "___________________new direction___________________: " << dir3 << std::endl;
        intersect(volume,start,dir3);
        start.SetCoordinates(0.,0.,0.);
        std::cout << "___________________new direction___________________: " << dir4 << std::endl;
        intersect(volume,start,dir4.Unit());
       start.SetCoordinates(0.,0.,0.);
       std::cout << "___________________new direction___________________: " << dir5 << std::endl;
       intersect(volume,start,dir5.Unit());*/
   // }
        //intersect
  /*      m_profile = new TProfile("tInX0_1", "tInX0 over Eta", N_REPEAT, -10., 10.);
        if (m_ths->regHist("tInX0_1", m_profile).isFailure()) {
            error() << "Couldn't register Histogram" << endmsg;
        }*/
        
     /*   for (int i = 0; i < N_REPEAT; i++) {
            start.SetCoordinates(0.,0.,0.);
  //          m_sumh = 0.;
            m_tInX0 = 0.;
            Alg::Vector3D dir = randomdir();
            double eta = dir.Eta();
  //          std::cout << "new direction: " << dir << " eta: " << eta << std::endl;
            intersect(volume, start, dir);
    //        sumh.push_back(std::make_pair(m_sumh, dir));
            m_profile->Fill(eta,m_tInX0);
        }*/
//    }
  //      m_trackhits.put(m_hitcoll);
        //hier dann auch fehler mitgeben
  //      if (m_hits.empty()) {
  //          std::cout << "VECTOR EMPTY" << std::endl;
  //      }
   //t     m_printhits->printMaterial(m_hits);
   //t     m_printhits->printHits(sumh);
        
  
  /*      for (auto& hit : m_hits) {
            m_modules << hit.second.X() << " " << hit.second.Y() << " " << hit.second.Z() << std::endl;
        }*/
    
    
//        m_printhits->printMaterial(m_worldVolume, Alg::Point3D(0.,0.,0.), N_REPEAT);
     //   m_particles.put(m_particlecoll);
        m_out.close();
        m_boundaries.close();
        m_layers.close();
        m_modules.close();
        m_sens.close();

        
   }// if volume
//    m_hitcoll->print();
    
    return StatusCode::SUCCESS;
}


StatusCode RecoGeoTest::finalize() {
    
    if (GaudiAlgorithm::finalize().isFailure())
        return StatusCode::FAILURE;
    
    TFile* file = new TFile("tInX0.root", "RECREATE");
    if (!file->IsOpen()) error() << "Could not open Root file" << endmsg;
    
  //  m_profile->Write();
    
    file->Print();
    
    return StatusCode::SUCCESS;
    
}
