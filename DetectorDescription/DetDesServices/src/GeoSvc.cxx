//
//  GeoSvc.cxx
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#include "DetDesServices/GeoSvc.h"

using namespace Gaudi;

DECLARE_COMPONENT(GeoSvc)

GeoSvc::GeoSvc(const std::string& name, ISvcLocator* svc) :
base_class(name, svc),
m_dd4hepgeo(0),
m_geant4geo(0),
m_xmlFileName("file:DetectorDescription/Detectors/compact/FCCTestTracker.xml"), //over joboptions
m_log(msgSvc(), name)
{}

GeoSvc::~GeoSvc()
{
    m_dd4hepgeo->destroyInstance();
}

StatusCode GeoSvc::initialize()
{
    if (buildDD4HepGeo().isFailure())
        m_log << MSG::ERROR << "Could not build DD4Hep geometry" << endmsg;
    else
        m_log << MSG::INFO << "DD4Hep geometry SUCCESSFULLY built" << endmsg;
    
    if (buildGeant4Geo().isFailure())
        m_log << MSG::ERROR << "Could not build Geant4 geometry" << endmsg;
    else
        m_log << MSG::INFO << "Geant4 geometry SUCCESSFULLY built" << endmsg;
    
    return StatusCode::SUCCESS;
}

StatusCode GeoSvc::finalize()
{
    return StatusCode::SUCCESS;
}

StatusCode GeoSvc::buildDD4HepGeo()
{
    // we retrieve the the static instance of the DD4HEP::Geometry
    m_dd4hepgeo = &(DD4hep::Geometry::LCDD::getInstance());
    m_dd4hepgeo->addExtension<IGeoSvc>(this);
    
    //load geometry
    m_log << MSG::INFO << "loading geometry from file:  '" << m_xmlFileName << "'" << endmsg;
    char* arg = (char*) m_xmlFileName.c_str();
    m_dd4hepgeo->apply("DD4hepXMLLoader", 1, &arg);
    m_dd4hepgeo->volumeManager();
    m_dd4hepgeo->apply("DD4hepVolumeManager",0,0);
    
    return StatusCode::SUCCESS;
}

DD4hep::Geometry::LCDD* GeoSvc::lcdd()
{
    return (m_dd4hepgeo);
}

DD4hep::Geometry::DetElement GeoSvc::getDD4HepGeo()
{
    return (lcdd()->world());
}

StatusCode GeoSvc::buildGeant4Geo()
{
    std::shared_ptr<G4VUserDetectorConstruction> detector(new DD4hep::Simulation::Geant4DetectorConstruction(*lcdd()));
    m_geant4geo = detector;
    if (m_geant4geo) {
        return StatusCode::SUCCESS;
    }
    else return StatusCode::FAILURE;
}

G4VUserDetectorConstruction* GeoSvc::getGeant4Geo()
{
    return (m_geant4geo.get());
}