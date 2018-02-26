/** @class IBFieldSvc MagneticField/MagneticField/IBFieldSvc.h
 * IBFieldSvc.h
 *
 *  Abstract interface for the magnetic field service. It provides the magnetic
 *  field value at any given point in the detector.
 *
 *
 *  @author Julia Hrdinka
 *  @date 2015-06
 */

#ifndef MAGNETICFIELD_IBFIELDSVC_H
#define MAGNETICFIELD_IBFIELDSVC_H

#include "ACTS/MagneticField/concept/AnyFieldLookup.hpp"
#include "GaudiKernel/IService.h"
#include "Math/Math.h"

class GAUDI_API IBFieldSvc : virtual public IService {
public:
  /// InterfaceID
  DeclareInterfaceID(IBFieldSvc, 1, 0);

  /// @brief retrieve magnetic field value
  ///
  /// @param [in] pos global position
  /// @return magnetic field vector at given position
  virtual fcc::Vector3D getField(const fcc::Vector3D& position) const = 0;

  /// @brief retrieve field cell for given position
  ///
  /// @param [in] position global 3D position
  /// @return field cell containing the given global position
  virtual Acts::concept::AnyFieldCell<> getFieldCell(const fcc::Vector3D& position) const = 0;

  /// @brief retrieve magnetic field value
  ///
  /// @param [in]  pos  global position
  /// @param [out] deriv gradient of magnetic field vector as (3x3) matrix
  /// @return magnetic field vector
  ///
  /// @note currently the derivative is not calculated
  /// @todo return derivative
  virtual fcc::Vector3D getFieldGradient(const fcc::Vector3D& pos, fcc::MatrixD<3, 3>& deriv) const = 0;

  // @brief check whether given 3D position is inside look-up domain
  ///
  /// @param [in] position global 3D position
  /// @return @c true if position is inside the defined look-up grid,
  ///         otherwise @c false
  virtual bool isInside(const fcc::Vector3D& position) const = 0;
  /// virtual desctructor
  virtual ~IBFieldSvc() {}
};

#endif  // MAGNETICFIELD_IBFIELDSVC_H
