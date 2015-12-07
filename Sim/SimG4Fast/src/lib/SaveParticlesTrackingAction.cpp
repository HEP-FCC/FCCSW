#include "SimG4Fast/SaveParticlesTrackingAction.h"

// Geant4
#include "G4TrackingManager.hh"
#include "G4PrimaryParticle.hh"
#include "G4ThreeVector.hh"

//FCCSW
#include "SimG4Common/Units.h"
#include "SimG4Common/ParticleInformation.h"

SaveParticlesTrackingAction::SaveParticlesTrackingAction() : G4UserTrackingAction() {}

SaveParticlesTrackingAction::~SaveParticlesTrackingAction() {}

void SaveParticlesTrackingAction::PostUserTrackingAction(const G4Track* aTrack) {
   if ( aTrack->GetTrackStatus() == fStopAndKill && aTrack->GetParentID()==0) {
      const G4DynamicParticle* g4dynamicparticle = aTrack->GetDynamicParticle();
      ParticleHandle& particle = dynamic_cast<sim::ParticleInformation*>(g4dynamicparticle->GetPrimaryParticle()->GetUserInformation())->GetParticleHandle();
      BareParticle& core = particle.mod().Core;
      core.Type = g4dynamicparticle->GetPDGcode();
      core.Status = 1; // how it is defined ???? as in HepMC ?
      core.Charge = g4dynamicparticle->GetCharge();
      core.P4.Px = g4dynamicparticle->GetMomentum().x()*sim::g42edm::energy;
      core.P4.Py = g4dynamicparticle->GetMomentum().y()*sim::g42edm::energy;
      core.P4.Pz = g4dynamicparticle->GetMomentum().z()*sim::g42edm::energy;
      core.P4.Mass = g4dynamicparticle->GetMass()*sim::g42edm::energy;
      core.Vertex.X = aTrack->GetVertexPosition().x()*sim::g42edm::length;
      core.Vertex.Y = aTrack->GetVertexPosition().y()*sim::g42edm::length;
      core.Vertex.Z = aTrack->GetVertexPosition().z()*sim::g42edm::length;
   }
   return;
}
