#include "SimG4Full/ParticleHistoryAction.h"

#include "SimG4Common/EventInformation.h"

#include "G4EventManager.hh"
#include "G4LorentzVector.hh"

namespace sim {

ParticleHistoryAction::ParticleHistoryAction() {}

void ParticleHistoryAction::PreUserTrackingAction(const G4Track* aTrack) {}

void ParticleHistoryAction::PostUserTrackingAction(const G4Track* aTrack) {
  auto g4EvtMgr = G4EventManager::GetEventManager();
  auto evtinfo = dynamic_cast<sim::EventInformation*>(g4EvtMgr->GetUserInformation());
  G4LorentzVector prodPos(aTrack->GetGlobalTime() - aTrack->GetLocalTime(), aTrack->GetVertexPosition());
  G4LorentzVector endPos(aTrack->GetGlobalTime(), aTrack->GetPosition());
  if (selectSecondary(*aTrack)) {
    evtinfo->addParticle(aTrack);
  }
}

bool ParticleHistoryAction::selectSecondary(const G4Track& aTrack) {
  G4LorentzVector p4(aTrack.GetMomentum(), aTrack.GetTotalEnergy());
  if (p4.e() < 5.0) {  // this value is very arbitrary
    return false;
  }
  return true;
}
}
