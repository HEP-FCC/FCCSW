#include "DetSegmentation/GridPhiEta.h"

namespace DD4hep {
namespace DDSegmentation {

/// default constructor using an encoding string
GridPhiEta::GridPhiEta(const std::string& cellEncoding) : GridEta(cellEncoding) {
  // define type and description
  _type = "GridPhiEta";
  _description = "Phi-eta segmentation in the global coordinates";

  // register all necessary parameters (additional to those registered in GridEta)
  registerParameter("phi_bins", "Number of bins phi", m_phiBins, 1);
  registerParameter("offset_phi", "Angular offset in phi", m_offsetPhi, 0., SegmentationParameter::AngleUnit, true);
  registerIdentifier("identifier_phi", "Cell ID identifier for phi", m_phiID, "phi");
}

GridPhiEta::GridPhiEta(BitField64* decoder) : GridEta(decoder) {
  // define type and description
  _type = "GridPhiEta";
  _description = "Phi-eta segmentation in the global coordinates";

  // register all necessary parameters (additional to those registered in GridEta)
  registerParameter("phi_bins", "Number of bins phi", m_phiBins, 1);
  registerParameter("offset_phi", "Angular offset in phi", m_offsetPhi, 0., SegmentationParameter::AngleUnit, true);
  registerIdentifier("identifier_phi", "Cell ID identifier for phi", m_phiID, "phi");
}

/// determine the local based on the cell ID
Vector3D GridPhiEta::position(const CellID& cID) const {
  _decoder->setValue(cID);
  return positionFromREtaPhi(1.0, eta(), phi());
}

/// determine the cell ID based on the position
CellID GridPhiEta::cellID(const Vector3D& /* localPosition */, const Vector3D& globalPosition,
                          const VolumeID& vID) const {
  _decoder->setValue(vID);
  double lEta = etaFromXYZ(globalPosition);
  double lPhi = phiFromXYZ(globalPosition);
  (*_decoder)[m_etaID] = positionToBin(lEta, m_gridSizeEta, m_offsetEta);
  (*_decoder)[m_phiID] = positionToBin(lPhi, 2 * M_PI / (double)m_phiBins, m_offsetPhi);
  return _decoder->getValue();
}

/// determine the azimuthal angle phi based on the current cell ID
double GridPhiEta::phi() const {
  CellID phiValue = (*_decoder)[m_phiID].value();
  return binToPosition(phiValue, 2. * M_PI / (double)m_phiBins, m_offsetPhi);
}

/// determine the azimuthal angle phi based on the cell ID
double GridPhiEta::phi(const CellID& cID) const {
  _decoder->setValue(cID);
  CellID phiValue = (*_decoder)[m_phiID].value();
  return binToPosition(phiValue, 2. * M_PI / (double)m_phiBins, m_offsetPhi);
}
}
}
