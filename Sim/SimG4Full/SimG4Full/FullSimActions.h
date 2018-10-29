#ifndef SIMG4FULL_FULLSIMACTIONS_H
#define SIMG4FULL_FULLSIMACTIONS_H

#include "G4VUserActionInitialization.hh"
#include <G4String.hh>

/** @class FullSimActions SimG4Full/SimG4Full/FullSimActions.h FullSimActions.h
 *
 *  User action initialization for full simulation.
 *
 *  @author A. Zaborowska
 *  @author J. Lingemann (adding particle history)
 */

namespace sim {
class FullSimActions : public G4VUserActionInitialization {
public:
  /**constructor
  * @param[in] enableHistory flag if history should be enabled
  * @param[in] aEnergyCut energy threshold above which particles are saved
  * @param[in] selectTaggedOnly possibility select only tagged (by setting the G4VUserTrackInformation of the G4Track)
  * tracks, if set to true
  */
  FullSimActions(bool enableHistory, double aEnergyCut, bool selectTaggedOnly = false);
  /// destructor
  virtual ~FullSimActions();
  /// Create all user actions.
  virtual void Build() const final;

private:
  /// Flag whether or not to store particle history
  bool m_enableHistory;
  /// energy threshold for secondaries to be saved
  double m_energyCut;
  /// name  of track information to exclude certain particles
  bool m_selectTaggedOnly;
};
}

#endif /* SIMG4FULL_FULLSIMACTIONS_H */
