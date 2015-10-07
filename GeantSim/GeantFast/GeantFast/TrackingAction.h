#ifndef TRACKING_ACTION_H
#define TRACKING_ACTION_H

#include "G4UserTrackingAction.hh"
#include "FWCore/DataHandle.h"
#include "datamodel/ParticleCollection.h"

/**
  @brief     Tracking action (before/after track processing).
   @details   Defines the action at the start and at the end of processing of each track. The class needs to be set in G4RunManager::SetUserAction().
   @author    Anna Zaborowska
*/

class TrackingAction : public G4UserTrackingAction
{
  public:
   /**
      A default constructor.
    */
    TrackingAction();
   /**
      A constructor.
    */
    TrackingAction(DataHandle<ParticleCollection>* aPartColl);
    virtual ~TrackingAction();

   /**
     Defines the actions at the start of processing the track. It checks the pseudorapidity range and if the particle is primary.
    */
   virtual void  PreUserTrackingAction(const G4Track* track);
   /**
     Defines the actions at the end of processing the track. It saves the information of MC data (PDG code, initial momentum), tracker (momentum), EMCal and HCal (energy deposit and its position) as well as resolution and efficiency for all the detectors.
    */
   virtual void  PostUserTrackingAction(const G4Track* track);
private:
   DataHandle<ParticleCollection>* m_particleCollection;
};

#endif
