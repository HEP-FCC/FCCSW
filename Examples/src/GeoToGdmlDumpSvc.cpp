#include "GeoToGdmlDumpSvc.h"

// Geant
#include "G4GDMLParser.hh"
#include "G4TransportationManager.hh"

DECLARE_SERVICE_FACTORY(GeoToGdmlDumpSvc)

GeoToGdmlDumpSvc::GeoToGdmlDumpSvc(const std::string& aName, ISvcLocator* aSL) : base_class(aName, aSL) {}

StatusCode GeoToGdmlDumpSvc::initialize() {
  if (Service::initialize().isFailure()) {
    error() << "Unable to initialize Service()" << endmsg;
    return StatusCode::FAILURE;
  }
  // dump geometry to gdml
  G4GDMLParser parser;
  parser.Write(m_gdmlFileName.value(),
               G4TransportationManager::GetTransportationManager()
                   ->GetNavigatorForTracking()
                   ->GetWorldVolume()
                   ->GetLogicalVolume());
  return StatusCode::SUCCESS;
}

StatusCode GeoToGdmlDumpSvc::finalize() { return Service::finalize(); }
