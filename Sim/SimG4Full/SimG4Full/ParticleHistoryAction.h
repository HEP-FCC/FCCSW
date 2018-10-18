#ifndef SIMG4FULL_PARTICLEHISTORYACTION_H
#define SIMG4FULL_PARTICLEHISTORYACTION_H

#include "G4UserTrackingAction.hh"
#include <G4String.hh>

/** @class ParticleHistoryAction SimG4Full/SimG4Full/ParticleHistoryAction.h ParticleHistoryAction.h
 *
 *  User tracking action that stores particle history
 *
 *  Heavily inspired by the HepMC MCTruth example in Geant:
 *  http://geant4.web.cern.ch/geant4/geant4_public/source/geant4/examples/extended/eventgenerator/HepMC/MCTruth/README
 *
 *  @author J. Lingemann
 */

namespace sim {
class ParticleHistoryAction : public G4UserTrackingAction {
public:
  /** constructor
  * @param[in] energyCut energy threshold above which particles are saved
  * @param[in] excludeTracks possibility to hand over name  of track information to exclude certain particles
  */
  ParticleHistoryAction(double energyCut, const G4String& excludeTrack = "");
  /// destructor
  virtual ~ParticleHistoryAction() = default;

  /// empty action - particles are only saved at the end of track
  void PreUserTrackingAction(const G4Track* aTrack);
  /// particles are saved here, after geant4 is done simulating the track
  void PostUserTrackingAction(const G4Track* aTrack);

  /** Simple filter for particles to be saved, based on their energy.
  * @param[in] aTrack track of the particle to be saved
  * @param[in] aEnergyCut energy threshold above which particles are saved
  * @param[in] excludeTracks possibility to set the name  of track information to exclude certain particles
  */
  bool selectParticle(const G4Track& aTrack, double aEnergyCut, const G4String& excludeTracks = "");

private:
  /// energy threshold for secondaries to be saved
  double m_energyCut;
  /// name  of track information to exclude certain particles
  G4String m_excludeTracks;
};
}

#endif /* SIMG4FULL_PARTICLEHISTORYACTION_H */
