#ifndef SIMG4FULL_FULLSIMACTIONS_H
#define SIMG4FULL_FULLSIMACTIONS_H

#include "G4VUserActionInitialization.hh"

/** @class FullSimActions SimG4Full/SimG4Full/FullSimActions.h FullSimActions.h
 *
 *  User action initialization for full simulation.
 *
 *  @author Anna Zaborowska
 */

namespace sim {
class FullSimActions : public G4VUserActionInitialization {
public:
  FullSimActions();
  virtual ~FullSimActions();
  /// Create all user actions.
  virtual void Build() const final;
};
}

#endif /* SIMG4FULL_FULLSIMACTIONS_H */
