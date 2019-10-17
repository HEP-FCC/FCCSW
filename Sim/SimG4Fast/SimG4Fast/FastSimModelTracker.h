#ifndef SIMG4FAST_FASTSIMMODELTRACKER_H
#define SIMG4FAST_FASTSIMMODELTRACKER_H

// FCCSW
class ISimG4ParticleSmearTool;

// Geant
#include "G4VFastSimulationModel.hh"

// Gaudi
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

class IToolSvc;

/** FastSimModelTracker SimG4Fast/SimG4Fast/FastSimModelTracker.h FastSimModelTracker.h
 *
 *  Fast simulation model describes what should be done instead of a normal tracking
 *  for the certain particle in the certain volumes.
 *  Particles need to have the parametrisation process attached (FastSimPhysics)
 *  and fulfill conditions of a) FastSimModelTracker::IsApplicable() and b) FastSimModelTracker::ModelTrigger().
 *  a) all charged particles are parametrised.
 *  b) if minimum and/or maximum momentum is defined, particle within that range are parametrised. Maximum eta can also
 * be set.
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
   *  @param aSmearToolType Type of the implementation of ISimG4ParticleSmearTool (conrete class name) to be searched
   * for by ToolSvc.
   *  @param aSmearToolName Name of the implementation of ISimG4ParticleSmearTool to be searched for by ToolSvc.
   *  @param aMinMomentum Minimum particle momentum that triggers that model
   *  @param aMinMomentum Maximum particle momentum that triggers that model
   *  @param aMinMomentum Maximum pseudorapidity that triggers that model
   */
  explicit FastSimModelTracker(const std::string& aModelName, G4Region* aEnvelope,
                               ToolHandle<ISimG4ParticleSmearTool>& aSmearTool, double aMinMomentum,
                               double aMaxMomentum, double aMaxEta);
  /** Constructor.
   *  @param aModelName Name of the fast simulation model.
   */
  explicit FastSimModelTracker(const std::string& aModelName, ToolHandle<ISimG4ParticleSmearTool>& aSmearTool);
  virtual ~FastSimModelTracker();
  /** Check if this model should be applied to this particle type.
   *  @param aParticle Particle definition (type).
   */
  virtual G4bool IsApplicable(const G4ParticleDefinition& aParticle) final;
  /** Check if the model should be applied taking into account the kinematics of a track.
   *  @param aFastTrack Track.
   */
  virtual G4bool ModelTrigger(const G4FastTrack& aFastTrack) final;
  /** Apply the parametrisation.
   *  Move the particle to the exit from the volume along the computed trajectory.
   *  Smear the momentum with the smearing tool m_smearTool.
   *  @param aFastTrack Track.
   *  @param aFastStep Step.
   */
  virtual void DoIt(const G4FastTrack& aFastTrack, G4FastStep& aFastStep) final;

private:
  /// Message Service
  ServiceHandle<IMessageSvc> m_msgSvc;
  /// Message Stream
  MsgStream m_log;
  /// Pointer to a smearing tool
  ToolHandle<ISimG4ParticleSmearTool>& m_smearTool;
  /// minimum P that triggers model
  double m_minTriggerMomentum;
  /// maximum P that triggers model
  double m_maxTriggerMomentum;
  /// maximum eta that triggers model
  double m_maxTriggerEta;
};
}

#endif /* SIMG4FAST_FASTSIMMODELTRACKER_H */
