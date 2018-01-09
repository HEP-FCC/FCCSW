//
//  GeoSvc.cxx
//
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#include "GeoSvc.h"
#include "GaudiKernel/Service.h"
#include "GeoConstruction.h"

#include "DD4hep/Printout.h"

using namespace Gaudi;

DECLARE_COMPONENT(GeoSvc)

GeoSvc::GeoSvc(const std::string& name, ISvcLocator* svc)
: base_class(name, svc), m_incidentSvc("IncidentSvc", "GeoSvc"), m_dd4hepgeo(0), m_geant4geo(0), m_log(msgSvc(), name), m_failureFlag(false) {}

GeoSvc::~GeoSvc() { m_dd4hepgeo->destroyInstance(); }

StatusCode GeoSvc::initialize() {
  StatusCode sc = Service::initialize();
  if (!sc.isSuccess()) return sc;
  uint printoutLevel = msgLevel();
  DD4hep::setPrintLevel(DD4hep::PrintLevel(printoutLevel));
  m_incidentSvc->addListener(this, "GeometryFailure");
  if (buildDD4HepGeo().isFailure())
    m_log << MSG::ERROR << "Could not build DD4Hep geometry" << endmsg;
  else
    m_log << MSG::INFO << "DD4Hep geometry SUCCESSFULLY built" << endmsg;

  if (buildGeant4Geo().isFailure())
    m_log << MSG::ERROR << "Could not build Geant4 geometry" << endmsg;
  else
    m_log << MSG::INFO << "Geant4 geometry SUCCESSFULLY built" << endmsg;
  if (m_failureFlag) {
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode GeoSvc::finalize() { return StatusCode::SUCCESS; }

StatusCode GeoSvc::buildDD4HepGeo() {
  // we retrieve the the static instance of the DD4HEP::Geometry
  m_dd4hepgeo = &(DD4hep::Geometry::LCDD::getInstance());
  m_dd4hepgeo->addExtension<IGeoSvc>(this);

  // load geometry
  for (auto& filename : m_xmlFileNames) {
    m_log << MSG::INFO << "loading geometry from file:  '" << filename << "'" << endmsg;
    m_dd4hepgeo->fromCompact(filename);
  }
  m_dd4hepgeo->volumeManager();
  m_dd4hepgeo->apply("DD4hepVolumeManager", 0, 0);

  return StatusCode::SUCCESS;
}

DD4hep::Geometry::LCDD* GeoSvc::lcdd() { return (m_dd4hepgeo); }

DD4hep::Geometry::DetElement GeoSvc::getDD4HepGeo() { return (lcdd()->world()); }

StatusCode GeoSvc::buildGeant4Geo() {
  std::shared_ptr<G4VUserDetectorConstruction> detector(new det::GeoConstruction(*lcdd()));
  m_geant4geo = detector;
  if (m_geant4geo) {
    return StatusCode::SUCCESS;
  } else
    return StatusCode::FAILURE;
}

G4VUserDetectorConstruction* GeoSvc::getGeant4Geo() { return (m_geant4geo.get()); }

void GeoSvc::handle(const Incident& inc) {
  error() << "Handling incident '" << inc.type() << "'" << endmsg;
  if (!inc.type().compare("GeometryFailure")) {
    m_failureFlag = true;
  }
}
