#ifndef SIMG4COMMON_BFIELDG4_H
#define SIMG4COMMON_BFIELDG4_H

#include "Acts/MagneticField/concept/AnyFieldLookup.hpp"
#include "GaudiKernel/ServiceHandle.h"
#include "MagneticField/IBFieldSvc.h"
// Geant 4
#include "G4MagneticField.hh"

/** @class sim::BFieldG4 SimG4Common/SimG4Common/BFieldG4.h BFieldG4.h
*
*   Geant4 wrapper internally using the IBFieldSvc.
*
 *  @author Julia Hrdinka
 *  @date 2018-01
*/

namespace sim {
class BFieldG4 : public G4MagneticField {
public:
  /// Constructor
  BFieldG4(ServiceHandle<IBFieldSvc> bFieldSvc, double rMax = std::numeric_limits<double>::max(),
           double zMax = std::numeric_limits<double>::max());
  // Destructor
  virtual ~BFieldG4() = default;

  /// Get the value of the magnetic field value at position
  /// @param[in] point the position where the field is to be returned
  /// @param[out] bField the return value
  virtual void GetFieldValue(const double* point, double* field) const final override;

  /// Set the extend of the field in radial direction
  void setMaxR(double value) { m_rMax = value; }
  /// Set the extend of the field in longitudinal direction
  void setMaxZ(double value) { m_zMax = value; }

private:
  /// Shared pointer to the magnetic field service
  ServiceHandle<IBFieldSvc> m_bFieldSvc;
  /// Internal cash object of the magnetic field cell (in this way we only interpolate if needed)
  mutable Acts::concept::AnyFieldCell<> m_fieldCell;

  /// Extend of the field in radial direction
  double m_rMax;
  /// Extend of the field in longitudinal direction
  double m_zMax;
};
}

#endif  // SIMG4COMMON_BFIELDG4_H
