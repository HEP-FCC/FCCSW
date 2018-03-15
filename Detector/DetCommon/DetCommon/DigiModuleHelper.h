#ifndef DETCOMMON_DIGIMODULEHELPER_H
#define DETCOMMON_DIGIMODULEHELPER_H

#include "ACTS/Digitization/CartesianSegmentation.hpp"
#include "ACTS/Digitization/DigitizationModule.hpp"
#include "DD4hep/Detector.h"
#include "DD4hep/Objects.h"

/** Helper methods to create the Acts::DigitizationModule for cartesian xz-segmentation
 *
 * @todo These methods will be implemented into the next version of ACTS
 *
 *  @author Julia Hrdinka
 *  @date   2018-02
 */

namespace det {
namespace utils {
/// Global method to build an Acts::DigitizationModule with rectangular
/// segmentation.
/// @note This function should be used in order to create the input
/// needed for construction with
/// Acts::ActsExtension(std::shared_ptr<const DigitizationModule>)
/// @param halflengthX The half length in x of the detector module
/// @param halflengthZ The half length in z of the detector module
/// @param thickness The thickness of the detector module
/// @param segmentation the DD4hep segmentation
std::shared_ptr<const Acts::DigitizationModule> rectangleDigiModuleXZ(double halflengthX,
                                                                      double halflengthZ,
                                                                      double thickness,
                                                                      const dd4hep::Segmentation& segmentation);

/// Global method to build an Acts::DigitizationModule with rectangular
/// segmentation.
/// @note This function should be used in order to create the input
/// needed for construction with
/// Acts::ActsExtension(std::shared_ptr<const DigitizationModule>)
/// @param halflengthX The half length in x of the detector module
/// @param halflengthZ The half length in z of the detector module
/// @param thickness The thickness of the detector module
/// @param gridSizeX The grid size in x
/// @param gridSizeY The grid size in y
std::shared_ptr<const Acts::DigitizationModule>
rectangleDigiModuleXZ(double halflengthX, double halflengthZ, double thickness, double gridSizeX, double gridSizeZ);

/// Global method to build an Acts::DigitizationModule with trapezoidal
/// segmentation.
/// @note This function should be used in order to create the input
/// needed for construction with
/// Acts::ActsExtension(std::shared_ptr<const DigitizationModule>)
/// @param minHalflengthX The half length in x of the detector module on the
/// negative side of z
/// @param maxHalflengthX The half length in x of the detector module on the
/// positive side of z
/// @param halflengthZ The half length in z of the detector module
/// @param thickness The thickness of the detector module
/// @param segmentation the DD4hep segmentation
std::shared_ptr<const Acts::DigitizationModule> trapezoidalDigiModuleXZ(double minHalflengthX,
                                                                        double maxHalflengthX,
                                                                        double halflengthZ,
                                                                        double thickness,
                                                                        const dd4hep::Segmentation& segmentation);

/// Global method to build an Acts::DigitizationModule with trapezoidal
/// segmentation.
/// @note This function should be used in order to create the input
/// needed for construction with
/// Acts::ActsExtension(std::shared_ptr<const DigitizationModule>)
/// @param minHalflengthX The half length in x of the detector module on the
/// negative side of z
/// @param maxHalflengthX The half length in x of the detector module on the
/// positive side of z
/// @param halflengthZ The half length in z of the detector module
/// @param thickness The thickness of the detector module
/// @param gridSizeX The grid size in x
/// @param gridSizeY The grid size in y
std::shared_ptr<const Acts::DigitizationModule> trapezoidalDigiModuleXZ(double minHalflengthX, double maxHalflengthX,
                                                                        double halflengthZ, double thickness,
                                                                        double gridSizeX, double gridSizeZ);
}
}

#endif  // DETCOMMON_DIGIMODULEHELPER_H
