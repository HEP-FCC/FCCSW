#ifndef MAGNETICFIELD_INTERPOLATEDBFIELDSVC_H
#define MAGNETICFIELD_INTERPOLATEDBFIELDSVC_H

#include <string>
#include "Acts/MagneticField/InterpolatedBFieldMap.hpp"
#include "Acts/Utilities/Units.hpp"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "MagneticField/IBFieldSvc.h"

/** @class InterpolatedBFieldSvc InterpolatedBFieldSvc.h
 * MagneticField/InterpolatedBFieldSvc.h
 *
 *	This service is the implementation of the IBFieldSvc for magnetic field
 *  read in from a map. It provides the magnetic field value at any given point
 *  in the detector using tri-linear interpolation. This service is a wrapper
 *  which internally uses the Acts::InterpolatedBField in
 *  ACTS/MagneticField/InterpolatedBField.
 *
 *  @note currently the calculation of the derivatives is not implemented on the
 *  ACTS::ConstantBField
 *
 *  @author Julia Hrdinka
 *  @date 2017-03
 */

namespace Acts {
class InterpolatedBFieldMap;
}

class InterpolatedBFieldSvc : public extends1<Service, IBFieldSvc> {
public:
  /// @enum FileType describing of which type the file is
  enum FileType { root = 0, text = 1 };
  /// Default constructor
  InterpolatedBFieldSvc(const std::string& name, ISvcLocator* svc);
  /// Destructor
  virtual ~InterpolatedBFieldSvc() = default;
  /// Initialize function
  virtual StatusCode initialize() final override;
  /// Finalize function
  virtual StatusCode finalize() final override;
  /// @copydoc IBFieldSvc::getField(const Acts::Vector3D)
  virtual Acts::Vector3D getField(const Acts::Vector3D& position) const final override;
  /// @copydoc IBFieldSvc::getFieldCell(const Acts::Vector3D&)
  virtual Acts::concept::AnyFieldCell<> getFieldCell(const Acts::Vector3D& position) const final override;
  /// @copydoc IBFieldSvc::getField(const Acts::Vector3D&,Acts::Vector3D)
  virtual Acts::Vector3D getFieldGradient(const Acts::Vector3D& pos,
                                          Acts::ActsMatrixD<3, 3>& deriv) const final override;
  /// @copydoc IBFieldSvc::getField(const Acts::Vector3D&)
  /// @note The method will always return true for the constant B-Field
  virtual bool isInside(const Acts::Vector3D& position) const final override;

private:
  /// unique pointer to the internally used Acts::InterpolatedBField
  std::unique_ptr<Acts::InterpolatedBFieldMap> m_actsBField;
  /// path to file containing field map
  /// @note For the description of the format of such a file, please refer to
  /// the documentation for InterpolatedBFieldMap::InterpolatedBFieldMap.
  Gaudi::Property<std::string> m_fieldMapFile{this, "fieldMap", "", "path to file containing field map"};
  /// Tree name in case of a root file
  Gaudi::Property<std::string> m_treeName{
      this, "treeName", "",
      "in case your field map file is given in root format, please specify the name of the TTree"};
  /// @brief number of grid points in field map
  ///
  /// @note This information is only used as a hint for the required size of
  ///       the internal vectors. A correct value is not needed, but will help
  ///       to speed up the field map initialization process.
  Gaudi::Property<size_t> m_nPoints{
      this, "nPoints", 100000, "number of grid points in field map, needed for allocation only for txt & csv files"};
  /// Scalor to mm of for position values used in the field map
  Gaudi::Property<double> m_lengthScalor{this, "lengthScalor", 1.,
                                         "Scalor to Millimeter, in case grid points of map are given in other unit"};
  /// Scalor to Tesla of for bField values used in the field map
  Gaudi::Property<double> m_bFieldScalor{
      this, "bFieldScalor", 1., "Scalor to Tesla, in case magnetic field values of map are given in other unit"};
  /// Flag indicating if the coordinates of both position and bField values are given in cylindrical coordinates (rz)
  Gaudi::Property<bool> m_cylinderCoordinates{
      this, "cylinderCoordinates", false,
      "needs to be set to true in case magnetic field map is given in cylinder(rz) cooridnates"};
  /// Flag indicating if only the first octant/quadrant is given in the map
  Gaudi::Property<bool> m_firstOctant{
      this, "firstOctant", false, "needs to be set true, if only the first octant/quadrant is given in "
                                  "the map and the magnetic field should be mirrored for all other octants/quadrants"};

  /// Method to setup the FieldMapper
  /// @param localToGlobalBin Function mapping the local bins of r,z to the global bin of the map magnetic field value
  /// e.g.: we have small grid with the values: r={2,3}, z ={4,5}, the corresponding indices are i(r) and j(z), the
  /// globalIndex is M and the field map is:
  ///|| r | i || z | j || |B(r,z)| ||  M ||
  ///  -----------------------------------
  ///|| 2 | 0 || 4 | 0 ||  2.323   ||  0 ||
  ///|| 2 | 0 || 5 | 1 ||  2.334   ||  1 ||
  ///|| 3 | 1 || 4 | 0 ||  2.325   ||  2 ||
  ///|| 3 | 1 || 5 | 1 ||  2.331   ||  3 ||
  ///
  /// @code
  /// In this case the function would look like:
  /// [](std::array<size_t, 2> binsRZ, std::array<size_t, 2> nBinsRZ) {
  ///    return (binsRZ.at(0) * nBinsRZ.at(1) + binsRZ.at(1));
  /// }
  /// @endcode
  /// @param[in] fieldMapFile Path to file containing field map in txt format
  /// @param[in] lengthUnit The unit of the grid points
  /// @param[in] BFieldUnit The unit of the magnetic field
  /// @param[in] nPoints Extimate of number of grid points in field map needed
  /// for allocation
  /// @note This information is only used as a hint for the required size of the internal vectors. A correct value is
  ///       not needed, but will help to speed up the field map initialization process.
  /// @param[in] firstOctant Flag if set to true indicating that only the first quadrant of the grid points and the
  /// BField values has been given and that the BFieldMap should be created symmetrically for all quadrants. e.g. we
  /// have the grid values r={0,1} with BFieldValues={2,3} on the r axis. If the flag is set to true the r-axis grid
  /// values will be set to {-1,0,1} and the BFieldValues will be set to {3,2,3}.
  Acts::InterpolatedBFieldMap::FieldMapper<2, 2> fieldMapperRZ_txt(
      std::function<size_t(std::array<size_t, 2> binsRZ, std::array<size_t, 2> nBinsRZ)> localToGlobalBin,
      std::string fieldMapFile = "",
      double lengthUnit = Acts::units::_mm,
      double BFieldUnit = Acts::units::_T,
      size_t nPoints = 100000,
      bool firstOctant = false);

  /// Method to setup the FieldMapper
  /// @param localToGlobalBin Function mapping the local bins of x,y,z to the global bin of the map magnetic field value
  /// e.g.: we have small grid with the values: x={2,3}, y={3,4}, z ={4,5}, the corresponding indices are i(x), j(y) and
  /// z(k), the globalIndex is M and the field map is:
  ///|| x | i || y | j || z | k || |B(x,y,z)| ||  M ||
  ///  --------------------------------------------
  ///|| 2 | 0 || 3 | 0 || 4 | 0 ||  2.323   ||  0 ||
  ///|| 2 | 0 || 3 | 0 || 5 | 1 ||  2.334   ||  1 ||
  ///|| 2 | 0 || 4 | 1 || 4 | 0 ||  2.325   ||  2 ||
  ///|| 2 | 0 || 4 | 1 || 5 | 1 ||  2.331   ||  3 ||
  ///|| 3 | 1 || 3 | 0 || 4 | 0 ||  2.323   ||  4 ||
  ///|| 3 | 1 || 3 | 0 || 5 | 1 ||  2.334   ||  5 ||
  ///|| 3 | 1 || 4 | 1 || 4 | 0 ||  2.325   ||  6 ||
  ///|| 3 | 1 || 4 | 1 || 5 | 1 ||  2.331   ||  7 ||
  ///
  /// @code
  /// In this case the function would look like:
  /// [](std::array<size_t, 3> binsXYZ, std::array<size_t, 3> nBinsXYZ) {
  ///   return (binsXYZ.at(0) * (nBinsXYZ.at(1) * nBinsXYZ.at(2))
  ///        + binsXYZ.at(1) * nBinsXYZ.at(2)
  ///        + binsXYZ.at(2));
  /// }
  /// @endcode
  /// @param[in] fieldMapFile Path to file containing field map in txt format
  /// @param[in] lengthUnit The unit of the grid points
  /// @param[in] BFieldUnit The unit of the magnetic field
  /// @param[in] nPoints Extimate of number of grid points in field map needed for allocation
  /// @note This information is only used as a hint for the required size of the internal vectors. A correct value is
  ///       not needed, but will help to speed up the field map initialization process.
  /// @param[in] firstOctant Flag if set to true indicating that only the first octant of the grid points and the BField
  /// values has been given and that the BFieldMap should be created symmetrically for all quadrants. e.g. we have the
  /// grid values z={0,1} with BFieldValues={2,3} on the r axis. If the flag is set to true the z-axis grid values will
  /// be set to {-1,0,1} and the BFieldValues will be set to {3,2,3}.
  Acts::InterpolatedBFieldMap::FieldMapper<3, 3> fieldMapperXYZ_txt(
      std::function<size_t(std::array<size_t, 3> binsXYZ, std::array<size_t, 3> nBinsXYZ)> localToGlobalBin,
      std::string fieldMapFile = "",
      double lengthUnit = Acts::units::_mm,
      double BFieldUnit = Acts::units::_T,
      size_t nPoints = 100000,
      bool firstOctant = false);

  /// Method to setup the FieldMapper
  /// @param localToGlobalBin Function mapping the local bins of r,z to the global bin of the map magnetic field value
  /// e.g.: we have small grid with the values: r={2,3}, z ={4,5}, the corresponding indices are i(r) and j(z), the
  /// globalIndex is M and the field map is:
  ///|| r | i || z | j || |B(r,z)| ||  M ||
  ///  -----------------------------------
  ///|| 2 | 0 || 4 | 0 ||  2.323   ||  0 ||
  ///|| 2 | 0 || 5 | 1 ||  2.334   ||  1 ||
  ///|| 3 | 1 || 4 | 0 ||  2.325   ||  2 ||
  ///|| 3 | 1 || 5 | 1 ||  2.331   ||  3 ||
  ///
  /// @code
  /// In this case the function would look like:
  /// [](std::array<size_t, 2> binsRZ, std::array<size_t, 2> nBinsRZ) {
  ///    return (binsRZ.at(0) * nBinsRZ.at(1) + binsRZ.at(1));
  /// }
  /// @endcode
  /// @param[in] fieldMapFile Path to file containing field map in txt format
  /// @param[in] treeName The name of the root tree
  /// @param[in] lengthUnit The unit of the grid points
  /// @param[in] BFieldUnit The unit of the magnetic field
  /// @param[in] firstQuadrant Flag if set to true indicating that only the first quadrant of the grid points and the
  /// BField values has been given and that the BFieldMap should be created symmetrically for all quadrants. e.g. we
  /// have the grid values r={0,1} with BFieldValues={2,3} on the r axis. If the flag is set to true the r-axis grid
  /// values will be set to {-1,0,1} and the BFieldValues will be set to {3,2,3}.
  Acts::InterpolatedBFieldMap::FieldMapper<2, 2> fieldMapperRZ_root(
      std::function<size_t(std::array<size_t, 2> binsRZ, std::array<size_t, 2> nBinsRZ)> localToGlobalBin,
      std::string fieldMapFile = "",
      std::string treeName = "",
      double lengthUnit = Acts::units::_mm,
      double BFieldUnit = Acts::units::_T,
      bool firstOctant = false);

  /// Method to setup the FieldMapper
  /// @param localToGlobalBin Function mapping the local bins of x,y,z to the global bin of the map magnetic field value
  /// e.g.: we have small grid with the values: x={2,3}, y={3,4}, z ={4,5}, the corresponding indices are i(x), j(y) and
  /// z(k), the globalIndex is M and the field map is:
  ///|| x | i || y | j || z | k || |B(x,y,z)| ||  M ||
  ///  --------------------------------------------
  ///|| 2 | 0 || 3 | 0 || 4 | 0 ||  2.323   ||  0 ||
  ///|| 2 | 0 || 3 | 0 || 5 | 1 ||  2.334   ||  1 ||
  ///|| 2 | 0 || 4 | 1 || 4 | 0 ||  2.325   ||  2 ||
  ///|| 2 | 0 || 4 | 1 || 5 | 1 ||  2.331   ||  3 ||
  ///|| 3 | 1 || 3 | 0 || 4 | 0 ||  2.323   ||  4 ||
  ///|| 3 | 1 || 3 | 0 || 5 | 1 ||  2.334   ||  5 ||
  ///|| 3 | 1 || 4 | 1 || 4 | 0 ||  2.325   ||  6 ||
  ///|| 3 | 1 || 4 | 1 || 5 | 1 ||  2.331   ||  7 ||
  ///
  /// @code
  /// In this case the function would look like:
  /// [](std::array<size_t, 3> binsXYZ, std::array<size_t, 3> nBinsXYZ) {
  ///   return (binsXYZ.at(0) * (nBinsXYZ.at(1) * nBinsXYZ.at(2))
  ///        + binsXYZ.at(1) * nBinsXYZ.at(2)
  ///        + binsXYZ.at(2));
  /// }
  /// @endcode
  /// @param[in] fieldMapFile Path to file containing field map in txt format
  /// @param[in] treeName The name of the root tree
  /// @param[in] lengthUnit The unit of the grid points
  /// @param[in] BFieldUnit The unit of the magnetic field
  /// @param[in] firstOctant Flag if set to true indicating that only the first octant of the grid points and the BField
  /// values has been given and that the BFieldMap should be created symmetrically for all quadrants. e.g. we have the
  /// grid values z={0,1} with BFieldValues={2,3} on the r axis. If the flag is set to true the z-axis grid values will
  /// be set to {-1,0,1} and the BFieldValues will be set to {3,2,3}.
  Acts::InterpolatedBFieldMap::FieldMapper<3, 3> fieldMapperXYZ_root(
      std::function<size_t(std::array<size_t, 3> binsXYZ, std::array<size_t, 3> nBinsXYZ)> localToGlobalBin,
      std::string fieldMapFile = "",
      std::string treeName = "",
      double lengthUnit = Acts::units::_mm,
      double BFieldUnit = Acts::units::_T,
      bool firstOctant = false);

  /// @todo This method will be removed with the next update of ACTS
  /// Method to setup the FieldMapper
  /// @param localToGlobalBin Function mapping the local bins of r,z to the global
  /// bin of the map magnetic field value
  ///
  /// e.g.: we have small grid with the
  /// values: r={2,3}, z ={4,5}, the corresponding indices are i (belonging to r)
  /// and j (belonging to z), the
  /// globalIndex is M (belonging to the value of the magnetic field B(r,z)) and
  /// the field map is:
  ///|   r |    i |    z |    j |   B(r,z) |   M |
  ///|----:|:----:|:----:|:----:|:--------:|:----|
  ///|   2 |    0 |    4 |    0 |  2.323   |   0 |
  ///|   2 |    0 |    5 |    1 |  2.334   |   1 |
  ///|   3 |    1 |    4 |    0 |  2.325   |   2 |
  ///|   3 |    1 |    5 |    1 |  2.331   |   3 |
  ///
  /// In this case the function would look like:
  /// @code
  /// [](std::array<size_t, 2> binsRZ, std::array<size_t, 2> nBinsRZ) {
  ///    return (binsRZ.at(0) * nBinsRZ.at(1) + binsRZ.at(1));
  /// }
  /// @endcode
  /// @param[in] rPos Values of the grid points in r
  /// @note The values do not need to be sorted or unique (this will be done
  /// inside the function)
  /// @param[in] zPos Values of the grid points in z
  /// @note The values do not need to be sorted or unique (this will be done
  /// inside the function)
  /// @param[in] bField The magnetic field values inr r and z for all given grid
  /// points stored in a vector
  /// @note The function localToGlobalBin determines how the magnetic field was
  /// stored in the vector in respect to the grid values
  /// @param[in] lengthUnit The unit of the grid points
  /// @param[in] BFieldUnit The unit of the magnetic field
  /// @param[in] firstQuadrant Flag if set to true indicating that only the first
  /// quadrant of the grid points and the BField values has been given and that
  /// the BFieldMap should be created symmetrically for all quadrants.
  /// e.g. we have the grid values r={0,1} with BFieldValues={2,3} on the r axis.
  /// If the flag is set to true the r-axis grid values will be set to {-1,0,1}
  /// and the BFieldValues will be set to {3,2,3}.
  Acts::InterpolatedBFieldMap::FieldMapper<2, 2>
      fieldMapperRZ(std::function<size_t(std::array<size_t, 2> binsRZ, std::array<size_t, 2> nBinsRZ)> localToGlobalBin,
                    std::vector<double>
                        rPos,
                    std::vector<double>
                        zPos,
                    std::vector<Acts::Vector2D>
                        bField,
                    double lengthUnit = Acts::units::_mm,
                    double BFieldUnit = Acts::units::_T,
                    bool firstQuadrant = false);

  /// @todo This method will be removed with the next update of ACTS
  /// Method to setup the FieldMapper
  /// @param localToGlobalBin Function mapping the local bins of x,y,z to the
  /// global bin of the map magnetic field value
  ///
  /// e.g.: we have small grid with the
  /// values: x={2,3}, y={3,4}, z ={4,5}, the corresponding indices are i
  /// (belonging to x), j (belonging to y)
  /// and k (belonging to z), the globalIndex is M (belonging to the value of the
  /// magnetic field B(x,y,z)) and the field map is:
  ///
  ///| x   |    i |    y |    j |    z |    k | B(x,y,z) |   M |
  ///|----:|:----:|:----:|:----:|:----:|:----:|:--------:|:----|
  ///|   2 |    0 |    3 |    0 |    4 |    0 |  2.323   |   0 |
  ///|   2 |    0 |    3 |    0 |    5 |    1 |  2.334   |   1 |
  ///|   2 |    0 |    4 |    1 |    4 |    0 |  2.325   |   2 |
  ///|   2 |    0 |    4 |    1 |    5 |    1 |  2.331   |   3 |
  ///|   3 |    1 |    3 |    0 |    4 |    0 |  2.323   |   4 |
  ///|   3 |    1 |    3 |    0 |    5 |    1 |  2.334   |   5 |
  ///|   3 |    1 |    4 |    1 |    4 |    0 |  2.325   |   6 |
  ///|   3 |    1 |    4 |    1 |    5 |    1 |  2.331   |   7 |
  ///
  /// In this case the function would look like:
  /// @code
  /// [](std::array<size_t, 3> binsXYZ, std::array<size_t, 3> nBinsXYZ) {
  ///   return (binsXYZ.at(0) * (nBinsXYZ.at(1) * nBinsXYZ.at(2))
  ///        + binsXYZ.at(1) * nBinsXYZ.at(2)
  ///        + binsXYZ.at(2));
  /// }
  /// @endcode
  /// @param[in] xPos Values of the grid points in x
  /// @note The values do not need to be sorted or unique (this will be done
  /// inside the function)
  /// @param[in] yPos Values of the grid points in y
  /// @note The values do not need to be sorted or unique (this will be done
  /// inside the function)
  /// @param[in] zPos Values of the grid points in z
  /// @note The values do not need to be sorted or unique (this will be done
  /// inside the function)
  /// @param[in] bField The magnetic field values inr r and z for all given grid
  /// points stored in a vector
  /// @note The function localToGlobalBin determines how the magnetic field was
  /// stored in the vector in respect to the grid values
  /// @param[in] lengthUnit The unit of the grid points
  /// @param[in] BFieldUnit The unit of the magnetic field
  /// @param[in] firstOctant Flag if set to true indicating that only the first
  /// octant of the grid points and the BField values has been given and that
  /// the BFieldMap should be created symmetrically for all quadrants.
  /// e.g. we have the grid values z={0,1} with BFieldValues={2,3} on the r axis.
  /// If the flag is set to true the z-axis grid values will be set to {-1,0,1}
  /// and the BFieldValues will be set to {3,2,3}.
  Acts::InterpolatedBFieldMap::FieldMapper<3, 3> fieldMapperXYZ(
      std::function<size_t(std::array<size_t, 3> binsXYZ, std::array<size_t, 3> nBinsXYZ)> localToGlobalBin,
      std::vector<double>
          xPos,
      std::vector<double>
          yPos,
      std::vector<double>
          zPos,
      std::vector<Acts::Vector3D>
          bField,
      double lengthUnit = Acts::units::_mm,
      double BFieldUnit = Acts::units::_T,
      bool firstOctant = false);
};

#endif  // MAGNETICFIELD_INTERPOLATEDBFIELDSVC_H
