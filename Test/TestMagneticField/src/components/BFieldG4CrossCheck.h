#ifndef TESTMAGNETICFIELD_BFIELDG4CROSSCHECK_H
#define TESTMAGNETICFIELD_BFIELDG4CROSSCHECK_H

#include "GaudiKernel/Property.h"
#include "GaudiKernel/Service.h"
#include "MagneticField/IBFieldSvc.h"
#include "SimG4Interface/ISimG4MagneticFieldTool.h"

/** @class BFieldG4CrossCheck MagneticField/src/components/BFieldG4CrossCheck.h BFieldG4CrossCheck.h
 *
 *	@brief Testing the g4 magnetic field against plain implementation
 *
 *	This test is a consisteny check of the geant4 magnetic field tool used for simulation against the FCCSW BFieldSvc.
 *  The test loops through a position grid of given dimensions in x,y,z and compares the values of the magnetic fields
 *  given from the geant4 magnetic field tool and the BFieldSvc. In case the values do not match the test is stopped and
 *  an error message is printed.
 *
 *  @author J. Hrdinka
 *  @date 2018-02
 */

class BFieldG4CrossCheck : public Service {
public:
  /// constructor
  explicit BFieldG4CrossCheck(const std::string& name, ISvcLocator* svcLoc);
  /// The interface initialize() method
  /// @note Since writing out the magnetic field does not need its own event loop the initialize method writes out the
  /// magnetic field.
  virtual StatusCode initialize();
  /// The interface finalize method
  virtual StatusCode finalize();
  /// default destructor
  virtual ~BFieldG4CrossCheck() = default;

private:
  /// Handle to the magnetic field service
  ServiceHandle<IBFieldSvc> m_bFieldSvc{this, "bFieldSvc", "", "the magnetic field service"};
  /// Handle for the magnetic field initialization
  ToolHandle<ISimG4MagneticFieldTool> m_g4MagneticFieldTool{"SimG4BFieldTool", this, true};
  /// The minimum value of the position in x in mm
  Gaudi::Property<double> m_xMin{this, "xMin", 0., "minimum in x"};
  /// The maximum value of the position in x in mm
  Gaudi::Property<double> m_xMax{this, "xMax", 20000., "maximum in x"};
  /// The number of bins in in  x
  Gaudi::Property<double> m_xBins{this, "xBins", 200, "#bins in x"};
  /// The minimum value of the position in y in mm
  Gaudi::Property<double> m_yMin{this, "yMin", 0., "minimum in y"};
  /// The maximum value of the position in y in mm
  Gaudi::Property<double> m_yMax{this, "yMax", 20000., "maximum in y"};
  /// The number of bins in y
  Gaudi::Property<double> m_yBins{this, "yBins", 200, "#bins in y"};
  /// The minimum value of the position in z in mm
  Gaudi::Property<double> m_zMin{this, "zMin", -30000., "minimum in z"};
  /// The maximum value of the position in z in mm
  Gaudi::Property<double> m_zMax{this, "zMax", 30000., "maximum in z"};
  /// The number of bins in z
  Gaudi::Property<double> m_zBins{this, "zBins", 300, "#bins in z"};
  /// tolerance for the relative error
  Gaudi::Property<double> m_epsilon{this,"epsilon",10e-10,"Tolerance of the relative error during the euqality check"};
  // Method to check of two values are equal to each within the numeric limit epsilon
  StatusCode equalTo(const double& a, const double& b) const;
};

#endif  // TESTMAGNETICFIELD_BFIELDG4CROSSCHECK_H
