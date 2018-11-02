#include "SimG4Full/ParticleHistoryAction.h"

#include "SimG4Common/EventInformation.h"

#include "G4EventManager.hh"
#include "G4LorentzVector.hh"

namespace sim {

ParticleHistoryAction::ParticleHistoryAction(double aEnergyCut): m_energyCut(aEnergyCut) {}

void ParticleHistoryAction::PreUserTrackingAction(const G4Track* aTrack) {
  auto g4EvtMgr = G4EventManager::GetEventManager();
  auto evtinfo = dynamic_cast<sim::EventInformation*>(g4EvtMgr->GetUserInformation());
  G4LorentzVector prodPos(aTrack->GetGlobalTime() - aTrack->GetLocalTime(), aTrack->GetVertexPosition());
  G4LorentzVector endPos(aTrack->GetGlobalTime(), aTrack->GetPosition());
  if (selectSecondary(*aTrack, m_energyCut)) {
    evtinfo->addParticle(aTrack);
  }
}

void ParticleHistoryAction::PostUserTrackingAction(const G4Track* /*aTrack*/) {}

bool ParticleHistoryAction::selectSecondary(const G4Track& aTrack, double aEnergyCut) {
  G4LorentzVector p4(aTrack.GetMomentum(), aTrack.GetTotalEnergy());
  if (p4.e() < aEnergyCut) { 
    return false;
  }
  return true;
}
}
