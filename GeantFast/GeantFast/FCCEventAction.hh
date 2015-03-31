#ifndef FCC_EVENT_ACTION_H
#define FCC_EVENT_ACTION_H

#include "G4UserEventAction.hh"
#include "globals.hh"

/**
  @brief     Event action (before/after event processing)
   @details   Defines the action at the beginning and at the end of each event. It is invoked by a G4EventManager when a G4Event object is sent (that contains primary vertices and particles created by the FCCPrimaryGeneratorAction).
   @author    Anna Zaborowska
 */
class FCCEventAction : public G4UserEventAction
{
public:
   /**
      A default constructor. Sets the flag fSmear to true indicating that smearing will be performed.
    */
    FCCEventAction();
    virtual ~FCCEventAction();
   /**
     Defines the actions at the beginning of the event. It creates all the ntuples defined in FCCOutput singleton class.
    */
    virtual void BeginOfEventAction(const G4Event*);
   /**
      Defines the actions at the end of the event.
   */
    virtual void EndOfEventAction(const G4Event*);

};

#endif
