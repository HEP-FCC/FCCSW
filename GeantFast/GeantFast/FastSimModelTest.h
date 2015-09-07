#ifndef FAST_SIM_MODEL_TEST_H
#define FAST_SIM_MODEL_TEST_H

// Geant
#include "G4VFastSimulationModel.hh"
#include "G4Step.hh"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GeantFast/ISmearingTool.h"

/**
  @brief     Shortcut to the ordinary tracking for electromagnetic calorimeters.
   @details   Fast simulation model describes what should be done instead of a normal tracking. Instead of the ordinary tracking, a particle deposits its energy at the entrance to the detector and its value is smeared (by FCCSmearer::SmearMomentum()). Based on G4 examples/extended/parametrisations/Par01/include/Par01EMShowerModel.hh.
   @author    Anna Zaborowska
*/

class FastSimModelTest : public G4VFastSimulationModel
{
public:
   /**
      A constructor.
      @param aModelName A name of the fast simulation model.
      @param aEnvelope A region where the model can take over the ordinary tracking.
    */
   FastSimModelTest (G4String aModelName, G4Region* aEnvelope, std::string);
   /**
      A constructor.
      @param aModelName A name of the fast simulation model.
    */
   FastSimModelTest (G4String aModelName);
   ~FastSimModelTest ();
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
   /// Message Service
    ServiceHandle<IMessageSvc> m_msgSvc;
    // Message Stream
    MsgStream log;
   /// Tool Service
    ServiceHandle<IToolSvc> m_toolSvc;
   /// Pointer to a smearing tool
    ISmearingTool* m_smearTool;
};
#endif
