#include "SimG4UserLimitRegion.h"

// Geant4
#include "G4RegionStore.hh"
#include "G4LogicalVolume.hh"
#include "G4TransportationManager.hh"
#include "G4UserLimits.hh"

DECLARE_TOOL_FACTORY(SimG4UserLimitRegion)

SimG4UserLimitRegion::SimG4UserLimitRegion(const std::string& type, const std::string& name,
                                                     const IInterface* parent)
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
  for (const auto& trackerName : m_volumeNames) {
    for (int iter_region = 0; iter_region < world->GetNoDaughters(); ++iter_region) {
      if (world->GetDaughter(iter_region)->GetName().find(trackerName) != std::string::npos) {
        /// all G4Region objects are deleted by the G4RegionStore
        m_g4regions.emplace_back(
            new G4Region(world->GetDaughter(iter_region)->GetLogicalVolume()->GetName() + "_userLimits"));
        m_g4regions.back()->AddRootLogicalVolume(world->GetDaughter(iter_region)->GetLogicalVolume());
        // TODO add to shared_ptr vector so it is deleted (it won't be by Geant)
        m_g4regions.back()->SetUserLimits(new G4UserLimits(m_maxStep, m_maxTrack, m_maxTrack, m_minKineticEnergy, m_minRange));
        info() << "Creating user limits in the region " << m_g4regions.back()->GetName() << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}
