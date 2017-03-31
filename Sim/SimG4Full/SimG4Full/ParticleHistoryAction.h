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
 *  @author J. Lingemann (skeleton and simple implementation)
 *  @author N. Tehrani (actual selection logic)
 */

namespace sim {
class ParticleHistoryAction : public G4UserTrackingAction {
public:
  ParticleHistoryAction(ParticleHistoryManager& aHistoryMgr);
  virtual ~ParticleHistoryAction() = default;

  void PreUserTrackingAction(const G4Track*);
  void PostUserTrackingAction(const G4Track*);

private:
  /// Flag whether or not to store particle history
  sim::ParticleHistoryManager& m_historyMgr;
};
}

#endif /* SIMG4FULL_PARTICLEHISTORYACTION_H */
