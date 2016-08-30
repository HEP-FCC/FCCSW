#include "SimG4FastSimRegionTracker.h"

// FCCSW
#include "SimG4Fast/FastSimModelTracker.h"

// Geant4
#include "G4RegionStore.hh"
#include "G4VFastSimulationModel.hh"
#include "G4TransportationManager.hh"

DECLARE_TOOL_FACTORY(SimG4FastSimRegionTracker)

SimG4FastSimRegionTracker::SimG4FastSimRegionTracker(const std::string& type, const std::string& name, const IInterface* parent) :
GaudiTool(type, name, parent) {
  declareInterface<ISimG4RegionTool>(this);
  declareProperty("smearing", m_smearTool);
  declarePrivateTool(m_smearTool);
}

SimG4FastSimRegionTracker::~SimG4FastSimRegionTracker() {}

StatusCode SimG4FastSimRegionTracker::initialize() {
  if(GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if(!m_smearTool.retrieve()) {
    error()<<"Smearing tool cannot be retieved"<<endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4FastSimRegionTracker::finalize() {
  return GaudiTool::finalize();
}

StatusCode SimG4FastSimRegionTracker::create() {
  const std::vector<std::string> trackerNames = m_smearTool->volumeNames();
  G4LogicalVolume* world = (*G4TransportationManager::GetTransportationManager()->GetWorldsIterator())->GetLogicalVolume();
  for(const auto& trackerName: trackerNames) {
    auto smearToolName = m_smearTool->name();
    auto thisName = this->name();
    smearToolName.erase(smearToolName.begin(),smearToolName.begin()+thisName.length()+1);
    for(int iter_region = 0; iter_region<world->GetNoDaughters(); ++iter_region) {
      if(world->GetDaughter(iter_region)->GetName().find(trackerName) != std::string::npos) {
        /// all G4Region objects are deleted by the G4RegionStore
        m_g4regions.emplace_back(new G4Region(world->GetDaughter(iter_region)->GetLogicalVolume()->GetName()+"_fastsim"));
        m_g4regions.back()->AddRootLogicalVolume(world->GetDaughter(iter_region)->GetLogicalVolume());
        m_models.emplace_back(new sim::FastSimModelTracker(m_g4regions.back()->GetName(),m_g4regions.back(),m_smearTool->type(), smearToolName));
        info()<<"Attaching a Tracker fast simulation model to the region "<<m_g4regions.back()->GetName()<<endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}
