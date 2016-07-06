#ifndef SIMG4COMMON_CONSTANTFIELD_H
#define SIMG4COMMON_CONSTANTFIELD_H

// Geant 4
#include "G4MagneticField.hh"

/** @class sim::ConstantField SimG4Common/SimG4Common/ConstantField.h ConstantField.h
*
*  Constant magnetic field inside the cylinder.
*
*  @author Andrea Dell'Acqua
*/

namespace sim {
class ConstantField : public G4MagneticField {
public:
  /// Default constructor
  ConstantField();
  /// Constructor setting parameters
  explicit ConstantField(double bX, double bY, double bZ, double rMax, double zMax);
  // Destructor
  virtual ~ConstantField() {}

  /// Get the value of the magnetic field value at position
  /// @param[in] point the position where the field is to be returned
  /// @param[out] bField the return value
  virtual void GetFieldValue(const G4double point[4], double* bField) const final;

  /// Set the x component of the field
  void setBx(double value) { m_bX = value; }
  /// Set the y component of the field
  void setBy(double value) { m_bY = value; }
  /// Set the z component of the field
  void setBz(double value) { m_bZ = value; }

  /// Set the extend of the field in radial direction
  void setMaxR(double value) { m_rMax = value; }
  /// Set the extend of the field in longitudinal direction
  void setMaxZ(double value) { m_zMax = value; }

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
}
#endif /* SIMG4COMMON_CONSTANTFIELD_H */
