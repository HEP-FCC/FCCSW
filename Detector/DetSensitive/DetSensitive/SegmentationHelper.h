#ifndef DETSENSITIVE_SEGMENTATIONHELPER_H
#define DETSENSITIVE_SEGMENTATIONHELPER_H

// DD4hep
#include "DD4hep/Segmentations.h"

// Geant
#include "G4Step.hh"

namespace det {
namespace segmentation {
  /** Retrieves the cellID based on the position of the step and the detector segmentation.
   *  @param aSeg Handle to the segmentation of the volume.
   *  @param aStep Step in which particle deposited the energy.
   *  @param aPreStepPoint Flag indicating if the position of the deposit is the beginning of the step (default)
   *         or the middle of the step.
   */

  uint64_t cellID(const DD4hep::Geometry::Segmentation& aSeg, const G4Step& aStep, bool aPreStepPoint = true);
}
}

#endif /* DETSENSITIVE_SEGMENTATIONHELPER_H */
