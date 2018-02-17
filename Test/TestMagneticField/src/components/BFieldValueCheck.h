#ifndef TESTMAGNETICFIELD_BFIELDVALUECHECK_H
#define TESTMAGNETICFIELD_BFIELDVALUECHECK_H

#include "GaudiKernel/Property.h"
#include "GaudiKernel/Service.h"
#include "MagneticField/IBFieldSvc.h"

/** @class BFieldValueCheck MagneticField/src/components/BFieldValueCheck.h BFieldValueCheck.h
 *
 *  @brief Checks if magnetic field map was correctly created
 *
 *  This test reads in a given root magnetic field map which should contain the correct magnetic field values at a given
 * position and compares the magnetic field values with the ones given by the BFieldSvc at the same position.
 *
 *  @author J. Hrdinka
 *  @date 2018-02
 */

class BFieldValueCheck : public Service {
public:
  /// constructor
  explicit BFieldValueCheck(const std::string& name, ISvcLocator* svcLoc);
  /// The interface initialize() method
  /// @note Since writing out the magnetic field does not need its own event loop the initialize method writes out the
  /// magnetic field.
  virtual StatusCode initialize();
  /// The interface finalize method
  virtual StatusCode finalize();
  /// default destructor
  virtual ~BFieldValueCheck() = default;

private:
  /// Handle to the magnetic field service
  ServiceHandle<IBFieldSvc> m_bFieldSvc{this, "bFieldSvc", "", "The magnetic field service"};
  /// path to root file containing field map
  /// @note For the description of the format of such a file, please refer to
  /// the documentation for InterpolatedBFieldMap::InterpolatedBFieldMap.
  Gaudi::Property<std::string> m_fieldMapFile{this, "fieldMap", "", "Path to root file containing field map"};
  /// Root Tree name
  Gaudi::Property<std::string> m_treeName{
      this, "treeName", "bField",
      "In case your field map file is given in root format, please specify the name of the TTree"};
  /// Scalor to mm of for position values used in the field map
  Gaudi::Property<double> m_lengthScalor{this, "lengthScalor", 1.,
                                         "Scalor to Millimeter, in case grid points of map are given in other unit"};
  /// Scalor to Tesla of for bField values used in the field map
  Gaudi::Property<double> m_bFieldScalor{
      this, "bFieldScalor", 1., "Scalor to Tesla, in case magnetic field values of map are given in other unit"};
  /// tolerance for the relative error
  Gaudi::Property<double> m_epsilon{this,"epsilon",10e-10,"Tolerance of the relative error during the euqality check"};
  // Method to check of two values are equal to each within a given tolerance epsilon
  StatusCode equalTo(const double& a, const double& b) const;
};

#endif  // TESTMAGNETICFIELD_BFIELDVALUECHECK_H
