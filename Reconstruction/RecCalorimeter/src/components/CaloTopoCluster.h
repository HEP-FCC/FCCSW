#ifndef RECCALORIMETER_CALOTOPOCLUSTER_H
#define RECCALORIMETER_CALOTOPOCLUSTER_H

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "DetSegmentation/GridPhiEta.h"
#include "FWCore/DataHandle.h"
#include "RecInterface/ICalorimeterTool.h"
#include "RecInterface/ICellPositionsTool.h"
#include "RecInterface/INoiseConstTool.h"
#include "RecInterface/ITopoClusterInputTool.h"
#include "RecInterface/ICaloReadNeighboursMap.h"

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
  virtual void findingSeeds(const std::map<uint64_t, double>& allCells,
			    int nSigma,
			    std::vector<std::pair<uint64_t, double> >& seeds);
  /** Build proto-clusters
   */
  virtual void buildingProtoCluster(int nSigma,
				    std::vector<std::pair<uint64_t, double> >& seeds, 
				    std::map<uint64_t, double>& allCells,
				    std::map<uint, std::vector<std::pair<uint64_t, uint> >>& preClusterCollection);
 
  /** Search for neighbours and add them to lists
   */
  std::vector<uint64_t> searchForNeighbours(
					    const uint64_t id, uint& clusterNum, int nSigma, const std::map<uint64_t, double>& allCells, std::map<uint64_t, uint>& clusterOfCell,
					    std::map<uint, std::vector<std::pair<uint64_t, uint> >>& preClusterCollection);
  StatusCode execute();

  StatusCode finalize();

 private:
  // Cluster collection
  DataHandle<fcc::CaloClusterCollection> m_clusterCollection{"calo/clusters", Gaudi::DataHandle::Writer, this};
  // Cluster cells in collection
  DataHandle<fcc::CaloHitCollection> m_clusterCellsCollection{"calo/clusterCells", Gaudi::DataHandle::Writer, this};
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Name of the calorimeter readout
  Gaudi::Property<std::string> m_ReadoutName{this, "ReadoutName", "name of the readout"};
  /// Handle for the input tool
  ToolHandle<ITopoClusterInputTool> m_inputTool{"TopoClusterInput", this};
  /// Handle for the cells noise tool
  ToolHandle<INoiseConstTool> m_noiseTool{"ReadNoiseFromFileTool", this};
  /// Handle for neighbours tool
  ToolHandle<ICaloReadNeighboursMap> m_neighboursTool{"TopoCaloNeighbours", this};
  /// Handle for tool to get positions
  ToolHandle<ICellPositionsTool> m_cellPositionsTool{"CellPositionsBarrelTool", this};
  /// PhiEta segmentation of the detector (owned by DD4hep)
  DD4hep::DDSegmentation::GridPhiEta* m_Segmentation;

  int m_range;
  /// Seed threshold
  int m_seedSigma = 4;
  int m_neighbourSigma = 2;
  DD4hep::DDSegmentation::BitField64* m_decoder;
  /// all active Cells
  std::map<uint64_t, double> m_allCells;
  /// First list of CaloCells above seeding threshold
  std::vector<std::pair<uint64_t, double> > m_firstSeeds;
};
#endif /* RECCALORIMETER_CALOTOPOCLUSTER_H */
