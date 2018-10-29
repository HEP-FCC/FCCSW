#ifndef SIMG4FULL_SELECTTRACKINFO_H
#define SIMG4FULL_SELECTTRACKINFO_H

#include "G4String.hh"
#include "G4VUserTrackInformation.hh"

/** @class SelectTrackInformation
 *
 * Class to flag G4Tracks in the sensitive detector which should be written out in the particle history.
 *
 *  @author Julia Hrdinka
 */

namespace det {
class SelectTrackInformation : public G4VUserTrackInformation {
public:
  SelectTrackInformation(std::string trackInfoName = "SelectTrackInformation")
      : G4VUserTrackInformation(G4String(trackInfoName)){};
  virtual ~SelectTrackInformation() = default;
};
}

#endif /* SIMG4FULL_SELECTTRACKINFO_H */
