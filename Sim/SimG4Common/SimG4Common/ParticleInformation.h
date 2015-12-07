#ifndef SIMG4COMMON_PARTICLEINFORMATION_H
#define SIMG4COMMON_PARTICLEINFORMATION_H

// albers
#include "datamodel/MCParticleHandle.h"
#include "datamodel/ParticleHandle.h"

// Geant4
#include "G4VUserPrimaryParticleInformation.hh"

/** @class sim::ParticleInformation SimG4Common/ParticleInformation.h ParticleInformation.h
 *
 *  Describes the information that can be assosiated with a G4PrimaryParticle class object.
 *  It contains handles to the EDM objects.
 *  It is used for the fast simulation in Geant to associate MC particle with a 'reconstructed' particle.
 *  MCParticle information is filled when EDM event is translated to G4Event.
 *  Particle is meant to be filled with information on particle momentum etc. at the end of Geant processing (done in SaveParticlesTrackingAction::PostUserTrackingAction()).
 *
 *  @author Anna Zaborowska
 */

namespace sim {
class ParticleInformation: public G4VUserPrimaryParticleInformation {
public:
  /** A constructor.
   *  @param[in] aMCpart handle to the EDM MCParticle.
   *  @param[in] aPart handle to the EDM Particle.
   */
  ParticleInformation(const MCParticleHandle& aMCpart, ParticleHandle& aPart);
  /// A destructor
  virtual ~ParticleInformation();
  /// A printing method
  virtual void Print() const;
  /** Getter of the handle to the MCParticle.
   *  @returns handle to the EDM MCParticle.
   */
  const MCParticleHandle& GetMCParticleHandle() const;
  /** Getter of the handle to the Particle.
   *  @returns handle to the EDM Particle.
   */
  ParticleHandle& GetParticleHandle();
private:
  /// Handle to the MC particle
  const MCParticleHandle m_mcpart;
  /// Handle to the particle associated with a MCParticle.
  ParticleHandle m_part;
};
}

#endif /* SIMG4COMMON_PARTICLEINFORMATION_H */
