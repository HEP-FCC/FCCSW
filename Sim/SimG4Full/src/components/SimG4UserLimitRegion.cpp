#include "SimG4UserLimitRegion.h"

// Geant4
#include "G4LogicalVolume.hh"
#include "G4RegionStore.hh"
#include "G4TransportationManager.hh"

#include "GaudiKernel/SystemOfUnits.h"

DECLARE_TOOL_FACTORY(SimG4UserLimitRegion)

SimG4UserLimitRegion::SimG4UserLimitRegion(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ISimG4RegionTool>(this);
}

SimG4UserLimitRegion::~SimG4UserLimitRegion() {}


StatusCode SimG4UserLimitRegion::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if (m_volumeNames.size() == 0) {
    error() << "No detector name is specified for the parametrisation" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4UserLimitRegion::finalize() { return GaudiTool::finalize(); }

StatusCode SimG4UserLimitRegion::create() {
  G4LogicalVolume* world =
      (*G4TransportationManager::GetTransportationManager()->GetWorldsIterator())->GetLogicalVolume();
  // Support two usecases: 
  // (a) if user specifies world, set limits for everything
  if (std::find(m_volumeNames.begin(), m_volumeNames.end(), "world") != m_volumeNames.end()) {
    m_userLimits.emplace_back(new G4UserLimits(m_maxStep / Gaudi::Units::mm * CLHEP::mm,
                                               m_maxTrack / Gaudi::Units::mm * CLHEP::mm,
                                               m_maxTime / Gaudi::Units::s * CLHEP::s,
                                               m_minKineticEnergy / Gaudi::Units::MeV * CLHEP::MeV,
                                               m_minRange / Gaudi::Units::mm * CLHEP::mm));
          world->SetUserLimits(m_userLimits.back().get());
          info() << "Creating user limits for world" << endmsg;
  // (b) if individiual volumenames are specified, try to find them and set limits for them.
  } else {
  for (const auto& volumeName : m_volumeNames) {
      for (int iter_region = 0; iter_region < world->GetNoDaughters(); ++iter_region) {
        if (world->GetDaughter(iter_region)->GetName().find(volumeName) != std::string::npos) {
          /// all G4Region objects are deleted by the G4RegionStore
          m_g4regions.emplace_back(
              new G4Region(world->GetDaughter(iter_region)->GetLogicalVolume()->GetName() + "_userLimits"));
          m_g4regions.back()->AddRootLogicalVolume(world->GetDaughter(iter_region)->GetLogicalVolume());
          m_userLimits.emplace_back(new G4UserLimits(m_maxStep / Gaudi::Units::mm * CLHEP::mm,
                                                     m_maxTrack / Gaudi::Units::mm * CLHEP::mm,
                                                     m_maxTime / Gaudi::Units::s * CLHEP::s,
                                                     m_minKineticEnergy / Gaudi::Units::MeV * CLHEP::MeV,
                                                     m_minRange / Gaudi::Units::mm * CLHEP::mm));
          m_g4regions.back()->SetUserLimits(m_userLimits.back().get());
          info() << "Creating user limits in the region " << m_g4regions.back()->GetName() << endmsg;
        }
      }
    }
  if (m_g4regions.size() != m_volumeNames.size()) {
    error() << "Regions  were not created for all the volumes" << endmsg;
    return StatusCode::FAILURE;
  }
  }
  return StatusCode::SUCCESS;
}
