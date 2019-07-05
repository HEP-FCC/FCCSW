#include "DetSegmentation/GridDriftChamber.h"

namespace dd4hep {
namespace DDSegmentation {

/// default constructor using an encoding string
GridDriftChamber::GridDriftChamber(const std::string& cellEncoding) : Segmentation(cellEncoding) {
  // define type and description
  _type = "GridDriftChamber";
  _description = "Drift chamber segmentation in the global coordinates";

  registerParameter("inner_radius", "Inner radius of the detector", m_innerRadius, 0.,
                    SegmentationParameter::LengthUnit);
  registerParameter("cell_size", "cell size", m_cellSize, 0., SegmentationParameter::LengthUnit);
  registerParameter("detector_length", "Length of the wire", m_detectorLength, 1., SegmentationParameter::LengthUnit);
  registerParameter("offset_phi", "offset in phi", m_offsetPhi, 0., SegmentationParameter::LengthUnit, true);
  registerIdentifier("identifier_phi", "Cell ID identifier for phi", m_phiID, "phi");
}

GridDriftChamber::GridDriftChamber(const BitFieldCoder* decoder) : Segmentation(decoder) {
  // define type and description
  _type = "GridDriftChamber";
  _description = "Drift chamber segmentation in the global coordinates";

  registerParameter("inner_radius", "Inner radius of the detector", m_innerRadius, 1.,
                    SegmentationParameter::LengthUnit);
  registerParameter("cell_size", "cell size", m_cellSize, 1., SegmentationParameter::LengthUnit);
  registerParameter("detector_length", "Length of the detector", m_detectorLength, 1.,
                    SegmentationParameter::LengthUnit);
  registerParameter("offset_phi", "offset in phi", m_offsetPhi, 0., SegmentationParameter::LengthUnit, true);
  registerIdentifier("identifier_phi", "Cell ID identifier for phi", m_phiID, "phi");
}

Vector3D GridDriftChamber::position(const CellID& /*cID*/) const {  //// ???? TODO
  Vector3D cellPosition = {0, 0, 0};
  return cellPosition;
}

CellID GridDriftChamber::cellID(const Vector3D& /*localPosition*/, const Vector3D& globalPosition,
                                const VolumeID& vID) const {

  CellID cID = vID;
  unsigned int layerID = _decoder->get(vID, "layer");
  updateParams(layerID);

  double phi_hit = phiFromXY(globalPosition);
  double posz = globalPosition.Z;
  Vector3D wire0 = returnPosWire0(posz);
  double phi_wire0 = phiFromXY(wire0);
  double lphi = phi_hit - phi_wire0;
  if (lphi < 0) {
    lphi += 2 * M_PI;
  }

  _decoder->set(cID, m_phiID, positionToBin(lphi, _currentGridSizePhi, m_offsetPhi));
  return cID;
}

double GridDriftChamber::phi(const CellID& cID) const {
  CellID phiValue = _decoder->get(cID, m_phiID);
  return binToPosition(phiValue, _currentGridSizePhi, m_offsetPhi);
}

// Distance between a particle track and a wire
double GridDriftChamber::distanceTrackWire(const CellID& cID, const TVector3& hit_start,
                                           const TVector3& hit_end) const {
  auto layerIndex = _decoder->get(cID, "layer");
  updateParams(layerIndex);

  double phi_start = phi(cID);
  double phi_end = phi_start + returnAlpha();

  TVector3 Wstart = returnWirePosition(phi_start, 1);
  TVector3 Wend = returnWirePosition(phi_end, -1);

  TVector3 a = hit_end - hit_start;
  TVector3 b = Wend - Wstart;
  TVector3 c = Wstart - hit_start;

  double num = std::abs(c.Dot(a.Cross(b)));
  double denum = (a.Cross(b)).Mag();

  // Calculation of the distance of the closest approach
  // if a perpendicular to b -> denum zero -> track crossing the wire
  double DCA = 0;

  if (denum) {
    DCA = num / denum;
  }

  return DCA;
}

TVector3 GridDriftChamber::Line_TrackWire(const CellID& cID, const TVector3& hit_start, const TVector3& hit_end) const {
  // The line connecting a particle track to the closest wire
  // Returns the vector connecting the both
  auto layerIndex = _decoder->get(cID, "layer");
  updateParams(layerIndex);

  double phi_start = phi(cID);
  double phi_end = phi_start + returnAlpha();

  TVector3 Wstart = returnWirePosition(phi_start, 1);
  TVector3 Wend = returnWirePosition(phi_end, -1);

  TVector3 P1 = hit_start;
  TVector3 P2 = hit_end;
  TVector3 P3 = Wstart;
  TVector3 P4 = Wend;

  TVector3 intersect = LineLineIntersect(P1, P2, P3, P4);
  return intersect;
}

TVector3 GridDriftChamber::distanceClosestApproach(const CellID& cID, const TVector3& hitPos) const {
  // Distance of the closest approach between a single hit (point) and the closest wire

  auto layerIndex = _decoder->get(cID, "layer");
  updateParams(layerIndex);
  // std::cout << "segmentation: layer:" << layerIndex << std::endl;

  double phi_start = phi(cID);
  double phi_end = phi_start + returnAlpha();

  TVector3 Wstart = returnWirePosition(phi_start, 1);
  TVector3 Wend = returnWirePosition(phi_end, -1);
  TVector3 temp = (Wend + Wstart);
  TVector3 Wmid(temp.X() / 2.0, temp.Y() / 2.0, temp.Z() / 2.0);

  double hitPhi = hitPos.Phi();
  if (hitPhi < 0) {
    hitPhi = hitPhi + 2 * M_PI;
  }

  TVector3 PCA = Wstart + ((Wend - Wstart).Unit()).Dot((hitPos - Wstart)) * ((Wend - Wstart).Unit());
  TVector3 dca = hitPos - PCA;

  return dca;
}

// Get the wire position for a z
TVector3 GridDriftChamber::wirePos_vs_z(const CellID& cID, const double& zpos) const {
  auto layerIndex = _decoder->get(cID, "layer");
  updateParams(layerIndex);

  double phi_start = phi(cID);
  double phi_end = phi_start + returnAlpha();

  TVector3 Wstart = returnWirePosition(phi_start, 1);
  TVector3 Wend = returnWirePosition(phi_end, -1);

  double t = (zpos - Wstart.Z())/(Wend.Z()-Wstart.Z());
  double x = Wstart.X()+t*(Wend.X()-Wstart.X());
  double y = Wstart.Y()+t*(Wend.Y()-Wstart.Y());

  TVector3 wireCoord(x, y, zpos);
  return wireCoord;
}

TVector3 GridDriftChamber::IntersectionTrackWire(const CellID& cID, const TVector3& hit_start, const TVector3& hit_end) const {
  // Intersection between the particle track and the wire assuming that the track between hit_start and hit_end is linear
  auto layerIndex = _decoder->get(cID, "layer");
  updateParams(layerIndex);

  double phi_start = phi(cID);
  double phi_end = phi_start + returnAlpha();

  TVector3 Wstart = returnWirePosition(phi_start, 1);
  TVector3 Wend = returnWirePosition(phi_end, -1);

  TVector3 P1 = hit_start;
  TVector3 V1 = hit_end-hit_start;

  TVector3 P2 = Wstart;
  TVector3 V2 = Wend - Wstart;

  TVector3 denom = V1.Cross(V2);
  double mag_denom = denom.Mag();

  TVector3 intersect(0, 0, 0);

  if (mag_denom !=0)
    {
      TVector3 num = ((P2-P1)).Cross(V2);
      double mag_num = num.Mag();
      double a = mag_num / mag_denom;

      intersect = P1 + a * V1;
      
    }
 
  std::cout << "------- Segmentation -------" << std::endl;
  hit_start.Print();
  hit_end.Print();
  Wstart.Print();
  Wend.Print();
  intersect.Print();

  return intersect;
}



REGISTER_SEGMENTATION(GridDriftChamber)
}
}
