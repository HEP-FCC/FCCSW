#ifndef SIMG4FULL_PARTICLEHISTORYEVENTACTION_H
#define SIMG4FULL_PARTICLEHISTORYEVENTACTION_H

#include "G4UserEventAction.hh"

/** @class ParticleHistoryEventAction SimG4Full/SimG4Full/ParticleHistoryEventAction.h ParticleHistoryEventAction.h
 *
 *  User event action that creates the EventInformation class that holds information on secondaries
 *
 *  @author V. Volkl
 */

namespace sim {
class ParticleHistoryEventAction : public G4UserEventAction {
public:
  ParticleHistoryEventAction();
  virtual ~ParticleHistoryEventAction() = default;

  /// EventInformation data structure is created here
  virtual void  BeginOfEventAction ( const G4Event *anEvent);
  /// empty action 
  virtual void  EndOfEventAction (const G4Event *anEvent);

};
}

#endif /* SIMG4FULL_PARTICLEHISTORYEVENTACTION_H */
