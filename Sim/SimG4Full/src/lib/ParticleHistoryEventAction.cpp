#include "SimG4Full/ParticleHistoryEventAction.h"

#include "SimG4Common/EventInformation.h"

#include "G4EventManager.hh"
#include "G4LorentzVector.hh"
#include "G4Event.hh"

namespace sim {

ParticleHistoryEventAction::ParticleHistoryEventAction() {}

void  ParticleHistoryEventAction::BeginOfEventAction (const G4Event * /*anEvent*/) {

  auto eventInfo = new sim::EventInformation();
  G4EventManager::GetEventManager()->SetUserInformation(eventInfo);
}
  
void  ParticleHistoryEventAction::EndOfEventAction (const G4Event * /*anEvent*/) {}

}
