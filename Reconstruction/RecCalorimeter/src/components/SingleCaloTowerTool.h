#ifndef RECCALORIMETER_SINGLECALOTOWERTOOL_H
#define RECCALORIMETER_SINGLECALOTOWERTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "DetSegmentation/GridPhiEta.h"
#include "FWCore/DataHandle.h"
#include "RecInterface/ITowerTool.h"
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

/** @class SingleCaloTowerTool
 * Reconstruction/RecCalorimeter/src/components/SingleCaloTowerTool.h
 * SingleCaloTowerTool.h
 *
 *  Tool building the calorimeter towers for the sliding window algorithm.
 *  Towers are built of cells in eta-phi, summed over all radial layers.
 *  A tower contains all cells within certain eta and phi (tower size: '\b
 * deltaEtaTower', '\b deltaPhiTower').
 *  Distance in r plays no role, however `\b radiusForPosition` needs to be
 *  defined (e.g. to inner radius of the detector) for the cluster position 
 *  calculation. By default the radius is equal to 1.
 *
 *  This tool creates towers from a single cell collection (from one
 * calorimeter).
 *
 *  For more explanation please [see reconstruction documentation](@ref
 * md_reconstruction_doc_reccalorimeter).
 *
 *  @author Anna Zaborowska
 *  @author Jana Faltova
 */

class SingleCaloTowerTool : public GaudiTool, virtual public ITowerTool {
public:
  SingleCaloTowerTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~SingleCaloTowerTool() = default;
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;
  /**  Find number of calorimeter towers.
   *   Number of towers in phi is calculated from full azimuthal angle (2 pi)
   * and the size of tower in phi ('\b deltaPhiTower').
   *   Number of towers in eta is calculated from maximum detector eta ('\b
   * etaMax`) and the size of tower in eta ('\b deltaEtaTower').
   *   @return Struct containing number of towers in eta and phi.
   */
  virtual tower towersNumber() final;
  /**  Build calorimeter towers.
   *   Tower is segmented in eta and phi, with the energy from all layers 
   *   (no segmentation).
   *   @param[out] aTowers Calorimeter towers.
   *   @return Size of the cell collection.
   */
  virtual uint buildTowers(std::vector<std::vector<float>>& aTowers) final;
  /**  Get the radius (in mm) for the position calculation.
   *   Reconstructed cluster has eta and phi position, without the radial
   * coordinate. The cluster in EDM contains
   * Cartesian position, so the radial position (e.g. the inner radius of the
   * calorimeter) needs to be specified. By default it is equal to 1.
   *   @return Radius
   */
  virtual float radiusForPosition() const final;
  /**  Get the tower IDs in eta.
   *   @param[in] aEta Position of the calorimeter cell in eta
   *   @return ID (eta) of a tower
   */
  virtual uint idEta(float aEta) const final;
  /**  Get the tower IDs in phi.
   *   Tower IDs are shifted so they start at 0 (middle of cell with ID=0 is
   * phi=0, phi is defined form -pi to pi). No
   * segmentation offset is taken into account.
   *   @param[in] aPhi Position of the calorimeter cell in phi
   *   @return ID (phi) of a tower
   */
  virtual uint idPhi(float aPhi) const final;
  /**  Get the eta position of the centre of the tower.
   *   Tower IDs are shifted so they start at 0 (middle of cell with ID=0 is
   * eta=0). No segmentation offset is taken into account.
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

private:
  /// Handle for calo cells (input collection)
  DataHandle<fcc::CaloHitCollection> m_cells{"calo/cells", Gaudi::DataHandle::Reader, this};
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Name of the detector readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "", "Name of the detector readout"};
  /// PhiEta segmentation (owned by DD4hep)
  DD4hep::DDSegmentation::GridPhiEta* m_segmentation;
  /// Radius used to calculate cluster position from eta and phi (in mm)
  Gaudi::Property<double> m_radius{this, "radiusForPosition", 1.0,
                                   "Radius used to calculate cluster position from eta and phi (in mm)"};
  /// Maximum eta of the detector
  float m_etaMax;
  /// Maximum phi of the detector
  float m_phiMax;

  /// Size of the tower in eta
  Gaudi::Property<float> m_deltaEtaTower{this, "deltaEtaTower", 0.01, "Size of the tower in eta"};
  /// Size of the tower in phi
  Gaudi::Property<float> m_deltaPhiTower{this, "deltaPhiTower", 0.01, "Size of the tower in phi"};
  // Maximum layer (4 = first layer of 2cm length)
  Gaudi::Property<float> m_maximumLayer{this, "maximumLayer", 4, "Maximum cell layer"};
  /// number of towers in eta (calculated from m_deltaEtaTower and m_etaMax)
  uint m_nEtaTower;
  /// Number of towers in phi (calculated from m_deltaPhiTower)
  uint m_nPhiTower;

};

#endif /* RECCALORIMETER_SINGLECALOTOWERTOOL_H */
