#ifndef SIMG4FULL_FULLSIMACTIONS_H
#define SIMG4FULL_FULLSIMACTIONS_H

#include "G4VUserActionInitialization.hh"

/** @class FullSimActions SimG4Full/SimG4Full/FullSimActions.h FullSimActions.h
 *
 *  User action initialization for full simulation.
 *
 *  @author A. Zaborowska
 *  @author J. Lingemann (adding particle history)
 */

namespace sim {
class FullSimActions : public G4VUserActionInitialization {
public:
  FullSimActions(bool enableHistory);
  virtual ~FullSimActions();
  /// Create all user actions.
  virtual void Build() const final;

protected:
  /** Initializes G4UserTrackingActions
    * @out outAction the initialized user action
    */
  virtual SetUserAction(G4UserTrackingAction* outAction) const;

private:
  /// Flag whether or not to store particle history
  bool m_enableHistory;
};
}

#endif /* SIMG4FULL_FULLSIMACTIONS_H */
