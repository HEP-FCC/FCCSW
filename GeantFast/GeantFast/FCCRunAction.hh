#ifndef FCC_RUN_ACTION_H
#define FCC_RUN_ACTION_H

#include "G4UserRunAction.hh"
#include "globals.hh"

class G4Run;

/**
  @brief     Run action (before/after run processing).
   @details   Defines the action at the beginning and at the end of each run. No content of G4Run can be changed. The class needs to be set in G4RunManager::SetUserAction().
   @author    Anna Zaborowska
 */

class FCCRunAction : public G4UserRunAction
{
  public:
   /**
      A default constructor.
      @param OutName The output root file name. It will store all the events within run.
    */
    FCCRunAction(const G4String OutName);
    virtual ~FCCRunAction();

   /**
     Defines the actions at the beginning of the run. It starts the analysis (create output root file) and create all the histograms defined in FCCOutput singleton class.
    */
   virtual void BeginOfRunAction(const G4Run*);
   /**
      Defines the actions at the end of the run. It ends the analysis (write and close output root file) via FCCOutput singleton class.
   */
   virtual void   EndOfRunAction(const G4Run*);
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
