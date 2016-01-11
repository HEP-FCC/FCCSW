#ifndef SIMG4FAST_SAVEPARTICLESTRACKINGACTION_H
#define SIMG4FAST_SAVEPARTICLESTRACKINGACTION_H

// Geant
#include "G4UserTrackingAction.hh"

/** @class SaveParticlesTrackingAction SimG4Fast/SimG4Fast/SaveParticlesTrackingAction.h SaveParticlesTrackingAction.h
 *
 *  Tracking action (invoked before/after Geant track processing).
 *  Saves, at the end of the track processing, the 'reconstructed' particle.
 *
 *  @author Anna Zaborowska
*/

namespace sim {
class SaveParticlesTrackingAction : public G4UserTrackingAction {
public:
  SaveParticlesTrackingAction();
  virtual ~SaveParticlesTrackingAction();
  /* Defines the actions at the end of processing the track.
   * It takes the EDM ParticleHandle from the information associated with every G4PrimaryParticle
   * (ParticleInformation) and fills it with 'reconstructe particle information (momentum, vertex, ...)
   */
  virtual void  PostUserTrackingAction(const G4Track*) final;

};
}

#endif /* SIMG4FAST_SAVEPARTICLESTRACKINGACTION_H */


