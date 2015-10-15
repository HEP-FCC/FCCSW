#ifndef ACTION_INITIALIZATION_H
#define ACTION_INITIALIZATION_H

#include "G4VUserActionInitialization.hh"
#include "globals.hh"

/**
  @brief     Actions initialization.
   @details   Initialization of all user defined actions as well as mandatory particle generator. Based on G4 examples/extended/parametrisations/Par01/include/Par01ActionInitialization.hh.
   @author    Anna Zaborowska
 */


class ActionInitialization : public G4VUserActionInitialization
{
  public:
    /**
      A default constructor. Sets the output file name fFileName (passed to RunAction) to DefaultOutput.root as well as flag fSmear (passed to EventAction) to true indicating that smearing will be performed.
    */
    ActionInitialization();
    /**
      A constructor.  Sets the flag fSmear (passed to EventAction) to true indicating that smearing will be performed.
         @param aOutName The output file name passed to RunAction.
    */
   ActionInitialization(const G4String aOutName);
   virtual ~ActionInitialization();

    virtual void BuildForMaster() const;
    /**
      A method where all the user actions are created. One of them, PrimaryGeneratorAction is a mandatory class.
    */
    virtual void Build() const;
private:
    /**
      An output file name. Passed in Build() to the RunAction.
    */
   G4String fFileName;
};

#endif

