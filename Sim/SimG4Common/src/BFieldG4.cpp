// local
#include "SimG4Common/BFieldG4.h"
#include "ACTS/Utilities/Units.hpp"
#include "Math/Math.h"
// Geant 4
#include "G4SystemOfUnits.hh"

namespace sim {
BFieldG4::BFieldG4(ServiceHandle<IBFieldSvc> bFieldSvc)
    : G4MagneticField(), m_bFieldSvc(bFieldSvc), m_fieldCell(m_bFieldSvc->getFieldCell(Acts::Vector3D{0., 0., 0.})) {}

void BFieldG4::GetFieldValue(const double* point, double* bField) const {
  // translate the point
  Acts::Vector3D position(point);
  // check if position is still in cache
  if (!m_fieldCell.isInside(position)) {
    // update field cache
    m_fieldCell = m_bFieldSvc->getFieldCell(position);
  }
  // get the field from the cell
  // divide by ACTS units to get bfield in testla & multiply by clhep units to tell geant4 that it is in tesla
  Acts::Vector3D field = (m_fieldCell.getField(position) / Acts::units::_T) * CLHEP::tesla;
  // tranlate the output
  std::memcpy(bField, field.data(), 3 * sizeof field.data());
}
}
