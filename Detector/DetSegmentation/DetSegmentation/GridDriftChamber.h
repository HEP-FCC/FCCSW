#ifndef DETSEGMENTATION_GRIDDRIFTCHAMBER_H
#define DETSEGMENTATION_GRIDDRIFTCHAMBER_H

#include "DDSegmentation/Segmentation.h"

#include "TVector3.h"
#include <cmath>
#include <iostream>

/** GridDriftChamber Detector/DetSegmentation/DetSegmentation/GridDriftChamber.h GridDriftChamber.h
 *
 *  Segmentation for drift chamber.
 *
 *  @author    nalipour
 */

namespace dd4hep {
namespace DDSegmentation {
class GridDriftChamber : public Segmentation {
public:
  /// default constructor using an arbitrary type
  GridDriftChamber(const std::string& aCellEncoding);
  /// Default constructor used by derived classes passing an existing decoder
  GridDriftChamber(const BitFieldCoder* decoder);
  /// destructor
  virtual ~GridDriftChamber() = default;

  virtual Vector3D position(const CellID& aCellID) const;
  virtual CellID cellID(const Vector3D& aLocalPosition, const Vector3D& aGlobalPosition,
                        const VolumeID& aVolumeID) const;
  virtual TVector3 distanceClosestApproach(const CellID& cID, const TVector3& hitPos) const;
  virtual double distanceTrackWire(const CellID& cID, const TVector3& hit_start, const TVector3& hit_end) const;
  virtual TVector3 Line_TrackWire(const CellID& cID, const TVector3& hit_start, const TVector3& hit_end) const;
  virtual TVector3 IntersectionTrackWire(const CellID& cID, const TVector3& hit_start, const TVector3& hit_end) const;
  virtual TVector3 wirePos_vs_z(const CellID& cID, const double& zpos) const;

  inline double epsilon() const { return m_epsilon; }

  inline double innerRadius() const { return m_innerRadius; }
  inline double detectorLength() const { return m_detectorLength; }
  inline double offsetPhi() const { return m_offsetPhi; }
  inline const std::string& fieldNamePhi() const { return m_phiID; }
  // Setters
  inline void setEpsilon(double aEpsilon) { m_epsilon = aEpsilon; }

  inline void setGeomParams(int layer, double sizePhi, double R, double eps) {
    layer_params[layer] = {sizePhi, R, eps};
  }

  inline void setWiresInLayer(int layer, int numWires)
  {
    updateParams(layer);
    for (int i = 0; i<numWires; ++i)
      {
	auto phi_start = _currentGridSizePhi* i;
	auto phi_end = phi_start + returnAlpha();

	TVector3 Wstart = returnWirePosition(phi_start, 1);
	TVector3 Wend = returnWirePosition(phi_end, -1);
	
	TVector3 Wmid = (Wstart+Wend)*(1/2.0);
	TVector3 Wdirection = (Wend - Wstart);

	m_wiresPositions[layer].push_back(std::make_pair(Wmid, Wdirection));
      }
  }
  inline auto returnAllWires() const {return m_wiresPositions; }

  TVector3 LineLineIntersect(TVector3 p1, TVector3 p2, TVector3 p3, TVector3 p4) const {
    TVector3 p13, p43, p21;
    double d1343, d4321, d1321, d4343, d2121;
    double numer, denom;
    double mua, mub;
    TVector3 pa, pb;

    p13.SetX(p1.X() - p3.X());
    p13.SetY(p1.Y() - p3.Y());
    p13.SetZ(p1.Z() - p3.Z());
    p43.SetX(p4.X() - p3.X());
    p43.SetY(p4.Y() - p3.Y());
    p43.SetZ(p4.Z() - p3.Z());
    /* if (ABS(p43.X())  < EPS && ABS(p43.Y())  < EPS && ABS(p43.Z())  < EPS) */
    /*    return(FALSE); */
    p21.SetX(p2.X() - p1.X());
    p21.SetY(p2.Y() - p1.Y());
    p21.SetZ(p2.Z() - p1.Z());
    /* if (ABS(p21.X())  < EPS && ABS(p21.Y())  < EPS && ABS(p21.Z())  < EPS) */
    /*    return(FALSE); */

    d1343 = p13.X() * p43.X() + p13.Y() * p43.Y() + p13.Z() * p43.Z();
    d4321 = p43.X() * p21.X() + p43.Y() * p21.Y() + p43.Z() * p21.Z();
    d1321 = p13.X() * p21.X() + p13.Y() * p21.Y() + p13.Z() * p21.Z();
    d4343 = p43.X() * p43.X() + p43.Y() * p43.Y() + p43.Z() * p43.Z();
    d2121 = p21.X() * p21.X() + p21.Y() * p21.Y() + p21.Z() * p21.Z();

    denom = d2121 * d4343 - d4321 * d4321;
    /* if (ABS(denom) < EPS) */
    /*    return(FALSE); */
    numer = d1343 * d4321 - d1321 * d4343;

    mua = numer / denom;
    mub = (d1343 + d4321 * (mua)) / d4343;

    pa.SetX(p1.X() + mua * p21.X());
    pa.SetY(p1.Y() + mua * p21.Y());
    pa.SetZ(p1.Z() + mua * p21.Z());
    pb.SetX(p3.X() + mub * p43.X());
    pb.SetY(p3.Y() + mub * p43.Y());
    pb.SetZ(p3.Z() + mub * p43.Z());

    return pb - pa;
  }

  void updateParams(int layer) const {
    auto it_end = layer_params.cend();
    --it_end;
    double size = it_end->second[0];
    double radius = it_end->second[1];
    double eps = it_end->second[2];

    auto map_it = layer_params.find(layer);
    if (map_it != layer_params.cend()) {
      size = map_it->second[0];
      radius = map_it->second[1];
      eps = map_it->second[2];
    }

    _currentGridSizePhi = size;
    _currentRadius = radius;
    m_epsilon = eps;
  }

  inline double debug_projectToXY(const TVector3& pos) const {
    double gridSizePhi = _currentGridSizePhi;
    double _phi = pos.Phi();
    if (_phi < 0) {
      _phi += 2 * M_PI;
    }

    // distance between X,Y and the projected position of the Z on the (X,Y) plane

    double _L = (m_detectorLength / 2. - pos.Z()) * std::tan(m_epsilon);
    double sign = 1.;
    if (_L < 0) {
      sign = -1;
    }
    double _crd = _L * sign / _currentRadius;  // _crd>0
    double _theta = 2 * std::asin(_crd / 2) * sign;
    double _totalAngle = _phi + _theta;
    if (_totalAngle < 0) {
      _totalAngle += 2 * M_PI;
    }
    int wire = int(_totalAngle / gridSizePhi);

    return _totalAngle;
  }

  inline Vector3D returnPosWire0(double z) const {
    double alpha = returnAlpha();
    double t = 0.5 * (1 - 2.0 * z / m_detectorLength);
    double x = _currentRadius * (1 + t * (std::cos(alpha) - 1));
    double y = _currentRadius * t * std::sin(alpha);

    Vector3D vec(x, y, z);
    return vec;
  }

  inline double phiFromXY(const Vector3D& aposition) const { return std::atan2(aposition.Y, aposition.X) + M_PI; }

  inline double projectToXY(const Vector3D& aposition) const {
    double gridSizePhi = _currentGridSizePhi;
    // aposition is a global position
    double _phi = phiFromXY(aposition);

    // distance between X,Y and the projected position of the Z on the (X,Y) plane
    double _L = (m_detectorLength / 2. - aposition.Z) * std::tan(m_epsilon);
    // Chord
    double _crd = _L / _currentRadius;

    double _theta = 2 * std::asin(_crd / 2);
    double _totalAngle = _phi + _theta;

    // Angles are between 0 and 360 deg.
    if (_totalAngle < 0) {
      _totalAngle += 2 * M_PI;
    }

    int wire = int(_totalAngle / gridSizePhi);
    return _totalAngle;
  }

  inline double returnAlpha() const {
    double alpha = 2 * std::asin(m_detectorLength * std::tan(m_epsilon) / (2 * _currentRadius));
    return alpha;
  }

  inline TVector3 returnWirePosition(double angle, int sign) const {
    TVector3 w(0, 0, 0);
    // std::cout << "returnWirePosition: _currentRadius: " << _currentRadius << std::endl;

    w.SetX(_currentRadius * std::cos(angle));
    w.SetY(_currentRadius * std::sin(angle));
    w.SetZ(sign * m_detectorLength / 2.0);
    return w;
  }

  inline int returnLayer(double x, double y) const {
    // Hit R position
    double R = std::sqrt(x * x + y * y);
    // Layer
    int layer = int((R - m_innerRadius) / m_cellSize);

    return layer;
  }

protected:
  /* *** nalipour *** */
  double phi(const CellID& cID) const;

  std::map<int, std::vector<double>> layer_params;  // <layer, {phi, R, eps}>
  std::map<int, std::vector<std::pair<TVector3, TVector3> >> m_wiresPositions;   // < layer, vec<WireMidpoint, WireDirection> >

  double m_innerRadius;  // R0 // [cm]
  double m_cellSize;
  double m_detectorLength;
  double m_offsetPhi;
  std::string m_phiID;

  // Current parameters of the layer: sizePhi
  mutable double _currentGridSizePhi;  // current size Phi
  mutable double _currentRadius;       // current size radius
  mutable double m_epsilon;
};
}
}
#endif /* DETSEGMENTATION_GRIDDRIFTCHAMBER_H */
