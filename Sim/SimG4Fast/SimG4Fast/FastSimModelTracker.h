#ifndef SIMG4FAST_FASTSIMMODELTRACKER_H
#define SIMG4FAST_FASTSIMMODELTRACKER_H

//FCCSW
class ISimG4ParticleSmearTool;

// Geant
#include "G4VFastSimulationModel.hh"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
class IToolSvc;


/** FastSimModelTracker SimG4Fast/SimG4Fast/FastSimModelTracker.h FastSimModelTracker.h
 *
 *  Fast simulation model describes what should be done instead of a normal tracking
 *  for the certain particle in the certain volumes.
 *  Particles need to have the parametrisation process attached (FastSimPhysics)
 *  and fulfill conditions of FastSimModelTracker::IsApplicable() and FastSimModelTracker::ModelTrigger().
 *  Volumes are represented by G4Region given to the constructor of FastSimModelTracker.
 *  For those volumes, if the model is triggered, instead of the ordinary tracking,
 *  a particle is moved to the exit of the tracker (as it would be transported with initial momentum and no
 *  physics processes on the way) and the particle momentum is smeared according to the smearing tool
 *  defined in the job options file.
 *
 *  @author    Anna Zaborowska
 */

namespace sim {
class FastSimModelTracker : public G4VFastSimulationModel {
  public:
  /** Constructor.
   *  @param aModelName Name of the fast simulation model.
   *  @param aEnvelope Region where the model can take over the ordinary tracking.
   *  @param aSmearToolName Name of the implementation of ISimG4ParticleSmearTool to be searched for by ToolSvc.
   */
  explicit FastSimModelTracker (const std::string& aModelName, G4Region* aEnvelope, const std::string& aSmearToolName);
  /** Constructor.
   *  @param aModelName Name of the fast simulation model.
   */
  explicit FastSimModelTracker (const std::string& aModelName);
  virtual ~FastSimModelTracker ();
  /** Check if this model should be applied to this particle type.
   *  @param aParticle Particle definition (type).
   */
  virtual G4bool IsApplicable(const G4ParticleDefinition& aParticle) final;
  /** Check if the model should be applied taking into account the kinematics of a track.
   *  @param aFastTrack Track.
   */
  virtual G4bool ModelTrigger(const G4FastTrack & aFastTrack) final;
  /** Apply the parametrisation.
   *  Move the particle to the exit from the volume along the computed trajectory.
   *  Smear the momentum with the smearing tool m_smearTool.
   *  @param aFastTrack Track.
   *  @param aFastStep Step.
   */
  virtual void DoIt(const G4FastTrack& aFastTrack, G4FastStep& aFastStep) final;

  private:
  /// Tool Service
  ServiceHandle<IToolSvc> m_toolSvc;
  /// Pointer to a smearing tool
  ISimG4ParticleSmearTool* m_smearTool;
};
}

#endif /* SIMG4FAST_FASTSIMMODELTRACKER_H */
