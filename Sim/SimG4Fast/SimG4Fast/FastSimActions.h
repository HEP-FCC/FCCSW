#ifndef SIM_FASTSIMACTIONS_H
#define SIM_FASTSIMACTIONS_H

#include "G4VUserActionInitialization.hh"

/** @class FastSimActions FastSimActions.h SimG4Fast/FastSimActions.h
 *
   @brief     User action initialization.
   @author    Anna Zaborowska
*/

namespace sim {
class FastSimActions : public G4VUserActionInitialization {
public:
   FastSimActions();
   ~FastSimActions();
   /// A method where all  user actions are created.
   virtual void Build() const final;
};
}

#endif /* SIM_FASTSIMACTIONS_H */
