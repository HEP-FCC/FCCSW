#include "BFieldG4CrossCheck.h"
#include <limits>
#include "Acts/Utilities/Units.hpp"
#include "G4SystemOfUnits.hh"

BFieldG4CrossCheck::BFieldG4CrossCheck(const std::string& name, ISvcLocator* svcLoc) : Service(name, svcLoc) {
  declareProperty("g4magneticField", m_g4MagneticFieldTool, "Handle for the magnetic field initialization");
}

StatusCode BFieldG4CrossCheck::initialize() {
  if (Service::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }

  if (!m_bFieldSvc) {
    error() << "No magnetic field service handed over." << endmsg;
    return StatusCode::FAILURE;
  }

  if (!m_g4MagneticFieldTool.retrieve()) {
    error() << "Unable to retrieve the g4 magnetic field tool" << endmsg;
    return StatusCode::FAILURE;
  }
  // get the G4 magnetic field
  const G4MagneticField* g4Field = m_g4MagneticFieldTool->field();
  if (!g4Field) {
    error() << "Unable to retrieve the g4 magnetic field" << endmsg;
    return StatusCode::FAILURE;
  }

  // apply units to input
  m_xMin = m_xMin * Acts::units::_mm;
  m_yMin = m_yMin * Acts::units::_mm;
  m_zMin = m_zMin * Acts::units::_mm;

  m_xMax = m_xMax * Acts::units::_mm;
  m_yMax = m_yMax * Acts::units::_mm;
  m_zMax = m_zMax * Acts::units::_mm;

  // the step sizes
  double stepX = fabs(m_xMax - m_xMin) / m_xBins;
  double stepY = fabs(m_yMax - m_yMin) / m_yBins;
  double stepZ = fabs(m_zMax - m_zMin) / m_zBins;
  StatusCode sc = StatusCode::SUCCESS;

  // write out
  for (int i = 0; i < m_xBins; i++) {
    double raw_x = m_xMin + i * stepX;
    for (int j = 0; j < m_yBins; j++) {
      double raw_y = m_yMin + j * stepY;
      for (int k = 0; k < m_zBins; k++) {
        double raw_z = m_zMin + k * stepZ;
        Acts::Vector3D position(raw_x, raw_y, raw_z);
        if (m_bFieldSvc->isInside(position)) {
          Acts::Vector3D bFieldG4(0., 0., 0.);
          // the g4 bField
          double* bFieldG4Ptr = bFieldG4.data();
          g4Field->GetFieldValue(position.data(), bFieldG4Ptr);
          // get the bField at the different posistion in Tesla
          auto bFieldActs = m_bFieldSvc->getField(position);
          // check value
          sc = equalTo(bFieldActs.x() / Acts::units::_T, bFieldG4.x() / CLHEP::tesla);
          if (sc != StatusCode::SUCCESS) return sc;

          sc = equalTo(bFieldActs.y() / Acts::units::_T, bFieldG4.y() / CLHEP::tesla);
          if (sc != StatusCode::SUCCESS) return sc;

          sc = equalTo(bFieldActs.z() / Acts::units::_T, bFieldG4.z() / CLHEP::tesla);
          if (sc != StatusCode::SUCCESS) return sc;
        }
      }  // for z
    }    // for y
  }      // for x

  return StatusCode::SUCCESS;
}

StatusCode BFieldG4CrossCheck::finalize() { return StatusCode::SUCCESS; }

StatusCode BFieldG4CrossCheck::equalTo(const double& a, const double& b) const {
  // check if relative error is smaller or equal to epsilon
  bool equal = (std::fabs(a - b) <= (m_epsilon * std::fabs(a + b)));
  if (!equal) {
    error() << "Values: " << a << " and " << b << " are not euqal to each other!" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

DECLARE_SERVICE_FACTORY(BFieldG4CrossCheck)
