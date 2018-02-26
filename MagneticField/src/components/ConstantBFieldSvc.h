#ifndef MAGNETICFIELD_CONSTANTBFIELDSVC_H
#define MAGNETICFIELD_CONSTANTBFIELDSVC_H

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "MagneticField/IBFieldSvc.h"

/** @class ConstantBFieldSvc ConstantBFieldSvc.h
 * MagneticField/ConstantBFieldSvc.h
 *
 *	This service is the implementation of the IBFieldSvc for a constant
 *  magnetic field. It provides the magnetic field value at any given point in
 *  the detector. This service is a wrapper which internally uses the
 *  Acts::ConstantBField in ACTS/MagneticField/ConstantBField.
 *
 *  @note currently the calculation of the derivatives is not implemented
 *
 *  @author Julia Hrdinka
 *  @date 2017-03
 */

namespace Acts {
class ConstantBField;
}

class ConstantBFieldSvc : public extends1<Service, IBFieldSvc> {
public:
  /// Default constructor
  ConstantBFieldSvc(const std::string& name, ISvcLocator* svc);

  /// Destructor
  virtual ~ConstantBFieldSvc() = default;
  /// Initialize function
  virtual StatusCode initialize() final override;
  /// Finalize function
  virtual StatusCode finalize() final override;
  /// @copydoc IBFieldSvc::getField(const fcc::Vector3D&)
  virtual fcc::Vector3D getField(const fcc::Vector3D& position) const final override;
  /// @copydoc IBFieldSvc::getFieldCell(const fcc::Vector3D&)
  virtual Acts::concept::AnyFieldCell<> getFieldCell(const fcc::Vector3D& position) const final override;
  /// @copydoc IBFieldSvc::getField(const fcc::Vector3D&,fcc::MatrixD<3, 3>&)
  /// @note The @p position is ignored and only kept as argument to provide
  ///       a consistent interface with other magnetic field services.
  virtual fcc::Vector3D getFieldGradient(const fcc::Vector3D& pos, fcc::MatrixD<3, 3>& deriv) const final override;
  /// @copydoc IBFieldSvc::getField(const fcc::Vector3D&)
  /// @note The method will always return true for the constant B-Field
  virtual bool isInside(const fcc::Vector3D& position) const final override;
  /// @brief update magnetic field vector
  ///
  /// @param [in] fieldVector magnetic field vector in global coordinate system
  void setField(const fcc::Vector3D& fieldVector);

private:
  /// unique pointer to the internally used Acts::ConstantBField
  std::unique_ptr<Acts::ConstantBField> m_actsBField;
  /// the values of the magnetic field in cartesian coordinates xyz
  std::vector<float> m_bFieldValuesXYZ;
};

#endif  // MAGNETICFIELD_CONSTANTBFIELDSVC_H
