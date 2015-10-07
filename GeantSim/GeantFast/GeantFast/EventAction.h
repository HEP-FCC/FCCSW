#ifndef GEANTFAST_EVENT_ACTION_H
#define GEANTFAST_EVENT_ACTION_H

#include "G4UserEventAction.hh"

#include "FWCore/DataHandle.h"
#include "datamodel/ParticleCollection.h"

/**
	@brief     Event action (before/after event processing)
 	@details   Defines the action at the beginning and at the end of each event. It is invoked by a G4EventManager when a G4Event object is sent (that contains primary vertices and particles created by the FCCPrimaryGeneratorAction).
 	@author    Anna Zaborowska
 */
class EventAction : public G4UserEventAction
{
  public:
   /**
      A default constructor.
    */
    EventAction();
    virtual ~EventAction();
   /**
     Defines the actions at the beginning of the event. It sets the FCCEventInformation with fSmear flag. It creates all the ntuples defined in FCCOutput singleton class.
    */
    virtual void BeginOfEventAction(const G4Event*);
   /**
      Defines the actions at the end of the event.
   */
    virtual void EndOfEventAction(const G4Event* aEvent);
    inline ParticleCollection* GetParticleCollection() const;
private:
   ParticleCollection* m_particleCollection;
};

inline ParticleCollection* EventAction::GetParticleCollection() const
{
   return m_particleCollection;
}
#endif
