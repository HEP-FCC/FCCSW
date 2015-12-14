#ifndef GEANT_FAST_MODEL_TRACKER_H
#define GEANT_FAST_MODEL_TRACKER_H

// Geant
#include "G4VFastSimulationModel.hh"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
class IToolSvc;

//FCCSW
class ISmearingTool;

/**
   @brief     Shortcut to the ordinary tracking.
   @details   Fast simulation model describes what should be done instead of a normal tracking for certain particle in certain volumes. Instead of the ordinary tracking, a particle deposits its energy at the entrance to the detector and its value is smeared.
   @author    Anna Zaborowska
*/

class FastSimModelTracker : public G4VFastSimulationModel {
public:
   /**
      A constructor.
      @param aModelName A name of the fast simulation model.
      @param aEnvelope A region where the model can take over the ordinary tracking.
    */
   FastSimModelTracker (const std::string aModelName, G4Region* aEnvelope, std::string);
   /**
      A constructor.
      @param aModelName A name of the fast simulation model.
    */
   FastSimModelTracker (const std::string aModelName);
   ~FastSimModelTracker ();
   /**
      Checks if this model should be applied to this particle type.
      @param aParticle A particle definition (type).
    */
   virtual G4bool IsApplicable(const G4ParticleDefinition& aParticle) final;
   /**
      Checks if the model should be applied taking into account the kinematics of a track.
      @param aFastTrack A track.
    */
   virtual G4bool ModelTrigger(const G4FastTrack & aFastTrack) final;
   /**
      Smears the energy deposit and saves it, together with the position of the deposit, the detector resolution and efficiency to the PrimaryParticleInformation.
      @param aFastTrack A track.
      @param aFastStep A step.
    */
   virtual void DoIt(const G4FastTrack& aFastTrack, G4FastStep& aFastStep) final;

private:
   /// Tool Service
   ServiceHandle<IToolSvc> m_toolSvc;
   /// Pointer to a smearing tool
   ISmearingTool* m_smearTool;
};
#endif
