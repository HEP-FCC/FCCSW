#include "TrackingAction.h"
#include "G4TrackingManager.hh"
#include "G4PrimaryParticle.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "ParticleInformation.h"

TrackingAction::TrackingAction() : G4UserTrackingAction()
{}

TrackingAction::~TrackingAction()
{}

void TrackingAction::PreUserTrackingAction(const G4Track* /*aTrack*/)
{}

void TrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
   if ( aTrack->GetTrackStatus() == fStopAndKill && aTrack->GetParentID()==0)
   {
      const G4DynamicParticle* g4dynamicparticle = aTrack->GetDynamicParticle();
      ParticleHandle particle = dynamic_cast<ParticleInformation*>(g4dynamicparticle->GetPrimaryParticle()->GetUserInformation())->GetParticleHandle();
      BareParticle& core = particle.mod().Core;
      core.Type = g4dynamicparticle->GetPDGcode();
      core.Status = 1; // how it is defined ???? as in HepMC ?
      core.Charge = g4dynamicparticle->GetCharge();
      core.P4.Px = g4dynamicparticle->GetMomentum().x();
      core.P4.Py = g4dynamicparticle->GetMomentum().y();
      core.P4.Pz = g4dynamicparticle->GetMomentum().z();
      core.P4.Mass = g4dynamicparticle->GetMass();
      core.Vertex.X = aTrack->GetVertexPosition().x();
      core.Vertex.Y = aTrack->GetVertexPosition().y();
      core.Vertex.Z = aTrack->GetVertexPosition().z();
   }
   return;
}
