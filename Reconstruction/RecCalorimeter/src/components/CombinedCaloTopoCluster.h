#ifndef RECCALORIMETER_COMBINEDCALOTOPOCLUSTER_H
#define RECCALORIMETER_COMBINEDCALOTOPOCLUSTER_H

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "DetSegmentation/GridPhiEta.h"
#include "FWCore/DataHandle.h"
#include "RecInterface/ICalorimeterTool.h"
#include "RecInterface/ICellPositionsTool.h"
#include "RecInterface/IReadNoiseFileTool.h"

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

/** @class CombinedCaloTopoClusterAlgorithm Reconstruction/RecCalorimeter/src/components/CombinedCaloTopoCluster.h
 * CombinedCaloTopoCluster.h
 *
 *  Algorithm building the topological clusters for the energy reconstruction and as input for ATLAS PFA.
 *
 *  @author Coralie Neubueser
 */

class CombinedCaloTopoCluster : public GaudiAlgorithm {
public:
  CombinedCaloTopoCluster(const std::string& name, ISvcLocator* svcLoc);

  StatusCode initialize();
  /**  Find cells with a signal to noise ratio > 6 for ECal and > 4 for HCal, following ATLAS note
   * ATL-LARG-PUB-2008-002.
   *   For simulation without electronic and pile-up noise, the average noise levels are taken as reference for seeding
   * (1.5 and 3.5MeV/cell for E and HCAL, electronic noise only), (2.5 and 100MeV/cell for E and HCAL, added pile-up).
   *   @return list of seed cells to build proto-clusters.
   */
  virtual void findingSeedsWNoise(const fcc::CaloHitCollection* cells,
                                  double threshold,
                                  std::vector<fcc::CaloHit>& seeds,
                                  std::map<uint64_t, fcc::CaloHit>& allCells,
                                  IReadNoiseFileTool& aNoiseTool);
  virtual void findingSeeds(const fcc::CaloHitCollection* cells,
                            double threshold,
                            std::vector<fcc::CaloHit>& seeds,
                            std::map<uint64_t, fcc::CaloHit>& allCells);
  /** Build proto-clusters
  */
  virtual void buildingProtoClusterWNoise(double neighbourThr,
                                          const std::unordered_map<uint64_t, std::vector<uint64_t>> neighboursMap,
                                          std::vector<fcc::CaloHit>& seeds, std::map<uint64_t, fcc::CaloHit>& allCells,
                                          std::map<uint, std::vector<fcc::CaloHit>>& preClusterCollection,
                                          IReadNoiseFileTool& aNoiseTool);
  virtual void buildingProtoCluster(double neighbourThr,
                                    const std::unordered_map<uint64_t, std::vector<uint64_t>> neighboursMap,
                                    std::vector<fcc::CaloHit>& seeds, std::map<uint64_t, fcc::CaloHit>& allCells,
                                    std::map<uint, std::vector<fcc::CaloHit>>& preClusterCollection);
  /** Search for neighbours and add them to lists
    */
  std::vector<uint64_t> searchForNeighboursWNoise(
      const uint64_t id, uint& clusterNum, const std::unordered_map<uint64_t, std::vector<uint64_t>> neighboursMap,
      double threshold, std::map<uint64_t, fcc::CaloHit>& allCells, std::map<uint64_t, uint>& clusterOfCell,
      std::map<uint, std::vector<fcc::CaloHit>>& preClusterCollection, IReadNoiseFileTool& aNoiseTool);
  std::vector<uint64_t> searchForNeighbours(const uint64_t id, uint& clusterNum,
                                            const std::unordered_map<uint64_t, std::vector<uint64_t>> neighboursMap,
                                            double threshold, std::map<uint64_t, fcc::CaloHit>& allCells,
                                            std::map<uint64_t, uint>& clusterOfCell,
                                            std::map<uint, std::vector<fcc::CaloHit>>& preClusterCollection);

  StatusCode execute();

  StatusCode finalize();

private:
  /// Handle for electromagnetic calorimeter cells (input collection)
  DataHandle<fcc::CaloHitCollection> m_ECalCells{"ECalCells", Gaudi::DataHandle::Reader, this};
  /// Handle for hadronic calorimeter cells (input collection)
  DataHandle<fcc::CaloHitCollection> m_HCalCells{"HCalCells", Gaudi::DataHandle::Reader, this};
  // Pre-cluster collection
  DataHandle<fcc::CaloClusterCollection> m_clusterCollection{"calo/clusters", Gaudi::DataHandle::Writer, this};
  // Pre-cluster cells collection
  DataHandle<fcc::CaloHitCollection> m_clusterCellsCollection{"calo/clusterCells", Gaudi::DataHandle::Writer, this};
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Name of the electromagnetic calorimeter readout
  Gaudi::Property<std::string> m_ECalReadoutName{this, "ECalReadoutName", "name of the ECal readout"};
  /// Name of the hadronic calorimeter readout
  Gaudi::Property<std::string> m_HCalReadoutName{this, "HCalReadoutName", "name of the HCal readout"};
  /// Handle for the ECal cells noise tool
  ToolHandle<IReadNoiseFileTool> m_noiseToolECal{"ReadNoiseFromFileTool", this};
  /// Handle for the HCal cells noise tool
  ToolHandle<IReadNoiseFileTool> m_noiseToolHCal{"ReadNoiseFromFileTool", this};
  /// Noise added to electromagnetic calorimeter
  Gaudi::Property<bool> m_noiseECal{this, "noiseAddedECal", false, "noise has been added, thresholds adjusted"};
  /// Noise added to hadronic calorimeter
  Gaudi::Property<bool> m_noiseHCal{this, "noiseAddedHCal", false, "noise has been added, thresholds adjusted"};
  /// Handle for tool to get positions
  ToolHandle<ICellPositionsTool> m_cellPositionsToolECal{"CellPositionsECalBarrelTool", this};
  /// Handle for tool to get positions
  ToolHandle<ICellPositionsTool> m_cellPositionsToolHCal{"CellPositionsHCalBarrelTool", this};

  /// PhiEta segmentation of the electromagnetic detector (owned by DD4hep)
  DD4hep::DDSegmentation::GridPhiEta* m_ECalSegmentation;
  /// PhiEta segmentation of the hadronic detector (owned by DD4hep)
  DD4hep::DDSegmentation::GridPhiEta* m_HCalSegmentation;
  // Range for neighbours to be found
  int m_range;

  /// all active Cells
  std::map<uint64_t, fcc::CaloHit> m_allCellsECal;
  std::map<uint64_t, fcc::CaloHit> m_allCellsHCal;

  /// First list of CaloCells above seeding threshold
  std::vector<fcc::CaloHit> firstSeedsECal;
  std::vector<fcc::CaloHit> firstSeedsHCal;

  /// THRESHOLD FOR RECO WITHOUT NOISE
  /// Seed threshold ECal
  Gaudi::Property<double> m_seedThr_ECal{this, "seedThresholdECal", (7.5 / 4.), "seed threshold estimate [MeV]"};
  /// Seed threshold HCal
  Gaudi::Property<double> m_seedThr_HCal{this, "seedThresholdHCal", (11.5 / 4.), "seed threshold estimate [MeV]"};
  /// Seed threshold ECal
  Gaudi::Property<double> m_neighbourThr_ECal{this, "neighbourThresholdECal", (3. / 2.),
                                              "neighbour threshold estimate [MeV]"};
  /// Seed threshold HCal
  Gaudi::Property<double> m_neighbourThr_HCal{this, "neighbourThresholdHCal", (3.5 / 2.),
                                              "neighbour threshold estimate [MeV]"};

  /// Name of the bit-fields searching for neighbours in ECal
  Gaudi::Property<std::vector<std::string>> m_fieldNamesECal{
      this, "fieldsForNeighboursECal", {"layer", "phi", "eta"}, "dimensions to look for neighbours in ECal"};
  /// Name of the bit-fields searching for neighbours in HCal
  Gaudi::Property<std::vector<std::string>> m_fieldNamesHCal{
      this, "fieldsForNeighboursHCal", {"layer", "phi", "row"}, "dimensions to look for neighbours in HCal"};
  /// Seed threshold ECal
  Gaudi::Property<int> m_lastECalLayer{this, "lastECalLayer", 7, "last layer in the ECal"};

  std::vector<std::pair<int, int>> m_fieldExtremesECal;
  std::vector<std::pair<int, int>> m_fieldExtremesHCal;

  std::shared_ptr<DD4hep::DDSegmentation::BitField64> m_decoderECal;
  std::shared_ptr<DD4hep::DDSegmentation::BitField64> m_decoderHCal;
};

#endif /* RECCALORIMETER_COMBINEDCALOTOPOCLUSTER_H */
