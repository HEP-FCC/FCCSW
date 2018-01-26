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
			    std::vector<std::pair<uint64_t, double> >& seeds,
			    INoiseConstTool& aNoiseTool);
  /** Build proto-clusters
   */
  virtual void buildingProtoCluster(int nSigma,
				    const std::unordered_map<uint64_t, std::vector<uint64_t>> neighboursMap,
				    std::vector<std::pair<uint64_t, double> >& seeds, 
				    std::map<uint64_t, double>& allCells,
				    std::map<uint, std::vector<uint64_t> >& preClusterCollection,
				    INoiseConstTool& aNoiseTool);
 
  /** Search for neighbours and add them to lists
   */
  std::vector<uint64_t> searchForNeighbours(
					    const uint64_t id, uint& clusterNum, const std::unordered_map<uint64_t, std::vector<uint64_t>> neighboursMap,
					    int nSigma, const std::map<uint64_t, double>& allCells, std::map<uint64_t, uint>& clusterOfCell,
					    std::map<uint, std::vector<uint64_t>>& preClusterCollection, INoiseConstTool& aNoiseTool);
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
  /// Handle for tool to get positions
  ToolHandle<ICellPositionsTool> m_cellPositionsTool{"CellPositionsBarrelTool", this};
  /// PhiEta segmentation of the detector (owned by DD4hep)
  DD4hep::DDSegmentation::GridPhiEta* m_Segmentation;
  /// Name of the bit-fields searching for neighbours
  Gaudi::Property<std::vector<std::string>> m_fieldNames{
    this, "fieldsForNeighbours", {"layer", "phi", "eta"}, "dimensions to look for neighbours"};
 
  int m_range;
  /// Seed threshold
  int m_seedSigma = 4;
  int m_neighbourSigma = 2;
  Gaudi::Property<int> m_lastLayer{this, "lastLayer", 7, "last layer"};
  std::vector<std::pair<int, int>> m_fieldExtremes;
  DD4hep::DDSegmentation::BitField64* m_decoder;
  /// all active Cells
  std::map<uint64_t, double> m_allCells;
  // Map of cellIDs to vector of neighbouring cell ids
  std::unordered_map<uint64_t, std::vector<uint64_t>> m_neighboursMap;
  /// First list of CaloCells above seeding threshold
  std::vector<std::pair<uint64_t, double> > m_firstSeeds;
};
#endif /* RECCALORIMETER_CALOTOPOCLUSTER_H */
