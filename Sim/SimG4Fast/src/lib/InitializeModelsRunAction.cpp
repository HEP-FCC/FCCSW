#include "SimG4Fast/InitializeModelsRunAction.h"

// Geant4
#include "G4RegionStore.hh"
#include "G4VFastSimulationModel.hh"
#include "G4TransportationManager.hh"

// FCCSW
#include "SimG4Fast/FastSimModelTracker.h"

namespace sim {
InitializeModelsRunAction::InitializeModelsRunAction(const std::string& aSmearingToolName) :
  G4UserRunAction(), m_smearToolName(aSmearingToolName),
  m_msgSvc("MessageSvc","InitializeModelsRunAction"),m_log(&(*m_msgSvc),"InitializeModelsRunAction") {}

InitializeModelsRunAction::~InitializeModelsRunAction() {}

void InitializeModelsRunAction::BeginOfRunAction(const G4Run* aRun) {
  G4LogicalVolume* world = (*G4TransportationManager::GetTransportationManager()->GetWorldsIterator())->GetLogicalVolume();
   for(int iter_region = 0; iter_region<world->GetNoDaughters(); ++iter_region) {
      // TO DO: proper integration with DD4hep to resolve where to attach a fast sim model
      if(world->GetDaughter(iter_region)->GetName().find("Tracker") != std::string::npos) {
         /// deleted by the G4RegionStore
         m_g4regions.emplace_back(new G4Region(world->GetDaughter(iter_region)->GetLogicalVolume()->GetName()+"_fastsim"));
         m_g4regions.back()->AddRootLogicalVolume(world->GetDaughter(iter_region)->GetLogicalVolume());
         m_models.emplace_back(new FastSimModelTracker(m_g4regions.back()->GetName(),m_g4regions.back(),m_smearToolName));
         m_log<<MSG::INFO<<"Attaching a Fast Simulation Model to the region "<<m_g4regions.back()->GetName()<<endmsg;
      }
   }
  return;
}
}
