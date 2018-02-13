#ifndef SIMG4FULL_PARTICLEHISTORYEVENTACTION_H
#define SIMG4FULL_PARTICLEHISTORYEVENTACTION_H

#include "G4UserEventAction.hh"


namespace sim {
class ParticleHistoryEventAction : public G4UserEventAction {
public:
  ParticleHistoryEventAction();
  virtual ~ParticleHistoryEventAction() = default;

  virtual void  BeginOfEventAction ( const G4Event *anEvent);
  virtual void  EndOfEventAction (const G4Event *anEvent);

};
}

#endif /* SIMG4FULL_PARTICLEHISTORYEVENTACTION_H */
