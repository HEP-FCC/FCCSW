//
//  RecoGeoTest.cxx
//
//
//  Created by Julia Hrdinka on 26/02/15.
//
//

#include "TestAlgorithms/RecoGeoTest.h"

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
m_hits()
//m_hitcoll(new TrackHitCollection()),
{
    m_out.open ("recogeo.dat", std::ofstream::out);
    m_layers.open("layers.dat", std::ofstream::out);
    m_boundaries.open("boundaries.dat", std::ofstream::out);
    m_modules.open("modules.dat", std::ofstream::out);
    m_sens.open("sens.dat", std::ofstream::out);
 //   declareOutput("trackHits", m_trackhits, "in");
    declareOutput("particles", m_particles, "in");
}

StatusCode RecoGeoTest::initialize() {
    
    if (GaudiAlgorithm::initialize().isFailure()){
        return StatusCode::FAILURE;
    }
    
    if (service("StandardRecoGeoSvc", m_recoGeoSvc, true).isFailure()) {
        error() << "Couldn't get RecoGeoSvc" << endmsg;
        return StatusCode::FAILURE;
    }

    m_recoGeoSvc->buildGeometry();
    std::shared_ptr<const Reco::ContainerVolume> worldVolume(m_recoGeoSvc->getRecoGeo()->clone());
    m_worldVolume = worldVolume;
    if (m_worldVolume) {
        std::cout << "retrieved WorldVolume!!!" << std::endl;
    }
    
    if (service("ToolSvc", m_toolsvc).isFailure()) {
        error() << "Couldn't get ToolSvc" << endmsg;
        return StatusCode::FAILURE;
    }
    if (m_toolsvc->retrieveTool("PrintHits", m_printhits).isFailure()) {
        error() << "Couldn't get PrintHits Tool" << endmsg;
    }

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
    double phi   = flatrand(0.,2.*M_PI);
//    double theta = flatrand(-0.4282,0.4282);
    double theta = flatrand(0.,M_PI);
    Alg::Vector3D dir(cos(phi)*sin(theta),sin(phi)*sin(theta),cos(theta));
    return (dir);
}

StatusCode RecoGeoTest::intersect(const Reco::Volume* volume, Alg::Point3D& glopos, const Alg::Vector3D& dir) const
{
    if (volume) {
        std::vector<const Reco::Layer*> materialLayers = volume->materialLayersOrdered(glopos,dir,0.);
        if (materialLayers.empty()) {
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
            for (auto layer_it : materialLayers) {
                    Trk::Intersection layerintersect(layer_it->surfaceRepresentation()->straightLineIntersection(glopos,dir,true));
                    if (layerintersect.onSurface) {
                        Alg::Point3D layerpoint = layerintersect.position;
                        m_layers << layerpoint.X() << " " << layerpoint.Y() << " " << layerpoint.Z() << std::endl;
                        std::vector<const Reco::Surface*> csurfaces = layer_it->compatibleSurfaces(layerpoint);

                        for (auto surf_it : csurfaces)
                        {
                            if (surf_it) {
                                intersectSurface(*surf_it, glopos, dir);
                            }
                        } // for surfaces */
                    }
                    std::vector<const Reco::Surface*> surfaces = layer_it->getSurfaces();
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

StatusCode RecoGeoTest::intersectSurface(const Reco::Surface& surface, Alg::Point3D& glopos, const Alg::Vector3D& dir) const
{
    Trk::Intersection intersection(surface.straightLineIntersection(glopos,dir,true));
    if (intersection.onSurface) {
        const Alg::Point3D intersectPoint = intersection.position;
 //       m_modules << intersectPoint.X() << " " << intersectPoint.Y() << " " << intersectPoint.Z() << std::endl;
        m_sumh++;
        m_hits.push_back(std::make_tuple(&surface, intersectPoint, dir));
        
        m_modules << 0.5*M_PI-acos(dir.Dot(surface.normal())) << std::endl;
        
        ParticleHandle& part = m_particlecoll->create();
        part.mod().Core.Vertex.X =  intersectPoint.X();
        part.mod().Core.Vertex.Y =  intersectPoint.Y();
        part.mod().Core.Vertex.Z =  intersectPoint.Z();
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
    
    return StatusCode::SUCCESS;
}

const Reco::Volume* RecoGeoTest::nextVolume(std::weak_ptr<const Reco::BoundarySurface> surf, Alg::Point3D& glopos , const Alg::Vector3D& dir, bool& success) const
{
    if (std::shared_ptr<const Reco::BoundarySurface> surface = surf.lock()) {
        if (surface->surfaceRepresentation()) {
            glopos = glopos+0.0001*dir;
            Trk::Intersection intersection(surface->surfaceRepresentation()->straightLineIntersection(glopos,dir,true));
            if (intersection.onSurface) {
                success = false;
                glopos = intersection.position;
                m_boundaries << glopos.X() << " " << glopos.Y() << " " << glopos.Z() << " " << std::endl;
                return(surface->nextVolume(intersection.position));
            } //if onSurface
        } //if Surface
    } //ifBoundarySurface
    return (0);
}

StatusCode RecoGeoTest::execute() {
    
    Alg::Point3D start(0.,0.,0.);
    std::vector<std::pair<double, const Alg::Vector3D>> sumh;
    
    const Reco::Volume* volume (m_worldVolume->getVolume(start));
    if (volume) {
        //scan through volumes if it is a container volume
        if (volume->type()==Reco::Volume::ContainerVolume) {
         const Reco::Volume* subvolume(scanVolumes(volume, start));
         volume = subvolume;
         }
        //intersect
        for (int i = 0; i < N_REPEAT; i++) {
            start.SetCoordinates(0.,0.,0.);
            m_sumh = 0.;
            Alg::Vector3D dir = randomdir();
            intersect(volume, start, dir);
            sumh.push_back(std::make_pair(m_sumh, dir));
        }

  //      m_trackhits.put(m_hitcoll);
        //hier dann auch fehler mitgeben
        std::cout << "before!!!" << std::endl;
  //      if (m_hits.empty()) {
  //          std::cout << "VECTOR EMPTY" << std::endl;
  //      }
        m_printhits->printMaterial(m_hits);
        m_printhits->printHits(sumh);
        
  /*      for (auto& hit : m_hits) {
            m_modules << hit.second.X() << " " << hit.second.Y() << " " << hit.second.Z() << std::endl;
        }*/
        
        std::cout << "after!!!" << std::endl;
        m_particles.put(m_particlecoll);
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
    
    
    return StatusCode::SUCCESS;
    
}
