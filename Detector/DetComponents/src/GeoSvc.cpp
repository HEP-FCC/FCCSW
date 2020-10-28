
#include "GeoSvc.h"
#include "GaudiKernel/Service.h"
#include "GeoConstruction.h"
#include "TGeoManager.h"

#include "DD4hep/Printout.h"

using namespace Gaudi;

DECLARE_COMPONENT(GeoSvc)

GeoSvc::GeoSvc(const std::string& name, ISvcLocator* svc)
: base_class(name, svc), m_dd4hepgeo(0), m_geant4geo(0) {}

GeoSvc::~GeoSvc() {
  if (m_dd4hepgeo){
      //m_dd4hepgeo->destroyInstance();
  }
}

StatusCode GeoSvc::initialize() {
  StatusCode sc = Service::initialize();
  if (!sc.isSuccess()) return sc;
  // Turn off TGeo printouts if appropriate for the msg level
  if (msgLevel() >= MSG::INFO) {
    TGeoManager::SetVerboseLevel(0);
  }
  uint printoutLevel = msgLevel();
  dd4hep::setPrintLevel(dd4hep::PrintLevel(printoutLevel));
  if (buildDD4HepGeo().isFailure())
    error() << "Could not build DD4Hep geometry" << endmsg;
  else
    info() <<  "DD4Hep geometry SUCCESSFULLY built" << endmsg;

  if (buildGeant4Geo().isFailure())
    error() << "Could not build Geant4 geometry" << endmsg;
  else
    info() << "Geant4 geometry SUCCESSFULLY built" << endmsg;
  // TODO: return failure
  return StatusCode::SUCCESS;
}

StatusCode GeoSvc::finalize() { return StatusCode::SUCCESS; }

StatusCode GeoSvc::buildDD4HepGeo() {
  // we retrieve the the static instance of the DD4HEP::Geometry
  m_dd4hepgeo = &(dd4hep::Detector::getInstance());
  m_dd4hepgeo->addExtension<IGeoSvc>(this);

  // load geometry
  for (auto& filename : m_xmlFileNames) {
    info() << "loading geometry from file:  '" << filename << "'" << endmsg;
    m_dd4hepgeo->fromCompact(filename);
  }
  m_dd4hepgeo->volumeManager();
  m_dd4hepgeo->apply("DD4hepVolumeManager", 0, 0);

  return StatusCode::SUCCESS;
}

dd4hep::Detector* GeoSvc::lcdd() { return (m_dd4hepgeo); }

dd4hep::DetElement GeoSvc::getDD4HepGeo() { return (lcdd()->world()); }

StatusCode GeoSvc::buildGeant4Geo() {
  std::shared_ptr<G4VUserDetectorConstruction> detector(new det::GeoConstruction(*lcdd()));
  m_geant4geo = detector;
  if (m_geant4geo) {
    return StatusCode::SUCCESS;
  } else
    return StatusCode::FAILURE;
}

G4VUserDetectorConstruction* GeoSvc::getGeant4Geo() { return (m_geant4geo.get()); }

