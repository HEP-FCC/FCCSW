#ifndef FCC_EVENT_INFORMATION_H
#define FCC_EVENT_INFORMATION_H

#include "G4VUserEventInformation.hh"
#include "globals.hh"

/**
  @brief     Event information
   @details   Describes the information that can be assosiated with a G4Event class object.
   @author    Anna Zaborowska
*/
class FCCEventInformation: public G4VUserEventInformation
{
public:
   /**
      A default constructor. Sets flag fDoSmearing to true and fSavePerigee to false.
   */
   FCCEventInformation();
   /**
      A constructor. Sets flag fSavePerigee to false.
      @param aSmear The flag indicating if smearing should be done.
   */
   FCCEventInformation(G4bool aSmear);
   virtual ~FCCEventInformation();
   /**
      Prints event information.
   */
   virtual void Print() const;
   /**
      Sets the flag indicating if smearing should be done.
      @param aSmear A boolean flag.
   */
   void SetDoSmearing(G4bool aSmear);
   /**
      Gets the flag indicating if smearing should be done.
   */
   G4bool GetDoSmearing();

private:
   /**
      A flag indicating if smearing should be performed. It is read by implementations of G4VFastSimulationModel.
   */
   G4bool fDoSmearing;
};

#endif

