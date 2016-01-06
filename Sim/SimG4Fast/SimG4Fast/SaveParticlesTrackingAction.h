#ifndef GEANT_SAVEPARTICLES_TRACKINGACTION_H
#define GEANT_SAVEPARTICLES_TRACKINGACTION_H

#include "G4UserTrackingAction.hh"

/**
   @brief     Tracking action (before/after Geant track processing).
   @details   Defines the action at the start and at the end of processing of each track. The class needs to be set by G4RunManager::SetUserAction().
   @author    Anna Zaborowska
*/

class SaveParticlesTrackingAction : public G4UserTrackingAction {
public:
   SaveParticlesTrackingAction();
   virtual ~SaveParticlesTrackingAction();
   /// Defines the actions at the end of processing the track.
   virtual void  PostUserTrackingAction(const G4Track*) final;

};
#endif


