#include "DetActs/DigiModuleHelper.h"
#include "ACTS/Surfaces/PlanarBounds.hpp"
#include "ACTS/Surfaces/RectangleBounds.hpp"
#include "ACTS/Surfaces/TrapezoidBounds.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "DD4hep/CartesianGridXZ.h"

std::shared_ptr<const Acts::DigitizationModule>
det::utils::rectangleDigiModuleXZ(double halflengthX, double halflengthZ, double thickness,
                                  const dd4hep::Segmentation& segmentation) {
  // convert to ACTS units
  double scalor = Acts::units::_cm;
  halflengthX *= scalor;
  halflengthZ *= scalor;
  thickness *= scalor;
  auto bounds = std::make_shared<const Acts::RectangleBounds>(halflengthX, halflengthZ);
  dd4hep::CartesianGridXZ cartesianGrid = segmentation;
  if (cartesianGrid.isValid()) {
    // the Acts segmentation of the DigitizationModule
    double gridSizeX = cartesianGrid.gridSizeX() * scalor;
    double gridSizeZ = cartesianGrid.gridSizeZ() * scalor;
    size_t bins0 = (cartesianGrid.gridSizeX() != 0) ? (2 * halflengthX) / gridSizeX : 0;
    size_t bins1 = (cartesianGrid.gridSizeZ() != 0) ? (2 * halflengthZ) / gridSizeZ : 0;

    std::shared_ptr<const Acts::CartesianSegmentation> actsSegmentation =
        std::make_shared<const Acts::CartesianSegmentation>(bounds, bins0, bins1);
    // finally create the digitization module
    // @todo set lorentz angle
    return (std::make_shared<const Acts::DigitizationModule>(actsSegmentation, thickness, 1, 0));
  }
  return nullptr;
}

std::shared_ptr<const Acts::DigitizationModule> det::utils::rectangleDigiModuleXZ(double halflengthX,
                                                                                  double halflengthZ, double thickness,
                                                                                  double gridSizeX, double gridSizeZ) {
  // convert to ACTS units
  double scalor = Acts::units::_cm;
  halflengthX *= scalor;
  halflengthZ *= scalor;
  thickness *= scalor;
  auto bounds = std::make_shared<const Acts::RectangleBounds>(halflengthX, halflengthZ);
  // the Acts segmentation of the DigitizationModule
  size_t bins0 = (gridSizeX != 0) ? (2 * halflengthX) / (gridSizeX * scalor) : 0;
  size_t bins1 = (gridSizeZ != 0) ? (2 * halflengthZ) / (gridSizeZ * scalor) : 0;

  std::shared_ptr<const Acts::CartesianSegmentation> actsSegmentation =
      std::make_shared<const Acts::CartesianSegmentation>(bounds, bins0, bins1);

  // finally create the digitization module
  // @todo set lorentz angle
  return (std::make_shared<const Acts::DigitizationModule>(actsSegmentation, thickness, 1, 0));
}

std::shared_ptr<const Acts::DigitizationModule>
det::utils::trapezoidalDigiModuleXZ(double minHalflengthX,
                                    double maxHalflengthX,
                                    double halflengthZ,
                                    double thickness,
                                    const dd4hep::Segmentation& segmentation) {
  // convert to ACTS units
  double scalor = Acts::units::_cm;
  minHalflengthX *= scalor;
  maxHalflengthX *= scalor;
  halflengthZ *= scalor;
  thickness *= scalor;

  auto bounds = std::make_shared<const Acts::TrapezoidBounds>(minHalflengthX, maxHalflengthX, halflengthZ);

  dd4hep::CartesianGridXZ cartesianGrid = segmentation;
  if (cartesianGrid.isValid()) {
    // the Acts segmentation of the DigitizationModule
    double gridSizeX = cartesianGrid.gridSizeX() * scalor;
    double gridSizeZ = cartesianGrid.gridSizeZ() * scalor;
    size_t bins0 = (cartesianGrid.gridSizeX() != 0) ? (2 * maxHalflengthX) / gridSizeX : 0;
    size_t bins1 = (cartesianGrid.gridSizeZ() != 0) ? (2 * halflengthZ) / gridSizeZ : 0;

    std::shared_ptr<const Acts::CartesianSegmentation> actsSegmentation =
        std::make_shared<const Acts::CartesianSegmentation>(bounds, bins0, bins1);
    // finally create the digitization module
    // @todo set lorentz angle
    return (std::make_shared<const Acts::DigitizationModule>(actsSegmentation, thickness, 1, 0));
  }
  return nullptr;
}

std::shared_ptr<const Acts::DigitizationModule>
det::utils::trapezoidalDigiModuleXZ(double minHalflengthX, double maxHalflengthX, double halflengthZ, double thickness,
                                    double gridSizeX, double gridSizeZ) {
  // convert to ACTS units
  double scalor = Acts::units::_cm;
  minHalflengthX *= scalor;
  maxHalflengthX *= scalor;
  halflengthZ *= scalor;
  thickness *= scalor;

  auto bounds = std::make_shared<const Acts::TrapezoidBounds>(minHalflengthX, maxHalflengthX, halflengthZ);

  // the Acts segmentation of the DigitizationModule
  size_t bins0 = (gridSizeX != 0) ? (2 * maxHalflengthX) / (gridSizeX * scalor) : 0;
  size_t bins1 = (gridSizeZ != 0) ? (2 * halflengthZ) / (gridSizeZ * scalor) : 0;

  std::shared_ptr<const Acts::CartesianSegmentation> actsSegmentation =
      std::make_shared<const Acts::CartesianSegmentation>(bounds, bins0, bins1);
  // finally create the digitization module
  // @todo set lorentz angle
  return (std::make_shared<const Acts::DigitizationModule>(actsSegmentation, thickness, 1, 0));
}
