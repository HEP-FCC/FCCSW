#include "DetSegmentation/GridEta.h"

namespace DD4hep {
namespace DDSegmentation {

/// default constructor using an encoding string
GridEta::GridEta(const std::string& cellEncoding) : Segmentation(cellEncoding) {
  // define type and description
  _type = "GridEta";
  _description = "Eeta segmentation in the global coordinates";

  // register all necessary parameters
  registerParameter("grid_size_eta", "Cell size in Eta", m_gridSizeEta, 1., SegmentationParameter::LengthUnit);
  registerParameter("offset_eta", "Angular offset in eta", m_offsetEta, 0., SegmentationParameter::AngleUnit, true);
  registerIdentifier("identifier_eta", "Cell ID identifier for eta", m_etaID, "eta");
}

GridEta::GridEta(BitField64* decoder) : Segmentation(decoder) {
  // define type and description
  _type = "GridEta";
  _description = "Eeta segmentation in the global coordinates";

  // register all necessary parameters
  registerParameter("grid_size_eta", "Cell size in Eta", m_gridSizeEta, 1., SegmentationParameter::LengthUnit);
  registerParameter("offset_eta", "Angular offset in eta", m_offsetEta, 0., SegmentationParameter::AngleUnit, true);
  registerIdentifier("identifier_eta", "Cell ID identifier for eta", m_etaID, "eta");
}

/// determine the local based on the cell ID
Vector3D GridEta::position(const CellID& cID) const {
  _decoder->setValue(cID);
  return positionFromREtaPhi(1.0, eta(), 0.);
}

/// determine the cell ID based on the position
CellID GridEta::cellID(const Vector3D& /* localPosition */, const Vector3D& globalPosition, const VolumeID& vID) const {
  _decoder->setValue(vID);
  double lEta = etaFromXYZ(globalPosition);
  (*_decoder)[m_etaID] = positionToBin(lEta, m_gridSizeEta, m_offsetEta);
  return _decoder->getValue();
}

/// determine the pseudorapidity based on the current cell ID
double GridEta::eta() const {
  CellID etaValue = (*_decoder)[m_etaID].value();
  return binToPosition(etaValue, m_gridSizeEta, m_offsetEta);
}

/// determine the polar angle theta based on the cell ID
double GridEta::eta(const CellID& cID) const {
  _decoder->setValue(cID);
  CellID etaValue = (*_decoder)[m_etaID].value();
  return binToPosition(etaValue, m_gridSizeEta, m_offsetEta);
}
REGISTER_SEGMENTATION(GridEta)
}
}
