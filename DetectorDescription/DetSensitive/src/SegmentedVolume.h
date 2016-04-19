#ifndef DETSENSITIVE_SEGMENTEDVOLUME_H
#define DETSENSITIVE_SEGMENTEDVOLUME_H

// DD4hep
#include "DDSegmentation/Segmentation.h"
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

// Geant
class G4Step;
class G4TouchableHistory;

/** SegmentedVolume DetectorDescription/DetSensitive/src/SegmentedVolume.h SegmentedVolume.h
 *
 *  Basic interface for the sensitive detector.
 *
 *  @author    Anna Zaborowska
 */

namespace det {
class SegmentedVolume
{
  public:
  /** Constructor.
   *  @param aSeg Segmentation of the detector (used to retrieve the cell ID)
   */
  SegmentedVolume(DD4hep::Geometry::Segmentation aSeg);
  /// Destructor
  ~SegmentedVolume();
  /** Retrieves the cellID based on the position and the detector segmentation.
   *  @param aStep Step in which particle deposited the energy.
   */
  uint64_t getCellID(G4Step* aStep);

private:
  /// Segmentation of the detector used to retrieve the cell Ids
  DD4hep::Geometry::Segmentation m_seg;
};
}

#endif /* DETSENSITIVE_SEGMENTEDVOLUME_H */
