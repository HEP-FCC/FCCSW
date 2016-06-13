// local
#include "SimG4Common/ConstantField.h"
// Geant 4
#include "G4SystemOfUnits.hh"

namespace sim {
ConstantField::ConstantField() : m_bX(0), m_bY(0), m_bZ(0), m_rMax(9 * m), m_zMax(8 * m) {}

ConstantField::ConstantField(double bX, double bY, double bZ, double rMax, double zMax)
    : m_bX(bX), m_bY(bY), m_bZ(bZ), m_rMax(rMax), m_zMax(zMax) {}

void ConstantField::GetFieldValue(const G4double point[4], double* bField) const {
  if (std::sqrt(point[0] * point[0] + point[1] * point[1]) < m_rMax && std::abs(point[2]) < m_zMax) {
    bField[0] = m_bX;
    bField[1] = m_bY;
    bField[2] = m_bZ;
  } else {
    bField[0] = bField[1] = bField[2] = 0;
  }
}
}
