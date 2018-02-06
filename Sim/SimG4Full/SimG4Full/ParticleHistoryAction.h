#ifndef SIMG4FULL_PARTICLEHISTORYACTION_H
#define SIMG4FULL_PARTICLEHISTORYACTION_H

#include "G4UserTrackingAction.hh"

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
  ParticleHistoryAction(double energyCut);
  virtual ~ParticleHistoryAction() = default;

  void PreUserTrackingAction(const G4Track* aTrack);
  void PostUserTrackingAction(const G4Track* aTrack);

  bool selectSecondary(const G4Track& aTrack, double energyCut);
private:
  double m_energyCut;
};
}

#endif /* SIMG4FULL_PARTICLEHISTORYACTION_H */
