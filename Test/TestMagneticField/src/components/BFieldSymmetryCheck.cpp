#include "BFieldSymmetryCheck.h"
#include <limits>
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Utilities/Units.hpp"

BFieldSymmetryCheck::BFieldSymmetryCheck(const std::string& name, ISvcLocator* svcLoc) : Service(name, svcLoc) {}

StatusCode BFieldSymmetryCheck::initialize() {
  if (Service::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }

  if (!m_bFieldSvc) {
    error() << "No magnetic field service handed over." << endmsg;
    return StatusCode::FAILURE;
  }
  // apply units to input
  m_rMax = m_rMax * Acts::units::_mm;
  m_zMax = m_zMax * Acts::units::_mm;

  // the step sizes
  double stepX = m_rMax / m_rBins;
  double stepY = m_rMax / m_rBins;
  double stepZ = m_zMax / m_zBins;

  StatusCode sc = StatusCode::SUCCESS;

  // write out
  for (int i = 0; i < m_rBins; i++) {
    double raw_x = 0 + i * stepX;
    for (int j = 0; j < m_rBins; j++) {
      double raw_y = 0 + j * stepY;
      for (int k = 0; k < m_zBins; k++) {
        double raw_z = 0 + k * stepZ;
        Acts::Vector3D position0(raw_x, raw_y, raw_z);
        Acts::Vector3D position1(raw_x, raw_y, -raw_z);
        Acts::Vector3D position2(-raw_x, -raw_y, raw_z);
        Acts::Vector3D position3(-raw_x, raw_y, raw_z);
        Acts::Vector3D position4(raw_x, -raw_y, raw_z);
        if (m_bFieldSvc->isInside(position0)) {
          // get the bField at the different posistion in Tesla
          auto bField0 = m_bFieldSvc->getField(position0);
          auto bField1 = m_bFieldSvc->getField(position1);
          auto bField2 = m_bFieldSvc->getField(position2);
          auto bField3 = m_bFieldSvc->getField(position3);
          auto bField4 = m_bFieldSvc->getField(position4);
          // check phi symmetry
          sc = equalTo(Acts::VectorHelpers::perp(bField0), Acts::VectorHelpers::perp(bField2));
          if (sc != StatusCode::SUCCESS) return sc;

          sc = equalTo(Acts::VectorHelpers::perp(bField0), Acts::VectorHelpers::perp(bField3));
          if (sc != StatusCode::SUCCESS) return sc;

          sc = equalTo(Acts::VectorHelpers::perp(bField0), Acts::VectorHelpers::perp(bField4));
          if (sc != StatusCode::SUCCESS) return sc;

          // check z-symmetry
          sc = equalTo(bField0.z(), bField1.z());
          if (sc != StatusCode::SUCCESS) return sc;
        }
      }  // for z
    }    // for y
  }      // for x

  return StatusCode::SUCCESS;
}

StatusCode BFieldSymmetryCheck::finalize() { return StatusCode::SUCCESS; }

StatusCode BFieldSymmetryCheck::equalTo(const double& a, const double& b) const {
  // check if relative error is smaller or equal to epsilon
  bool equal = (std::fabs(a - b) <= (m_epsilon * std::fabs(a + b)));
  if (!equal) {
    error() << "Values: " << a << " and " << b << " are not euqal to each other!" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

DECLARE_SERVICE_FACTORY(BFieldSymmetryCheck)
