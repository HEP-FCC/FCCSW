#ifndef MAGNETICFIELD_BFIELDWRITER_H
#define MAGNETICFIELD_BFIELDWRITER_H

#include "GaudiKernel/Property.h"
#include "GaudiKernel/Service.h"
#include "MagneticField/IBFieldSvc.h"

/** @class BFieldWriter MagneticField/src/components/BFieldWriter.h BFieldWriter.h
 *
 *  Service that writes out the magnetic field onto a root file. This service is meant for testing the magnetic field
 * service and for visualisation of the magnetic field. The magnetic field can be written out either in cylinder or in
 * cartesian coordinates.
 *
 *  @author J. Hrdinka
 *  @date 2017-11
 */

class BFieldWriter : public Service {
public:
  /// constructor
  explicit BFieldWriter(const std::string& name, ISvcLocator* svcLoc);
  /// The interface initialize() method
  /// @note Since writing out the magnetic field does not need its own event loop the initialize method writes out the
  /// magnetic field.
  virtual StatusCode initialize();
  /// The interface finalize method
  virtual StatusCode finalize();
  /// default destructor
  virtual ~BFieldWriter() = default;

private:
  /// Handle to the magnetic field service
  ServiceHandle<IBFieldSvc> m_bFieldSvc{this, "bFieldSvc", "", "the magnetic field service"};
  /// name of the output file
  Gaudi::Property<std::string> m_filename{this, "filename", "bFieldTest.root",
                                          "file name to save the magnetic field map to"};
  /// name of the output tree
  Gaudi::Property<std::string> m_treename{this, "treename", "bFieldTest",
                                          "tree name to save the magnetic field map to"};
  /// The minimum value of the position in either r (for cylinder coordinates) or x/y (for cartesian coordinates) in mm
  Gaudi::Property<double> m_rMin{this, "rMin", 0., "minimum in r"};
  /// The maximum value of the position in either r (for cylinder coordinates) or x/y (for cartesian coordinates) in mm
  Gaudi::Property<double> m_rMax{this, "rMax", 20000., "maximum in r"};
  /// The number of bins in in either r (for cylinder coordinates) or x/y (for cartesian coordinates)
  Gaudi::Property<double> m_rBins{this, "rBins", 200, "#bins in r"};
  /// The minimum value of the position in z in mm
  Gaudi::Property<double> m_zMin{this, "zMin", -30000., "minimum in z"};
  /// The maximum value of the position in z in mm
  Gaudi::Property<double> m_zMax{this, "zMax", 30000., "maximum in z"};
  /// The number of bins in z
  Gaudi::Property<double> m_zBins{this, "zBins", 300, "#bins in z"};
  /// The number of bins in phi (only needs to be set for cyliner coordinates)
  Gaudi::Property<double> m_phiBins{this, "phiBins", 300, "#bins in phi"};
  /// Flag indicating if the magnetic field map should be written out in cartesian coordinates
  Gaudi::Property<bool> m_cartesian{this, "cartesian", false,
                                    "If the magnetic field should be written out in cartesian "
                                    "coordinates, please set this flag to 'TRUE'. Default "
                                    "are cylinder coordinates."};
};

#endif  // MAGNETICFIELD_BFIELDWRITER_H
