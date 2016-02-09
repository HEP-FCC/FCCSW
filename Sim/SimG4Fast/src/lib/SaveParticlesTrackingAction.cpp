#include "SimG4Fast/SaveParticlesTrackingAction.h"

//FCCSW
#include "SimG4Common/Units.h"
#include "SimG4Common/ParticleInformation.h"

// Geant4
#include "G4TrackingManager.hh"
#include "G4PrimaryParticle.hh"
#include "G4ThreeVector.hh"

namespace sim {
SaveParticlesTrackingAction::SaveParticlesTrackingAction():
  G4UserTrackingAction() {}

SaveParticlesTrackingAction::~SaveParticlesTrackingAction() {}

void SaveParticlesTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {
  if ( aTrack->GetTrackStatus() == fStopAndKill && aTrack->GetParentID()==0) {
    const G4DynamicParticle* g4dynamicparticle = aTrack->GetDynamicParticle();
    ParticleInformation* info = dynamic_cast<ParticleInformation*>(g4dynamicparticle->GetPrimaryParticle()->GetUserInformation());
    info->setEndStatus(1); // how it is defined ???? as in HepMC ?
    info->setEndMomentum(g4dynamicparticle->GetMomentum()*sim::g42edm::energy);
    info->setVertexPosition( aTrack->GetVertexPosition()*sim::g42edm::length);
  }
  return;
}
}
