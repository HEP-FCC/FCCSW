#ifndef SIMG4COMMON_PARTICLEINFORMATION_H
#define SIMG4COMMON_PARTICLEINFORMATION_H

// albers
#include "datamodel/MCParticleHandle.h"
#include "datamodel/ParticleHandle.h"

// Geant4
#include "G4VUserPrimaryParticleInformation.hh"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

/** @class FastSimParticleInformation SimG4Common/SimG4Common/FastSimParticleInformation.h FastSimParticleInformation.h
 *
 *  Describes the information that can be assosiated with a G4PrimaryParticle class object.
 *  It contains handles to the EDM Monte Carlo object.
 *  It is used for the fast simulation in Geant to associate MC particle with a 'reconstructed' particle.
 *  MCParticle information is filled when EDM event is translated to G4Event.
 *  Momentum, status and vertex info is filled at the end of Geant's track processing
 *  (SaveParticlesTrackingAction::PostUserTrackingAction).
 *
 *  @author Anna Zaborowska
 */

namespace sim {
class ParticleInformation: public G4VUserPrimaryParticleInformation {
public:
  /** A constructor.
   *  @param[in] aMCpart handle to the EDM MCParticle.
   */
  ParticleInformation(const MCParticleHandle& aMCpartt);
  /// A destructor
  virtual ~ParticleInformation();
  /// A printing method
  virtual void Print() const final;
  /** Getter of the handle to the MCParticle.
   *  @returns handle to the EDM MCParticle.
   */
  const MCParticleHandle& mcParticleHandle() const;
  /** Setter of the end-of-tracking momentum (used for fast simulation).
   *  @param[in] aMom Particle momentum.
   */
  void setEndMomentum(const CLHEP::Hep3Vector& aMom);
  /** Getter of the vertex posiiton taken at the end-of-tracking (used for fast simulation).
   *  @returns Vertex position.
   */
  const CLHEP::Hep3Vector& endMomentum() const;
  /** Setter of the vertex positon (used for fast simulation).
   *  @param[in] aPos Vertex position.
   */
  void setVertexPosition(const CLHEP::Hep3Vector& aPos);
  /** Getter of the vertex positon (used for fast simulation).
   *  @returns Vertex position.
   */
  const CLHEP::Hep3Vector& vertexPosition() const;
  /** Setter of the end-of-tracking particle status (used for fast simulation).
   *  @param[in] aStat Particle status.
   */
  void setEndStatus(int aStat);
  /** Getter of the end-of-tracking particle status (used for fast simulation).
   *  @returns Particle status.
   */
  const int endStatus() const;
private:
  /// Handle to the MC particle
  const MCParticleHandle m_mcParticleHandle;
  /// Particle momentum at the end of tracking (filled for fast-sim)
  CLHEP::Hep3Vector m_endMomentum;
  /// Particle vertex position saved at the end of tracking (filled for fast-sim)
  CLHEP::Hep3Vector m_vertexPosition;
  /// Particle status at the end of tracking (filled for fast-sim)
  int m_endStatus;
};
}

#endif /* SIMG4COMMON_PARTICLEINFORMATION_H */
