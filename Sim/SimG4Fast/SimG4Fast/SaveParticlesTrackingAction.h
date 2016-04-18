#ifndef SIMG4FAST_SAVEPARTICLESTRACKINGACTION_H
#define SIMG4FAST_SAVEPARTICLESTRACKINGACTION_H

// Geant
#include "G4UserTrackingAction.hh"

/** @class SaveParticlesTrackingAction SimG4Fast/SimG4Fast/SaveParticlesTrackingAction.h SaveParticlesTrackingAction.h
 *
 *  Tracking action (invoked before/after Geant track processing).
 *  At the end of the track processing fills the information about the 'reconstructed' particle in ParticleInformation
 *  object that is associated with every G4PrimaryParticle.
 *
 *  @author Anna Zaborowska
*/

namespace sim {
class SaveParticlesTrackingAction : public G4UserTrackingAction {
public:
  SaveParticlesTrackingAction();
  virtual ~SaveParticlesTrackingAction();
  /* Defines the actions at the end of processing the track.
   * It save the information about momentum, status and vertex in the
   * information associated with every G4PrimaryParticle (ParticleInformation)
   */
  virtual void  PostUserTrackingAction(const G4Track*) final;

};
}

#endif /* SIMG4FAST_SAVEPARTICLESTRACKINGACTION_H */


