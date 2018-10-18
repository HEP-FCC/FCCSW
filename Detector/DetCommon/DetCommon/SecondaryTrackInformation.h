#ifndef SIMG4FULL_SECONDARYTRACKINFO_H
#define SIMG4FULL_SECONDARYTRACKINFO_H

#include "G4String.hh"
#include "G4VUserTrackInformation.hh"

/** @class SecondaryTrackInformation
 *
 * Class to flag secondaries which have only been created within the detector material and do not leave it.
 *
 *  @author Julia Hrdinka
 */

namespace det {
class SecondaryTrackInformation : public G4VUserTrackInformation {
public:
  SecondaryTrackInformation(std::string trackInfoName = "SecondaryTrackInformation")
      : G4VUserTrackInformation(G4String(trackInfoName)){};
  virtual ~SecondaryTrackInformation() = default;
};
}

#endif /* SIMG4FULL_SECONDARYTRACKINFO_H */
