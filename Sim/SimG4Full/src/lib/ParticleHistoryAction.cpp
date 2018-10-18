#include "SimG4Full/ParticleHistoryAction.h"

#include "SimG4Common/EventInformation.h"

#include "G4EventManager.hh"
#include "G4LorentzVector.hh"

namespace sim {

ParticleHistoryAction::ParticleHistoryAction(double aEnergyCut, const G4String& excludeTracks)
    : m_energyCut(aEnergyCut), m_excludeTracks(excludeTracks) {}

void ParticleHistoryAction::PreUserTrackingAction(const G4Track* aTrack) {
  auto g4EvtMgr = G4EventManager::GetEventManager();
  auto evtinfo = dynamic_cast<sim::EventInformation*>(g4EvtMgr->GetUserInformation());
  G4LorentzVector prodPos(aTrack->GetGlobalTime() - aTrack->GetLocalTime(), aTrack->GetVertexPosition());
  G4LorentzVector endPos(aTrack->GetGlobalTime(), aTrack->GetPosition());
  if (selectParticle(*aTrack, m_energyCut, m_excludeTracks)) {
    evtinfo->addParticle(aTrack);
  }
}

void ParticleHistoryAction::PostUserTrackingAction(const G4Track* /*aTrack*/) {}


bool ParticleHistoryAction::selectParticle(const G4Track& aTrack, double aEnergyCut, const G4String& excludeTracks) {
  G4LorentzVector p4(aTrack.GetMomentum(), aTrack.GetTotalEnergy());
  if (p4.e() < aEnergyCut) {
    return false;
  }
  // possibily exclude tracks, if flag is set
  if (aTrack.GetUserInformation()) {
    // check if it is the correct flag
    if (aTrack.GetUserInformation()->GetType() == excludeTracks) {
      return false;
    }
  }
  return true;
}
}
