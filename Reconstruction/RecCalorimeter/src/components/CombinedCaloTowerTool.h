#ifndef RECCALORIMETER_COMBINEDCALOTOWERTOOL_H
#define RECCALORIMETER_COMBINEDCALOTOWERTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ITowerTool.h"
#include "DetSegmentation/GridPhiEta.h"

class IGeoSvc;

// datamodel
namespace fcc {
class CaloHitCollection;
}

namespace DD4hep {
namespace DDSegmentation {
class Segmentation;
}
}

/** @class CombinedCaloTowerTool Reconstruction/RecCalorimeter/src/components/CombinedCaloTowerTool.h CombinedCaloTowerTool.h
 *
 *  Tool building the calorimeter towers for the sliding window algorithm.
 *  Towers are built of cells in eta-phi, summed over all radial layers.
 *  A tower contains all cells within certain eta and phi (tower size: '\b deltaEtaTower', '\b deltaPhiTower').
 *  Distance in r plays no role, however `\b radiusForPosition` needs to be defined
 *  (e.g. to inner radius of the detector) for the cluster position calculation. By default the radius is equal to 1.
 *
 *  This tool creates towers for a combined calorimetry, so based on two cell collections (from electromagnetic and hadronic calorimeters).
 *
 *  For more explanation please [see reconstruction documentation](@ref md_reconstruction_doc_reccalorimeter).
 *
 *  @author Anna Zaborowska
 *  @author Jana Faltova
 */

class CombinedCaloTowerTool : public GaudiTool, virtual public ITowerTool {
public:
  CombinedCaloTowerTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~CombinedCaloTowerTool() = default;
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;
  /**  Find number of calorimeter towers.
   *   Number of towers in phi is calculated from full azimuthal angle (2 pi) and the size of tower in phi ('\b deltaPhiTower').
   *   Number of towers in eta is calculated from maximum detector eta ('\b etaMax`) and the size of tower in eta ('\b deltaEtaTower').
   *   @return Array containing number of towers in eta and phi.
   */
  virtual tower towersNumber() final;
  /**  Build calorimeter towers.
   *   Tower is defined by a segment in eta and phi, with the energy from all layers (no r segmentation).
   *   @param[out] aTowers Calorimeter towers.
   *   @return Size of the cell collection.
   */
  virtual uint buildTowers(std::vector<std::vector<float>> & aTowers) final;

  /**  Get the radius for the position calculation.
   *   @return Radius
   */
  virtual float radiusForPosition() const final;
  /**  Get the tower IDs in eta.
   *   @param[in] aEta Position of the calorimeter cell in eta
   *   @return ID (eta) of a tower
   */
  virtual uint idEta(float aEta) const final;
  /**  Get the tower IDs in phi.
   *   @param[in] aPhi Position of the calorimeter cell in phi
   *   @return ID (phi) of a tower
   */
  virtual uint idPhi(float aPhi) const final;
  /**  Get the eta position of the centre of the tower.
   *   @param[in] aIdEta ID (eta) of a tower
   *   @return Position of the centre of the tower
   */
  virtual float eta(int aIdEta) const final;
  /**  Get the phi position of the centre of the tower.
   *   @param[in] aIdPhi ID (phi) of a tower
   *   @return Position of the centre of the tower
   */
  virtual float phi(int aIdPhi) const final;
  /**  Correct way to access the neighbour of the phi tower, taking into account
   * the full coverage in phi.
   *   Full coverage means that first tower in phi, with ID = 0 is a direct
   * neighbour of the last tower in phi with ID = m_nPhiTower - 1).
   *   @param[in] aIPhi requested ID of a phi tower, 
   *   may be < 0 or >=m_nPhiTower
   *   @return  ID of a tower - shifted and corrected 
   * (in [0, m_nPhiTower) range)
   */
  uint phiNeighbour(int aIPhi) const;
  /**  This is where the cell info is filled into towers
   *   @param[in] aTowers Calorimeter towers.
   *   @param[in] aCells Calorimeter cells collection.
   *   @param[in] aSegmentation Segmentation of the calorimeter
   */
  void CellsIntoTowers(std::vector<std::vector<float>>& aTowers, const fcc::CaloHitCollection* aCells, DD4hep::DDSegmentation::GridPhiEta* aSegmentation);
private:
  /// Handle for electromagnetic calorimeter cells (input collection)
  DataHandle<fcc::CaloHitCollection> m_ecalCells{"ecalCells", Gaudi::DataHandle::Reader, this};
  /// Handle for hadronic calorimeter cells (input collection)
  DataHandle<fcc::CaloHitCollection> m_hcalCells{"hcalCells", Gaudi::DataHandle::Reader, this};
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Name of the electromagnetic calorimeter readout
  Gaudi::Property<std::string> m_ecalReadoutName{this, "ecalReadoutName", "", "name of the ecal readout"};
  /// Name of thehadronic calorimeter readout
  Gaudi::Property<std::string> m_hcalReadoutName{this, "hcalReadoutName", "", "name of the hcal readout"};
  /// PhiEta segmentation of the electromagnetic detector (owned by DD4hep)
  DD4hep::DDSegmentation::GridPhiEta* m_ecalSegmentation;
  /// PhiEta segmentation of the hadronic detector (owned by DD4hep)
  DD4hep::DDSegmentation::GridPhiEta* m_hcalSegmentation;
  /// Radius used to calculate cluster position from eta and phi (in mm)
  Gaudi::Property<double> m_radius{this, "radiusForPosition", 1.0,
      "Radius used to calculate cluster position from eta and phi (in mm)"};
  /// Maximum eta of detector
  float m_etaMax;
  /// Maximum phi of the detector
  float m_phiMax;
  /// Size of the tower in eta
  Gaudi::Property<float> m_deltaEtaTower{this, "deltaEtaTower", 0.01, "Size of the tower in eta"};
  /// Size of the tower in phi
  Gaudi::Property<float> m_deltaPhiTower{this, "deltaPhiTower", 0.01, "Size of the tower in phi"};
  /// number of towers in eta (calculated from m_deltaEtaTower and m_etaMax)
  int m_nEtaTower;
  /// Number of towers in phi (calculated from m_deltaPhiTower)
  int m_nPhiTower;
};

#endif /* RECCALORIMETER_COMBINEDCALOTOWERTOOL_H */
