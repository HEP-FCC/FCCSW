#include "TrackingAction.h"

#include <iostream>
#include "G4Track.hh"
#include "G4DynamicParticle.hh"
#include "G4PrimaryParticle.hh"

TrackingAction::TrackingAction() : G4UserTrackingAction(){}

TrackingAction::~TrackingAction(){}

void TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{}

void TrackingAction::PostUserTrackingAction(const G4Track* aTrack){}
