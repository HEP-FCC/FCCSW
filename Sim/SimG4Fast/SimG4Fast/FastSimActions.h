#ifndef GEANT_FAST_ACTIONS_H
#define GEANT_FAST_ACTIONS_H

#include "G4VUserActionInitialization.hh"

/**
   @brief     User action initialization.
   @author    Anna Zaborowska
*/

class FastSimActions : public G4VUserActionInitialization {
public:
   FastSimActions();
   ~FastSimActions();
   /// A method where all  user actions are created.
   virtual void Build() const final;
};


#endif
