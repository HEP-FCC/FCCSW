#ifndef RECCALORIMETER_CALOTOPOCLUSTER_H
#define RECCALORIMETER_CALOTOPOCLUSTER_H

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ICaloReadNeighboursMap.h"
#include "RecInterface/ICaloReadCellNoiseMap.h"
#include "RecInterface/ICalorimeterTool.h"
#include "RecInterface/ICellPositionsTool.h"
#include "RecInterface/ITopoClusterInputTool.h"

class IGeoSvc;

// datamodel
namespace fcc {
class CaloHit;
class CaloHitCollection;
class CaloClusterCollection;
}

namespace DD4hep {
namespace DDSegmentation {
class Segmentation;
}
}

/** @class CaloTopoClusterAlgorithm Reconstruction/RecCalorimeter/src/components/CaloTopoCluster.h
 * CombinedCaloTopoCluster.h
 *
 *  Algorithm building the topological clusters for the energy reconstruction and as input for ATLAS PFA.
 *
 *  @author Coralie Neubueser
 */

class CaloTopoCluster : public GaudiAlgorithm {
public:
  CaloTopoCluster(const std::string& name, ISvcLocator* svcLoc);

  StatusCode initialize();
  /**  Find cells with a signal to noise ratio > 6 for ECal and > 4 for HCal, following ATLAS note
   * ATL-LARG-PUB-2008-002.
   *   For simulation without electronic and pile-up noise, the average noise levels are taken as reference for seeding
   * (1.5 and 3.5MeV/cell for E and HCAL, electronic noise only), (2.5 and 100MeV/cell for E and HCAL, added pile-up).
   *   @return list of seed cells to build proto-clusters.
   */
  virtual void
  findingSeeds(const std::map<uint64_t, double>& allCells, int nSigma, std::vector<std::pair<uint64_t, double>>& seeds);
  /** Build proto-clusters
   */
  virtual void buildingProtoCluster(int nSigma,
				    int lastNSigma,
                                    std::vector<std::pair<uint64_t, double>>& seeds,
                                    std::map<uint64_t, double>& allCells,
                                    std::map<uint, std::vector<std::pair<uint64_t, uint>>>& preClusterCollection);

  /** Search for neighbours and add them to lists
   */
  std::vector<std::pair<uint64_t, uint>>
  searchForNeighbours(const uint64_t id, uint& clusterID, int nSigma, const std::map<uint64_t, double>& allCells,
                      std::map<uint64_t, uint>& clusterOfCell,
                      std::map<uint, std::vector<std::pair<uint64_t, uint>>>& preClusterCollection);
  StatusCode execute();

  StatusCode finalize();

private:
  // Cluster collection
  DataHandle<fcc::CaloClusterCollection> m_clusterCollection{"calo/clusters", Gaudi::DataHandle::Writer, this};
  // Cluster cells in collection
  DataHandle<fcc::CaloHitCollection> m_clusterCellsCollection{"calo/clusterCells", Gaudi::DataHandle::Writer, this};
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
   /// Handle for the input tool
  ToolHandle<ITopoClusterInputTool> m_inputTool{"TopoClusterInput", this};
  /// Handle for the cells noise tool
  ToolHandle<ICaloReadCellNoiseMap> m_noiseTool{"TopoCaloNoisyCells", this};
  /// Handle for neighbours tool
  ToolHandle<ICaloReadNeighboursMap> m_neighboursTool{"TopoCaloNeighbours", this};
  /// Handle for tool to get positions in ECal Barrel
  ToolHandle<ICellPositionsTool> m_cellPositionsECalBarrelTool{"CellPositionsECalBarrelTool", this};
  /// Handle for tool to get positions in HCal Barrel and Ext Barrel, no Segmentation
  ToolHandle<ICellPositionsTool> m_cellPositionsHCalBarrelTool{"CellPositionsHCalBarrelNoSegTool", this};
  /// Handle for tool to get positions in HCal Barrel and Ext Barrel, no Segmentation
  ToolHandle<ICellPositionsTool> m_cellPositionsHCalExtBarrelTool{"CellPositionsHCalBarrelNoSegTool", this};
  /// Handle for tool to get positions in Calo Discs
  ToolHandle<ICellPositionsTool> m_cellPositionsEMECTool{"CellPositionsCaloDiscsTool", this};
  /// Handle for tool to get positions in Calo Discs
  ToolHandle<ICellPositionsTool> m_cellPositionsHECTool{"CellPositionsCaloDiscsTool", this};
  /// Handle for tool to get positions in Calo Discs
  ToolHandle<ICellPositionsTool> m_cellPositionsEMFwdTool{"CellPositionsCaloDiscsTool", this};
  /// Handle for tool to get positions in Calo Discs
  ToolHandle<ICellPositionsTool> m_cellPositionsHFwdTool{"CellPositionsCaloDiscsTool", this};
  /// Seed threshold in sigma
  Gaudi::Property<int> m_seedSigma{this, "seedSigma", 4, "number of sigma in noise threshold"};
  /// Neighbour threshold in sigma
  Gaudi::Property<int> m_neighbourSigma{this, "neighbourSigma", 2, "number of sigma in noise threshold"};
  /// Last neighbour threshold in sigma
  Gaudi::Property<int> m_lastNeighbourSigma{this, "lastNeighbourSigma", 0, "number of sigma in noise threshold"};

  dd4hep::DDSegmentation::BitField64* m_decoder = new dd4hep::DDSegmentation::BitField64("system:4");
  /// all active Cells
  std::map<uint64_t, double> m_allCells;
  /// First list of CaloCells above seeding threshold
  std::vector<std::pair<uint64_t, double>> m_firstSeeds;
};
#endif /* RECCALORIMETER_CALOTOPOCLUSTER_H */
