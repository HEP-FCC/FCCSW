#ifndef RECCALORIMETER_CREATECALOCLUSTERSSLIDINGWINDOW_H
#define RECCALORIMETER_CREATECALOCLUSTERSSLIDINGWINDOW_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ITowerTool.h"

// datamodel
namespace fcc {
class CaloClusterCollection;
}

// Cluster
struct cluster {
  float transEnergy;
  float eta;
  float phi;
};

/** @class CreateCaloClustersSlidingWindow
 *
 *  Algorithm for creating calorimeter clusters from cells.
 *
 *  Reconstruction is performed for the calorimeter with readout '\b readoutName' and volume ID retrieved using '\b
 *fieldNames' and '\b fieldValues'.
 *
 *  Sliding window algorithm:
 *  1. Create calorimeter towers.
 *     A tower contains all cells within certain eta and phi (tower size: '\b deltaEtaTower', '\b deltaPhiTower').
 *     Distance in r plays no role in this algorithm.
 *     Currently there is no support of cell splitting, so each cell should be completely inside the tower
 *     and that can be achieved using `GridEtaPhi` segmentation.
 *  2. Find local maxima.
 *     Local maxima are found using the sliding window of a fixed size in phi x eta ('\b nEtaWindow' '\b nPhiWindow' in
 *units of tower size).
 *     If a local max is found and its energy is above threshold ('\b energyThreshold'), it is added to the preclusters
 *list.
 *     Each precluster contains the barycentre position and the transverse energy.
 *     Position is recalculated using the window size in eta x phi ('\b nEtaPosition', '\b nPhiPosition')
 *     that may be smaller than the sliding window to reduce the noise influence. Both windows are centred at the same
 *tower. The energy of the precluster is the energy calculated using the sliding window.
 *  3. Remove duplicates.
 *     If two pre-clusters are found next to each other (within window '\b nEtaDuplicates', '\b nPhiDuplicates'), the
 *pre-cluster with lower energy is removed.
 *     Currently there is no support on energy sharing between clusters, so if duplicate window is smaller than
 *sliding window, some towers may be taken twice (instead of the weighted energy).
 *  4. Build clusters.
 *     Clusters are created using the window of a fixed size in phi x eta ('\b nEtaFinal' '\b nPhiFinal' in units of
 *tower size) around the barycentre position.
 *     Position is calculated from the barycentre position and the radius of the detector.
 *     Radius may be defined by user ('\b radiusForPosition') or (if not defined) taken from det::utils::tubeDimensions.
 *     The second approach may be used for sensitive cylindrical geometries.
 *     For each cluster the cell collection is searched and all those inside the cluster are attached.
 *
 *  Note: Sliding window performs well for electrons/gamma reconstruction. Topological clusters should be better for
 *jets.
 *
 *  @author Jana Faltova
 *  @author Anna Zaborowska
 */

class CreateCaloClustersSlidingWindow : public GaudiAlgorithm {
public:
  CreateCaloClustersSlidingWindow(const std::string& name, ISvcLocator* svcLoc);
  /**  Initialize.
   *   @return status code
   */
  StatusCode initialize();
  /**  Execute.
   *   Perform the sliding window algorithm and build clusters.
   *   @return status code
   */
  StatusCode execute();
  /**  Finalize.
   *   @return status code
   */
  StatusCode finalize();

private:
  /**  Correct way to access the neighbour of the phi tower, taking into account the full coverage in phi.
   *   Full coverage means that first tower in phi, with ID = 0 is a direct neighbour
   *   of the last tower in phi with ID = m_nPhiTower - 1).
   *   @param[in] aIPhi requested ID of a phi tower, may be < 0 or >= m_nPhiTower
   *   @return  ID of a tower - shifted and corrected (in [0, m_nPhiTower) range)
   */
  unsigned int phiNeighbour(int aIPhi) const;
  /// Handle for calo clusters (output collection)
  DataHandle<fcc::CaloClusterCollection> m_clusters{"calo/clusters", Gaudi::DataHandle::Writer, this};
  /// Handle for the tower building tool
  ToolHandle<ITowerTool> m_towerTool;
  // calorimeter towers
  std::vector<std::vector<float>> m_towers;
  /// Vector of pre-clusters
  std::vector<cluster> m_preClusters;
  /// number of towers in eta (calculated from m_deltaEtaTower and the eta size of the first layer)
  int m_nEtaTower;
  /// Number of towers in phi (calculated from m_deltaPhiTower)
  int m_nPhiTower;
  /// Size of the window in eta for pre-clusters (in units of tower size)
  Gaudi::Property<int> m_nEtaWindow{this, "nEtaWindow", 5};
  /// Size of the window in phi for pre-clusters (in units of tower size)
  Gaudi::Property<int> m_nPhiWindow{this, "nPhiWindow", 15};
  /// Size of the window in eta for cluster position calculation (in units of tower size)
  Gaudi::Property<int> m_nEtaPosition{this, "nEtaPosition", 3};
  /// Size of the window in phi for cluster position calculation (in units of tower size)
  Gaudi::Property<int> m_nPhiPosition{this, "nPhiPosition", 3};
  /// Size of the window in eta for the overlap removal (in units of tower size)
  Gaudi::Property<int> m_nEtaDuplicates{this, "nEtaDuplicates", 2};
  /// Size of the window in phi for the overlap removal (in units of tower size)
  Gaudi::Property<int> m_nPhiDuplicates{this, "nPhiDuplicates", 2};
  /// Energy threshold for cluster finding
  Gaudi::Property<float> m_energyThreshold{this, "energyThreshold", 10};
  /// Size of the window in eta for the final cluster building (in units of tower size)
  Gaudi::Property<int> m_nEtaFinal{this, "nEtaFinal", 5};
  /// Size of the window in phi for the final cluster building (in units of tower size)
  Gaudi::Property<int> m_nPhiFinal{this, "nPhiFinal", 15};
  /// Fraction of the energy threshold used in the position calculation
  Gaudi::Property<float> m_energyThresholdFraction{this, "energyThresholdFraction", 0.25};
  /// Flag for the application of the correction for energy sharing between clusters
  Gaudi::Property<bool> m_energySharingCorrection{this, "energySharingCorrection", true};
};

#endif /* RECCALORIMETER_CREATECALOCLUSTERSSLIDINGWINDOW_H */
