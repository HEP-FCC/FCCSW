#ifndef DETCOMMON_DETUTILS_H
#define DETCOMMON_DETUTILS_H

// DD4hep
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Segmentations.h"
#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/CartesianGridXYZ.h"

// Geant
#include "G4Step.hh"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

#include "TGeoManager.h"


/** Given a XML element with several daughters with the same name, e.g.
 <detector> <layer name="1" /> <layer name="2"> </detector>
 this method returns the first daughter of type nodeName whose attribute has a given value
 e.g. returns <layer name="2"/> when called with (detector, "layer", "name", "1") */
namespace det {
namespace utils {
DD4hep::XML::Component getNodeByStrAttr(const DD4hep::XML::Handle_t& mother, const std::string& nodeName, const std::string& attrName,
                                        const std::string& attrValue);

/// try to get attribute with double value, return defaultValue if attribute not found
double getAttrValueWithFallback(const DD4hep::XML::Component& node, const std::string& attrName, const double& defaultValue);

/** Retrieves the cellID based on the position of the step and the detector segmentation.
 *  @param aSeg Handle to the segmentation of the volume.
 *  @param aStep Step in which particle deposited the energy.
 *  @param aPreStepPoint Flag indicating if the position of the deposit is the beginning of the step (default)
 *         or the middle of the step.
 */

uint64_t cellID(const DD4hep::Geometry::Segmentation& aSeg, const G4Step& aStep, bool aPreStepPoint = true);

/**  Get neighbours in many dimensions.
 *   @param[in] aDecoder Handle to the bitfield decoder.
 *   @param[in] aDimensionNames Names of the identifiers for which neighbours are found.
 *   @param[in] aCellId ID of cell.
 *   return Vector of neighbours.
 */
std::vector<uint64_t> neighbours(DD4hep::DDSegmentation::BitField64& aDecoder,
  const std::vector<std::string>& aDimensionNames,
  uint64_t aCellId);

/** Get minimal and maximal values that can be decoded in the fields of the bitfield.
 *   @param[in] aDecoder Handle to the bitfield decoder.
 *   return Vector of pairs (min,max)
 */
std::vector<std::pair<int,int>> bitfieldExtremes(DD4hep::DDSegmentation::BitField64& aDecoder);

CLHEP::Hep3Vector numberOfCellsInCartesian(uint64_t aVolumeID, const DD4hep::DDSegmentation::CartesianGridXYZ& aSeg);

unsigned int countPlacedVolumes(TGeoVolume* highestVolume, std::string matchName);
}
}
#endif /* DETCOMMON_DETUTILS_H */
