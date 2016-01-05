#ifndef SIM_FULLSIMACTIONS_H
#define SIM_FULLSIMACTIONS_H

#include "G4VUserActionInitialization.hh"

/** @class FullSimActions FullSimActions.h SimG4Full/FullSimActions.h
 *
 *  User action initialization for full simulation.
 *
 *  @author    Anna Zaborowska
*/

namespace sim {
class FullSimActions : public G4VUserActionInitialization {
public:
   FullSimActions();
   ~FullSimActions();
   /// A method where all  user actions are created.
   virtual void Build() const final;
};
}

#endif /* SIM_FULLSIMACTIONS_H */
