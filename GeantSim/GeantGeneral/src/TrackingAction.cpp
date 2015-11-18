#include "TrackingAction.h"

// Geant4
#include "G4TrackingManager.hh"
#include "G4PrimaryParticle.hh"
#include "G4ThreeVector.hh"

//FCCSW
#include "Units.h"
#include "ParticleInformation.h"

TrackingAction::TrackingAction() : G4UserTrackingAction() {}

TrackingAction::~TrackingAction() {}

void TrackingAction::PostUserTrackingAction(const G4Track* aTrack) {
   if ( aTrack->GetTrackStatus() == fStopAndKill && aTrack->GetParentID()==0) {
      const G4DynamicParticle* g4dynamicparticle = aTrack->GetDynamicParticle();
      ParticleHandle& particle = dynamic_cast<ParticleInformation*>(g4dynamicparticle->GetPrimaryParticle()->GetUserInformation())->GetParticleHandle();
      BareParticle& core = particle.mod().Core;
      core.Type = g4dynamicparticle->GetPDGcode();
      core.Status = 1; // how it is defined ???? as in HepMC ?
      core.Charge = g4dynamicparticle->GetCharge();
      core.P4.Px = g4dynamicparticle->GetMomentum().x()*g42edm::energy;
      core.P4.Py = g4dynamicparticle->GetMomentum().y()*g42edm::energy;
      core.P4.Pz = g4dynamicparticle->GetMomentum().z()*g42edm::energy;
      core.P4.Mass = g4dynamicparticle->GetMass()*g42edm::energy;
      core.Vertex.X = aTrack->GetVertexPosition().x()*g42edm::length;
      core.Vertex.Y = aTrack->GetVertexPosition().y()*g42edm::length;
      core.Vertex.Z = aTrack->GetVertexPosition().z()*g42edm::length;
   }
   return;
}
