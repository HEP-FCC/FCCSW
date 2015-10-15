#ifndef OUTPUT_H
#define OUTPUT_H

#include "G4ThreeVector.hh"
#include "globals.hh"

/**
  @brief     Handling saving to the file.
   @details   A singleton class manages creation, writing to and closing thr root output file.
   @author    Anna Zaborowska
 */

class Output
{
public:
/**
  Indicates to which ntuple save the information.
 */
   enum SaveType {eNoSave, eSaveMC, eSaveTracker, eSaveEMCal, eSaveHCal};

/**
   Allows the access to the Output class member without creating the class object.
  @return A pointer to the Output class.
 */
   static Output* Instance();
/**
   Sets the file name of the output root file.
   @param name The name of the file.
 */
   void SetFileName(G4String name);
/**
   Gets the file name of the output root file.
   @return The name of the file.
 */
   G4String GetFileName();
/**
   Sets fFileNameWithRunNo that indicates whether to add the run number to the file name.
   @param app If add the run number.
 */
   void AppendName(G4bool app);
/**
   Calls the G4AnalysisManager::Instance(). It sets the file name of the output file based on fFileName and fFileNameWithRunNo and opens the file.
   @param runID A run number (to be added to file name if fFileNameWithRunNo == TRUE).
 */
   void StartAnalysis(G4int runID);
/**
   Calls the G4AnalysisManager::Instance(). It writes to the output file and close it.
 */
   void EndAnalysis();
/**
   Creates Ntuples used to store information about particle (its ID, PDG code, energy deposits, etc.). To be called for each event in EventAction.
 */
   void CreateNtuples();
/**
   Creates histograms to combine information from all the events in the run. To be called for each run in RunAction.
 */
   void CreateHistograms();
/**
   Saves the information about the particle (track).
   @param aWhatToSave enum indicating what kind of information to store (in which ntuple).
   @param aPartID A unique ID within event (taken Geant TrackID).
   @param aPDG A PDG code of a particle.
   @param aVector A vector to be stored (particle momentum in tracker or position of energy deposit in calorimeter).
   @param aResolution A resolution of the detector that was used.
   @param aEfficiency An efficiency of the detector that was used.
   @param aEnergy An energy deposit (for calorimeters only: Output::SaveType::eEMCal or Output::SaveType::eHCal).
 */
   void SaveTrack(SaveType aWhatToSave, G4int aPartID,  G4int aPDG,
                  G4ThreeVector aVector, G4double aResolution = 0, G4double aEfficiency = 1, G4double aEnergy = 0) ;
/**
   Fills the histogram.
   @param HNo Number of a histogram (decided by the order of creation in CreateHistograms(), the first one is 0) .
   @param value A value to be filled into the histogram.
 */
   void FillHistogram(G4int HNo, G4double value) const;
   ~Output();
protected:
/**
   A default, protected constructor (due to singleton patern).
 */
   Output();
private:
/**
   A pointer to the only Output class object.
 */
   static Output* fOutput;
/**
   A name of the output root file.
 */
   G4String fFileName;
/**
   If true, to the file a run number should be added. Default: false.
 */
   G4bool fFileNameWithRunNo;
/**
   A control value of particle ID to ensure that data saved to various ntuples matches the same particle. It is set when Monte Carlo information is saved and checked for all the detectors.
 */
   G4int fCurrentID;
};

#endif
