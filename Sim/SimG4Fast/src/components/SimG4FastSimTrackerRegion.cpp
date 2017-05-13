#include "SimG4FastSimTrackerRegion.h"

// FCCSW
#include "SimG4Fast/FastSimModelTracker.h"

// Geant4
#include "G4RegionStore.hh"
#include "G4TransportationManager.hh"
#include "G4VFastSimulationModel.hh"

DECLARE_TOOL_FACTORY(SimG4FastSimTrackerRegion)

SimG4FastSimTrackerRegion::SimG4FastSimTrackerRegion(const std::string& type, const std::string& name,
                                                     const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ISimG4RegionTool>(this);
  declareProperty("smearing", m_smearTool,
                  "Pointer to a smearing tool, to retrieve tracker configuration (names of volumes)");
}

SimG4FastSimTrackerRegion::~SimG4FastSimTrackerRegion() {}

StatusCode SimG4FastSimTrackerRegion::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if (m_volumeNames.size() == 0) {
    error() << "No detector name is specified for the parametrisation" << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_minMomentum > m_maxMomentum) {
    error() << "Momentum range is not defined properly" << endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_smearTool.retrieve()) {
    error() << "Smearing tool cannot be retieved" << endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_smearTool->checkConditions(m_minMomentum, m_maxMomentum, m_maxEta)) {
    error() << "Wrong parameters for this smearing tool" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4FastSimTrackerRegion::finalize() { return GaudiTool::finalize(); }

StatusCode SimG4FastSimTrackerRegion::create() {
  G4LogicalVolume* world =
      (*G4TransportationManager::GetTransportationManager()->GetWorldsIterator())->GetLogicalVolume();
  for (const auto& trackerName : m_volumeNames) {
    for (int iter_region = 0; iter_region < world->GetNoDaughters(); ++iter_region) {
      if (world->GetDaughter(iter_region)->GetName().find(trackerName) != std::string::npos) {
        /// all G4Region objects are deleted by the G4RegionStore
        m_g4regions.emplace_back(
            new G4Region(world->GetDaughter(iter_region)->GetLogicalVolume()->GetName() + "_fastsim"));
        m_g4regions.back()->AddRootLogicalVolume(world->GetDaughter(iter_region)->GetLogicalVolume());
        m_models.emplace_back(new sim::FastSimModelTracker(m_g4regions.back()->GetName(), m_g4regions.back(),
                                                           m_smearTool, m_minMomentum, m_maxMomentum, m_maxEta));
        info() << "Attaching a Tracker fast simulation model to the region " << m_g4regions.back()->GetName() << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}
