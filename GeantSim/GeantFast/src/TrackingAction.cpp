#include "TrackingAction.h"

#include <iostream>
#include "G4Track.hh"
#include "G4DynamicParticle.hh"
#include "G4PrimaryParticle.hh"

TrackingAction::TrackingAction() : G4UserTrackingAction()
{}

// TrackingAction::TrackingAction(ParticleCollection* aPartColl) :
//    G4UserTrackingAction()
// {
//    m_particleCollection = aPartColl;
// }

TrackingAction::~TrackingAction()
{}

void TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
   // ParticleHandle outptc = particles->create();
   // BareParticle& core = outptc.mod().Core;
   // core.Type = aTrack->GetDynamicParticle()->GetPDGcode();
   // core.Status = 0;//ptc.status();
   // core.P4.Px = aTrack->GetMomentum().x();
   // core.P4.Py = aTrack->GetMomentum().y();
   // core.P4.Pz = aTrack->GetMomentum().z();
   // core.P4.Mass = aTrack->GetDynamicParticle()->GetMass();
   // m_particleCollection->put(particles);
}

void TrackingAction::PostUserTrackingAction(const G4Track* aTrack){}
