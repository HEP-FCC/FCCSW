#ifndef SIMG4COMMON_G4ConstantField_H
#define SIMG4COMMON_G4ConstantField_H

#include "G4MagneticField.hh"
#include "G4SystemOfUnits.hh"

class G4ConstantField: public G4MagneticField
{
public:
  /// Default constructor
  G4ConstantField() : m_bX(0), m_bY(0), m_bZ(0), m_rMax(9*m), m_zMax(8*m) {}
  /// Constructor setting parameters
  G4ConstantField(double bX, double bY, double bZ, double rMax, double zMax)
    : m_bX(bX), m_bY(bY), m_bZ(bZ), m_rMax(rMax), m_zMax(zMax) {}

  /// Get the value of the magnetic field value at position
  /// @param[in] point the position where the field is to be returned
  /// @param[out] bField the return value
  virtual void GetFieldValue(const G4double point[4], double* bField) const final {
    if (std::sqrt(point[0]*point[0]+point[1]*point[1]) < m_rMax &&
        std::abs(point[2]) < m_zMax) {
      bField[0] = m_bX;
      bField[1] = m_bY;
      bField[2] = m_bZ;
    }
    else bField[0] = bField[1] = bField[2] = 0;
  }

  /// Set the x component of the field
  void setBx(double value) {m_bX = value;}
  /// Set the y component of the field
  void setBy(double value) {m_bY = value;}
  /// Set the z component of the field
  void setBz(double value) {m_bZ = value;}

  /// Set the extend of the field in radial direction
  void setMaxR(double value) {m_rMax = value;}
  /// Set the extend of the field in longitudinal direction
  void setMaxZ(double value) {m_zMax = value;}
private:
  /// Field component in x
  double m_bX;
  /// Field component in y
  double m_bY;
  /// Field component in Z
  double m_bZ;

  /// Extend of the field in radial direction
  double m_rMax;
  /// Extend of the field in longitudinal direction
  double m_zMax;
};

#endif
