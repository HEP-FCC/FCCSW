#ifndef DETSEGMENTATION_GRIDETA_H
#define DETSEGMENTATION_GRIDETA_H

#include "DDSegmentation/Segmentation.h"

/* #include "DDSegmentation/SegmentationUtil.h" */
#include "TVector3.h"
#include <cmath>

/** GridEta Detector/DetSegmentation/DetSegmentation/GridEta.h GridEta.h
 *
 *  Segmentation in eta.
 *
 *  @author    Anna Zaborowska
 */

namespace DD4hep {
namespace DDSegmentation {
class GridEta : public Segmentation {
public:
  /// default constructor using an arbitrary type
  GridEta(const std::string& aCellEncoding);
  /// Default constructor used by derived classes passing an existing decoder
  GridEta(BitField64* decoder);
  /// destructor
  virtual ~GridEta() = default;

  /**  Determine the global position based on the cell ID.
   *   @warning This segmentation has no knowledge of radius, so radius = 1 is taken into calculations.
   *   @param[in] aCellId ID of a cell.
   *   return Position (radius = 1).
   */
  virtual Vector3D position(const CellID& aCellID) const;
  /**  Determine the cell ID based on the position.
   *   @param[in] aLocalPosition (not used).
   *   @param[in] aGlobalPosition position in the global coordinates.
   *   @param[in] aVolumeId ID of a volume.
   *   return Cell ID.
   */
  virtual CellID cellID(const Vector3D& aLocalPosition, const Vector3D& aGlobalPosition,
                        const VolumeID& aVolumeID) const;
  /**  Determine the pseudorapidity based on the cell ID.
   *   @param[in] aCellId ID of a cell.
   *   return Pseudorapidity.
   */
  double eta(const CellID& aCellID) const;
  /**  Get the grid size in pseudorapidity.
   *   return Grid size in eta.
   */
  inline double gridSizeEta() const { return m_gridSizeEta; }
  /**  Get the coordinate offset in pseudorapidity.
   *   return The offset in eta.
   */
  inline double offsetEta() const { return m_offsetEta; }
  /**  Get the field name used for pseudorapidity
   *   return The field name for eta.
   */
  inline const std::string& fieldNameEta() const { return m_etaID; }
  /**  Set the grid size in pseudorapidity.
   *   @param[in] aCellSize Cell size in eta.
   */
  inline void setGridSizeEta(double aCellSize) { m_gridSizeEta = aCellSize; }
  /**  Set the coordinate offset in pseudorapidity.
   *   @param[in] aOffset Offset in eta.
   */
  inline void setOffsetEta(double offset) { m_offsetEta = offset; }
  /**  Set the field name used for pseudorapidity.
   *   @param[in] aFieldName Field name for eta.
   */
  inline void setFieldNameEta(const std::string& fieldName) { m_etaID = fieldName; }
  /// calculates the Cartesian position from spherical coordinates (r, phi, eta)
  inline Vector3D positionFromREtaPhi(double ar, double aeta, double aphi) const {
    return Vector3D(ar * std::cos(aphi), ar * std::sin(aphi), ar * std::sinh(aeta));
  }
  /// calculates the pseudorapidity from Cartesian coordinates
  inline double etaFromXYZ(const Vector3D& aposition) const {
    TVector3 vec(aposition.X, aposition.Y, aposition.Z);
    return vec.Eta();
  }
  /// from SegmentationUtil
  /// to be removed once SegmentationUtil can be included w/o linker error
  /// calculates the azimuthal angle phi from Cartesian coordinates
  inline double phiFromXYZ(const Vector3D& aposition) const { return std::atan2(aposition.Y, aposition.X); }
  /// from SegmentationUtil
  /// to be removed once SegmentationUtil can be included w/o linker error
  /// calculates the radius in the xy-plane from Cartesian coordinates
  inline double radiusFromXYZ(const Vector3D& aposition) const {
    return std::sqrt(aposition.X * aposition.X + aposition.Y * aposition.Y);
  }

protected:
  /// determine the pseudorapidity based on the current cell ID
  double eta() const;
  /// the grid size in eta
  double m_gridSizeEta;
  /// the coordinate offset in eta
  double m_offsetEta;
  /// the field name used for eta
  std::string m_etaID;
};
}
}
#endif /* DETSEGMENTATION_GRIDETA_H */
