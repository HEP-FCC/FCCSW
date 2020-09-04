#ifndef RECCALORIMETER_COMBINEDCALOTOWERTOOL_H
#define RECCALORIMETER_COMBINEDCALOTOWERTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ITowerTool.h"
#include "DetSegmentation/FCCSWGridPhiEta.h"

class IGeoSvc;

// datamodel
namespace fcc {
class CaloHitCollection;
}

namespace dd4hep {
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
   *   If max eta is undefined, 0 is returned. In this case number of eta towers should be calculated for each event separately (etaTowersNumber()).
   *   @return Array containing number of towers in eta and phi.
   */
  virtual tower towersNumber() final;
  /**  Find number of calorimeter towers in eta for current event.
   *   Cell collections are searched for a highest eta (absolute value).
   *   @return Number of towers in eta for current event.
   */
  virtual int etaTowersNumber() final;
  /**  Build calorimeter towers.
   *   Tower is segmented in eta and phi, with the energy from all layers (no r segmentation).
   *   Currently the size of tower needs to be a multiple of a cell size (so each cell belongs to only one tower).
   *   TODO: split cell energy into more towers if cell size is larger than the tower.
   *   @param[out] aTowers Calorimeter towers.
   *   @return Size of the cell collection.
   */
  virtual uint buildTowers(std::vector<std::vector<float>> & aTowers) final;
  /**  Find cells belonging to a cluster.
   *   @param[in] aEta Position of the middle tower of a cluster in eta
   *   @param[in] aPhi Position of the middle tower of a cluster in phi
   *   @param[in] aHalfEtaFinal Half size of cluster in eta (in units of tower size). Cluster size is 2*aHalfEtaFinal+1
   *   @param[in] aHalfPhiFinal Half size of cluster in phi (in units of tower size). Cluster size is 2*aHalfPhiFinal+1
   *   @param[out] aEdmCluster Cluster where cells are attached to
   */
  virtual void matchCells(float aEta, float aPhi, uint aHalfEtaFinal, uint aHalfPhiFinal, fcc::CaloCluster& aEdmCluster) final;
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
private:
  /// Handle for electromagnetic calorimeter cells (input collection)
  DataHandle<fcc::CaloHitCollection> m_ecalCells{"ecalCells", Gaudi::DataHandle::Reader, this};
  /// Handle for hadronic calorimeter cells (input collection)
  DataHandle<fcc::CaloHitCollection> m_hcalCells{"hcalCells", Gaudi::DataHandle::Reader, this};
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Name of the electromagnetic calorimeter readout
  Gaudi::Property<std::string> m_ecalReadoutName{this, "readoutName", "ecalReadoutName", "name of the ecal readout"};
  /// Name of thehadronic calorimeter readout
  Gaudi::Property<std::string> m_hcalReadoutName{this, "readoutName", "hcalReadoutName", "name of the hcal readout"};
  /// PhiEta segmentation of the electromagnetic detector (owned by DD4hep)
  dd4hep::DDSegmentation::FCCSWGridPhiEta* m_ecalSegmentation;
  /// PhiEta segmentation of the hadronic detector (owned by DD4hep)
  dd4hep::DDSegmentation::FCCSWGridPhiEta* m_hcalSegmentation;
  /// Radius used to calculate cluster position from eta and phi
  double m_radius;
  /// Maximum eta of detector
  float m_etaMax;
  /// Size of the tower in eta
  float m_deltaEtaTower;
  /// Size of the tower in phi
  float m_deltaPhiTower;
  /// number of towers in eta (calculated from m_deltaEtaTower and m_etaMax)
  int m_nEtaTower;
  /// Number of towers in phi (calculated from m_deltaPhiTower)
  int m_nPhiTower;
};

#endif /* RECCALORIMETER_COMBINEDCALOTOWERTOOL_H */
