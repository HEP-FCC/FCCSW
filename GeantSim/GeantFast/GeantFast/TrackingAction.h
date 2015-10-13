#ifndef TRACKING_ACTION_H
#define TRACKING_ACTION_H

#include "G4UserTrackingAction.hh"

/// @brief     Tracking action (before/after track processing).
/// @details   Defines the action at the start and at the end of processing of each track. The class needs to be set by G4RunManager::SetUserAction().
/// @author    Anna Zaborowska

class TrackingAction : public G4UserTrackingAction
{
public:
   /// A default constructor.
   TrackingAction();
   virtual ~TrackingAction();
   /// Defines the actions at the start of processing the track.
   virtual void  PreUserTrackingAction(const G4Track*);
   /// Defines the actions at the end of processing the track.
   virtual void  PostUserTrackingAction(const G4Track*);

};
#endif


