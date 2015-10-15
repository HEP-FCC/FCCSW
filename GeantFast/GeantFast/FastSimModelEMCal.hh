#ifndef EMCAL_FAST_SIM_MODEL_H
#define EMCAL_FAST_SIM_MODEL_H

#include "G4VFastSimulationModel.hh"
#include "DetectorParametrisation.hh"
#include "G4Step.hh"

/**
  @brief     Shortcut to the ordinary tracking for electromagnetic calorimeters.
   @details   Fast simulation model describes what should be done instead of a normal tracking. Instead of the ordinary tracking, a particle deposits its energy at the entrance to the detector and its value is smeared (by Smearer::SmearMomentum()). Based on G4 examples/extended/parametrisations/Par01/include/Par01EMShowerModel.hh.
   @author    Anna Zaborowska
*/

class FastSimModelEMCal : public G4VFastSimulationModel
{
public:
   /**
      A constructor.
      @param aModelName A name of the fast simulation model.
      @param aEnvelope A region where the model can take over the ordinary tracking.
      @param aParamType A parametrisation type.
    */
   FastSimModelEMCal (G4String aModelName, G4Region* aEnvelope, DetectorParametrisation::Parametrisation aParamType);
   /**
      A constructor.
      @param aModelName A name of the fast simulation model.
      @param aEnvelope A region where the model can take over the ordinary tracking.
    */
   FastSimModelEMCal (G4String aModelName, G4Region* aEnvelope);
   /**
      A constructor.
      @param aModelName A name of the fast simulation model.
    */
   FastSimModelEMCal (G4String aModelName);
   ~FastSimModelEMCal ();
   /**
      Checks if this model should be applied to this particle type.
      @param aParticle A particle definition (type).
    */
   virtual G4bool IsApplicable(const G4ParticleDefinition& aParticle);
   /**
      Checks if the model should be applied taking into account the kinematics of a track.
      @param aFastTrack A track.
    */
   virtual G4bool ModelTrigger(const G4FastTrack & aFastTrack);
   /**
      Smears the energy deposit and saves it, together with the position of the deposit, the detector resolution and efficiency to the PrimaryParticleInformation.
      @param aFastTrack A track.
      @param aFastStep A step.
    */
   virtual void DoIt(const G4FastTrack& aFastTrack, G4FastStep& aFastStep);


private:
   /**
      A pointer to DetectorParametrisation used to get the efficiency and resolution of the detector for a given particle and parametrisation type (fParam).
    */
   DetectorParametrisation* fCalculateParametrisation;
   /**
      A parametrisation type.
    */
   DetectorParametrisation::Parametrisation fParametrisation;
};
#endif




