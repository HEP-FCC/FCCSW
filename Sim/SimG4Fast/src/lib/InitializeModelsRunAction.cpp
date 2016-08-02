#include "SimG4Fast/InitializeModelsRunAction.h"

// FCCSW
#include "SimG4Fast/FastSimModelTracker.h"
#include "SimG4Interface/ISimG4ParticleSmearTool.h"

// Geant4
#include "G4RegionStore.hh"
#include "G4VFastSimulationModel.hh"
#include "G4TransportationManager.hh"

// Gaudi
#include "GaudiKernel/IToolSvc.h"

namespace sim {
InitializeModelsRunAction::InitializeModelsRunAction(const std::string& aSmearingToolName) :
  G4UserRunAction(),
  m_msgSvc("MessageSvc","InitializeModelsRunAction"),
  m_log(&(*m_msgSvc),"InitializeModelsRunAction"),
  m_toolSvc("ToolSvc","ToolSvc"),
  m_smearToolName(aSmearingToolName) {
  if( m_toolSvc->retrieveTool(aSmearingToolName, m_smearTool, 0, false).isFailure())
    throw GaudiException("Smearing tool "+aSmearingToolName+" not found",
                         "FastSimModelTracker", StatusCode::FAILURE);}

InitializeModelsRunAction::~InitializeModelsRunAction() {}

void InitializeModelsRunAction::BeginOfRunAction(const G4Run* /*aRun*/) {
  const std::vector<std::string> trackerNames = m_smearTool->volumeNames();
  G4LogicalVolume* world = (*G4TransportationManager::GetTransportationManager()->GetWorldsIterator())->GetLogicalVolume();
  for(const auto& trackerName: trackerNames) {
    for(int iter_region = 0; iter_region<world->GetNoDaughters(); ++iter_region) {
      if(world->GetDaughter(iter_region)->GetName().find(trackerName) != std::string::npos) {
        /// all G4Region objects are deleted by the G4RegionStore
        m_g4regions.emplace_back(new G4Region(world->GetDaughter(iter_region)->GetLogicalVolume()->GetName()+"_fastsim"));
        m_g4regions.back()->AddRootLogicalVolume(world->GetDaughter(iter_region)->GetLogicalVolume());
        m_models.emplace_back(new FastSimModelTracker(m_g4regions.back()->GetName(),m_g4regions.back(),m_smearToolName));
        m_log<<MSG::INFO<<"Attaching a Fast Simulation Model to the region "<<m_g4regions.back()->GetName()<<endmsg;
      }
    }
  }
  return;
}
}
