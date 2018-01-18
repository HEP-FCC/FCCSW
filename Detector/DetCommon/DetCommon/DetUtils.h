#ifndef DETCOMMON_DETUTILS_H
#define DETCOMMON_DETUTILS_H

// FCCSW
#include "DetSegmentation/GridPhiEta.h"

// DD4hep
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Segmentations.h"
#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/CartesianGridXY.h"
#include "DDSegmentation/CartesianGridXYZ.h"
#include "DDSegmentation/PolarGridRPhi.h"

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
DD4hep::XML::Component getNodeByStrAttr(const DD4hep::XML::Handle_t& mother, const std::string& nodeName,
                                        const std::string& attrName, const std::string& attrValue);

/// try to get attribute with double value, return defaultValue if attribute not found
double getAttrValueWithFallback(const DD4hep::XML::Component& node, const std::string& attrName,
                                const double& defaultValue);

/** Retrieves the cellID based on the position of the step and the detector segmentation.
 *  @param aSeg Handle to the segmentation of the volume.
 *  @param aStep Step in which particle deposited the energy.
 *  @param aPreStepPoint Flag indicating if the position of the deposit is the beginning of the step (default)
 *         or the middle of the step.
 */

uint64_t cellID(const DD4hep::Geometry::Segmentation& aSeg, const G4Step& aStep, bool aPreStepPoint = true);

/**  Get neighbours in many dimensions.
 *   @param[in] aDecoder Handle to the bitfield decoder.
 *   @param[in] aFieldNames Names of the fields for which neighbours are found.
 *   @param[in] aFieldExtremes Minimal and maximal values for the fields.
 *   @param[in] aCellId ID of cell.
 *   @param[in] aSteps defines the bit range for which neighbours are looked for 
 *   return Vector of neighbours.
 */
std::vector<uint64_t> neighbours(DD4hep::DDSegmentation::BitField64& aDecoder,
                                 const std::vector<std::string>& aFieldNames,
                                 const std::vector<std::pair<int, int>>& aFieldExtremes,
                                 uint64_t aCellId,
				 int aSteps
				 );

/** Get minimal and maximal values that can be decoded in the fields of the bitfield.
 *   @param[in] aDecoder Handle to the bitfield decoder.
 *   @param[in] aFieldNames Names of the fields for which extremes are found.
 *   return Vector of pairs (min,max)
 */
std::vector<std::pair<int, int>> bitfieldExtremes(DD4hep::DDSegmentation::BitField64& aDecoder,
                                                  const std::vector<std::string>& aFieldNames);

/** Get the half widths of the box envelope (TGeoBBox).
 *   @param[in] aVolumeId The volume ID.
 *   return Half-widths of the volume (x,y,z).
 */
CLHEP::Hep3Vector envelopeDimensions(uint64_t aVolumeId);

/** Get the dimensions of a tube (TGeoTube).
 *   @param[in] aVolumeId The volume ID.
 *   return Dimensions of the tube (rmin, rmax, z(half-length)).
 */
CLHEP::Hep3Vector tubeDimensions(uint64_t aVolumeId);

/** Get the number of cells for the volume and a given Cartesian XY segmentation.
 *   For an example see: Test/TestReconstruction/tests/options/testcellcountingXYZ.py.
 *   @warning No offset in segmentation is currently taken into account.
 *   @param[in] aVolumeId The volume for which the cells are counted.
 *   @param[in] aSeg Handle to the segmentation of the volume.
 *   return Array of the number of cells in (X, Y).
 */
std::array<uint, 2> numberOfCells(uint64_t aVolumeId, const DD4hep::DDSegmentation::CartesianGridXY& aSeg);

/** Get the number of cells for the volume and a given Cartesian XYZ segmentation.
 *   For an example see: Test/TestReconstruction/tests/options/testcellcountingXYZ.py.
 *   @warning No offset in segmentation is currently taken into account.
 *   @param[in] aVolumeId The volume for which the cells are counted.
 *   @param[in] aSeg Handle to the segmentation of the volume.
 *   return Array of the number of cells in (X, Y, Z).
 */
std::array<uint, 3> numberOfCells(uint64_t aVolumeId, const DD4hep::DDSegmentation::CartesianGridXYZ& aSeg);

/** Get the number of cells for the volume and a given Phi-Eta segmentation.
 *   It is assumed that the volume has a cylindrical shape (and full azimuthal coverage)
 *   and that it is centred at (0,0,0).
 *   For an example see: Test/TestReconstruction/tests/options/testcellcountingPhiEta.py.
 *   @warning No offset in segmentation is currently taken into account.
 *   @param[in] aVolumeId The volume for which the cells are counted.
 *   @param[in] aSeg Handle to the segmentation of the volume.
 *   return Array of the number of cells in (phi, eta).
 */
std::array<uint, 2> numberOfCells(uint64_t aVolumeId, const DD4hep::DDSegmentation::GridPhiEta& aSeg);

/** Get the number of cells for the volume and a given R-phi segmentation.
 *   It is assumed that the volume has a cylindrical shape - TGeoTube (and full azimuthal coverage)
 *   and that it is centred at (0,0,0).
 *   For an example see: Test/TestReconstruction/tests/options/testcellcountingRPhi.py.
 *   @warning No offset in segmentation is currently taken into account.
 *   @param[in] aVolumeId The volume for which the cells are counted.
 *   @param[in] aSeg Handle to the segmentation of the volume.
 *   return Array of the number of cells in (r, phi).
 */
std::array<uint, 2> numberOfCells(uint64_t aVolumeId, const DD4hep::DDSegmentation::PolarGridRPhi& aSeg);

/** Get the number of the volumes containing a given name.
 *   For an example see: Test/TestReconstruction/tests/options/testcellcountingXYZ.py.
 *   @param[in] aHighestVolume The top volume in the geometry.
 *   @param[in] aMatchName Name (or its part) of the volume.
 *   return Number of the volumes.
 */
unsigned int countPlacedVolumes(TGeoVolume* aHighestVolume, const std::string& aMatchName);
}
}
#endif /* DETCOMMON_DETUTILS_H */
