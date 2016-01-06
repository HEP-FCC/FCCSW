#ifndef GEANT_FULL_ACTIONS_H
#define GEANT_FULL_ACTIONS_H

#include "G4VUserActionInitialization.hh"

/**
   @brief     User action initialization.
   @author    Anna Zaborowska
*/

class FullSimActions : public G4VUserActionInitialization {
public:
   FullSimActions();
   ~FullSimActions();
   /// A method where all  user actions are created.
   virtual void Build() const final;
};


#endif
