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
      // Default units for EDM: GeV and cm
      // Default units for G4:  MeV and mm
      float length_unit = CLHEP::mm/CLHEP::cm;
      float mom_unit = CLHEP::MeV/CLHEP::GeV;
      core.P4.Px = g4dynamicparticle->GetMomentum().x()*mom_unit;
      core.P4.Py = g4dynamicparticle->GetMomentum().y()*mom_unit;
      core.P4.Pz = g4dynamicparticle->GetMomentum().z()*mom_unit;
      core.P4.Mass = g4dynamicparticle->GetMass()*mom_unit;
      core.Vertex.X = aTrack->GetVertexPosition().x()*length_unit;
      core.Vertex.Y = aTrack->GetVertexPosition().y()*length_unit;
      core.Vertex.Z = aTrack->GetVertexPosition().z()*length_unit;
   }
   return;
}
