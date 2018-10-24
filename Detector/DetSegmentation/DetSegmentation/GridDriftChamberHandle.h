#ifndef DD4HEP_DDCORE_GRIDDRIFTCHAMBER_H
#define DD4HEP_DDCORE_GRIDDRIFTCHAMBER_H 1

// FCCSW
#include "DetSegmentation/GridDriftChamber.h"

// DD4hep
#include "DD4hep/Segmentations.h"
#include "DD4hep/detail/SegmentationsInterna.h"

/*
 *  @author    nalipour
 */

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

/// Namespace for base segmentations

// Forward declarations
class Segmentation;
template <typename T>
class SegmentationWrapper;

/// We need some abbreviation to make the code more readable.
typedef Handle<SegmentationWrapper<DDSegmentation::GridDriftChamber>> GridDriftChamberHandle;

class GridDriftChamber : public GridDriftChamberHandle {
public:
  /// Defintiion of the basic handled object
  typedef GridDriftChamberHandle::Object object;

public:
  /// Default constructor
  GridDriftChamber() = default;
  /// Copy constructor
  GridDriftChamber(const GridDriftChamber& e) = default;
  /// Copy Constructor from segmentation base object
  GridDriftChamber(const Segmentation& e) : Handle<Object>(e) {}
  /// Copy constructor from handle
  GridDriftChamber(const Handle<Object>& e) : Handle<Object>(e) {}
  /// Copy constructor from other polymorph/equivalent handle
  template <typename Q>
  GridDriftChamber(const Handle<Q>& e) : Handle<Object>(e) {}
  /// Assignment operator
  GridDriftChamber& operator=(const GridDriftChamber& seg) = default;
  /// Equality operator
  bool operator==(const GridDriftChamber& seg) const { return m_element == seg.m_element; }
  /// determine the position based on the cell ID
  inline Position position(const CellID& id) const { return Position(access()->implementation->position(id)); }

  /// determine the cell ID based on the position
  inline dd4hep::CellID cellID(const Position& local, const Position& global, const VolumeID& volID) const {
    return access()->implementation->cellID(local, global, volID);
  }

  // Getters
  inline double epsilon() const { return access()->implementation->epsilon(); }
  inline double innerRadius() const { return access()->implementation->innerRadius(); }
  inline double detectorLength() const { return access()->implementation->detectorLength(); }
  inline double offsetPhi() const { return access()->implementation->offsetPhi(); }
  inline const std::string& fieldNamePhi() const { return access()->implementation->fieldNamePhi(); }

  // Setters
  inline void setEpsilon(double aEpsilon) const { access()->implementation->setEpsilon(aEpsilon); }
  inline void setGeomParams(int layernum, double sizePhi, double radius, double eps) const {
    access()->implementation->setGeomParams(layernum, sizePhi, radius, eps);
  }

  inline void setWiresInLayer(int layer, int numWires) const {
    access()->implementation->setWiresInLayer(layer, numWires);
  }

  inline auto returnAllWires() const {
    access()->implementation->returnAllWires();
  }

  inline double phiFromXY(const Position& aposition) const { return access()->implementation->phiFromXY(aposition); }
  inline TVector3 distanceClosestApproach(const CellID& cID, const TVector3& hitPos) const {
    return access()->implementation->distanceClosestApproach(cID, hitPos);
  }
  inline double distanceTrackWire(const CellID& cID, const TVector3& hit_start, const TVector3& hit_end) const {
    return access()->implementation->distanceTrackWire(cID, hit_start, hit_end);
  }
  inline TVector3 Line_TrackWire(const CellID& cID, const TVector3& hit_start, const TVector3& hit_end) const {
    return access()->implementation->Line_TrackWire(cID, hit_start, hit_end);
  }
};

} /* End namespace dd4hep                */
#endif  // DD4HEP_DDCORE_GRIDDRIFTCHAMBER_H
