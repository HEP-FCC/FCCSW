#include "SimG4Fast/InitializeModelsRunAction.h"

// FCCSW
#include "SimG4Fast/FastSimModelTracker.h"

// Geant4
#include "G4RegionStore.hh"
#include "G4VFastSimulationModel.hh"
#include "G4TransportationManager.hh"

namespace sim {
InitializeModelsRunAction::InitializeModelsRunAction(const std::string& aSmearingToolName) :
  G4UserRunAction(),
  m_msgSvc("MessageSvc","InitializeModelsRunAction"),
  m_log(&(*m_msgSvc),"InitializeModelsRunAction"),
  m_smearToolName(aSmearingToolName) {}

InitializeModelsRunAction::~InitializeModelsRunAction() {}

void InitializeModelsRunAction::BeginOfRunAction(const G4Run* /*aRun*/) {
  G4LogicalVolume* world = (*G4TransportationManager::GetTransportationManager()->GetWorldsIterator())->GetLogicalVolume();
  for(int iter_region = 0; iter_region<world->GetNoDaughters(); ++iter_region) {
    if(world->GetDaughter(iter_region)->GetName().find("Tracker") != std::string::npos) {
      /// all G4Region objects are deleted by the G4RegionStore
      m_g4regions.emplace_back(new G4Region(world->GetDaughter(iter_region)->GetLogicalVolume()->GetName()+"_fastsim"));
      m_g4regions.back()->AddRootLogicalVolume(world->GetDaughter(iter_region)->GetLogicalVolume());
      m_models.emplace_back(new FastSimModelTracker(m_g4regions.back()->GetName(),m_g4regions.back(),m_smearToolName));
      m_log<<MSG::INFO<<"Attaching a Fast Simulation Model to the region "<<m_g4regions.back()->GetName()<<endmsg;
    }
  }
  return;
}
}
