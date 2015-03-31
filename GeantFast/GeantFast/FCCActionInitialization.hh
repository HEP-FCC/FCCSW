#ifndef FCC_ACTION_INITIALIZATION_H
#define FCC_ACTION_INITIALIZATION_H

#include "G4VUserActionInitialization.hh"
#include "globals.hh"

/**
  @brief     Actions initialization.
   @details   Initialization of all user defined actions as well as mandatory particle generator. Based on G4 examples/extended/parametrisations/Par01/include/Par01ActionInitialization.hh.
   @author    Anna Zaborowska
 */


class FCCActionInitialization : public G4VUserActionInitialization
{
  public:
    /**
      A default constructor. Sets the output file name fFileName (passed to FCCRunAction) to DefaultOutput.root as well as flag fSmear (passed to FCCEventAction) to true indicating that smearing will be performed.
    */
    FCCActionInitialization();
    /**
      A constructor.  Sets the flag fSmear (passed to FCCEventAction) to true indicating that smearing will be performed.
         @param aOutName The output file name passed to FCCRunAction.
    */
   FCCActionInitialization(const G4String aOutName);
   virtual ~FCCActionInitialization();

    virtual void BuildForMaster() const;
    /**
      A method where all the user actions are created. One of them, FCCPrimaryGeneratorAction is a mandatory class.
    */
    virtual void Build() const;
private:
    /**
      An output file name. Passed in Build() to the FCCRunAction.
    */
   G4String fFileName;
};

#endif

