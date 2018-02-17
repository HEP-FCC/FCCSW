#ifndef TESTMAGNETICFIELD_BFIELDSYMMETRYCHECK_H
#define TESTMAGNETICFIELD_BFIELDSYMMETRYCHECK_H

#include "GaudiKernel/Property.h"
#include "GaudiKernel/Service.h"
#include "MagneticField/IBFieldSvc.h"

/** @class BFieldSymmetryCheck MagneticField/src/components/BFieldSymmetryCheck.h BFieldSymmetryCheck.h
 *
 *
 *  @author J. Hrdinka
 *  @date 2018-02
 */

class BFieldSymmetryCheck : public Service {
public:
  /// constructor
  explicit BFieldSymmetryCheck(const std::string& name, ISvcLocator* svcLoc);
  /// The interface initialize() method
  /// @note Since writing out the magnetic field does not need its own event loop the initialize method writes out the
  /// magnetic field.
  virtual StatusCode initialize();
  /// The interface finalize method
  virtual StatusCode finalize();
  /// default destructor
  virtual ~BFieldSymmetryCheck() = default;

private:
  /// Handle to the magnetic field service
  ServiceHandle<IBFieldSvc> m_bFieldSvc{this, "bFieldSvc", "", "the magnetic field service"};
  /// The maximum value of the position in either r (for cylinder coordinates) or x/y (for cartesian coordinates) in mm
  Gaudi::Property<double> m_rMax{this, "rMax", 20000., "maximum in r"};
  /// The number of bins in in either r (for cylinder coordinates) or x/y (for cartesian coordinates)
  Gaudi::Property<double> m_rBins{this, "rBins", 200, "#bins in r"};
  /// The maximum value of the position in z in mm
  Gaudi::Property<double> m_zMax{this, "zMax", 30000., "maximum in z"};
  /// The number of bins in z
  Gaudi::Property<double> m_zBins{this, "zBins", 300, "#bins in z"};
  /// The number of bins in phi (only needs to be set for cyliner coordinates)
  Gaudi::Property<double> m_phiBins{this, "phiBins", 300, "#bins in phi"};
  /// tolerance for the relative error
  Gaudi::Property<double> m_epsilon{this,"epsilon",10e-10,"Tolerance of the relative error during the euqality check"};
  // Method to check of two values are equal to each within the numeric limit epsilon
  StatusCode equalTo(const double& a, const double& b) const;
};

#endif  // TESTMAGNETICFIELD_BFIELDSYMMETRYCHECK_H
