#ifndef RECCALORIMETER_SPLITCLUSTERS_H
#define RECCALORIMETER_SPLITCLUSTERS_H

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ICalorimeterTool.h"
#include "RecInterface/INoiseConstTool.h"
#include "RecInterface/ICellPositionsTool.h"
#include "RecInterface/ICaloReadNeighboursMap.h"
//#include "SimG4Interface/ISimG4ParticleSmearTool.h"

// DD4hep
#include "DDSegmentation/Segmentation.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include "datamodel/CaloHit.h"
#include "datamodel/CaloCluster.h"
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloClusterCollection.h"
#include "datamodel/MCParticleCollection.h"

class IGeoSvc;
namespace DD4hep {
namespace DDSegmentation {
class Segmentation;
}
}

class TH2F;
class TH1F;
class TLorentzVector;

/** @class SplitClusters
 *
 *  Tools called:
 *    - ReadPileupParameters
 *
 *  @author Coralie Neubueser
 *  @date   2018-03
 *
 */

class SplitClusters : public GaudiAlgorithm {

public:
  SplitClusters(const std::string& name, ISvcLocator* svcLoc);

  StatusCode initialize();

  /** Search for neighbours and add them to preClusterCollection
   * The 
   *   @param[in] aCellId, the cell ID for which to find the neighbours.
   *   @param[in] aClusterID, the current cluster ID.
   *   @param[in] aCells, map of all cells.
   *   @param[in] aCellTypes, vector of pairs of cell ids and type.
   *   @param[in] aClusterOfCell, map of cellID to clusterID.
   *   @param[in] aPreClusterCollection, map that is filled with clusterID pointing to the associated cells, in a pair of cellID and cellType.
   *   @param[in] aAllowClusterMerge, bool to allow for clusters to be merged, set to false in case of last iteration in CaloTopoCluster::buildingProtoCluster.
   *   return vector of pairs with cellID and energy of found neighbours.
   */
  std::vector<std::pair<uint64_t, uint>> 
    searchForNeighbours(const uint64_t aCellId,
			uint aClusterID, 
			const std::map<uint64_t, int> aCellType,
			std::map<uint64_t, uint>& aClusterOfCell,
                        std::map<uint64_t, TLorentzVector> aCellPosition,
			std::map<uint, TLorentzVector>& aClusterPositions
			);

  StatusCode execute();

  StatusCode finalize();

private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;

  /// Handle for calo clusters (input collection)
  DataHandle<fcc::CaloClusterCollection> m_clusters{"calo/clusters", Gaudi::DataHandle::Reader, this};
  /// Handle for calo clusters (output collection)
  DataHandle<fcc::CaloClusterCollection> m_newClusters{"calo/calibClusters", Gaudi::DataHandle::Writer, this};
  // Handle for calo cells (output collection)
  DataHandle<fcc::CaloHitCollection> m_newCells{"calo/calibClusterCells", Gaudi::DataHandle::Writer, this};
  /// Handle for neighbours tool 
  ToolHandle<ICaloReadNeighboursMap> m_neighboursTool{"TopoCaloNeighbours", this};

  /// Handle for tool to get positions in ECal Barrel
  ToolHandle<ICellPositionsTool> m_cellPositionsECalBarrelTool{"CellPositionsECalBarrelTool", this};
  /// Handle for tool to get positions in HCal Barrel
  ToolHandle<ICellPositionsTool> m_cellPositionsHCalBarrelNoSegTool{"CellPositionsHCalBarrelNoSegTool", this};
  /// Handle for tool to get positions in HCal Barrel 
  ToolHandle<ICellPositionsTool> m_cellPositionsHCalBarrelTool{"CellPositionsHCalBarrelTool", this};

  /// General decoder to encode the calorimeter sub-system to determine which positions tool to use
  dd4hep::DDSegmentation::BitFieldCoder* m_decoder = new dd4hep::DDSegmentation::BitFieldCoder("system:4");

  bool m_noSegmentationHCalUsed = false; 

  // Energy threshold to find local maxima
  Gaudi::Property<double> m_threshold{this, "threshold", 0.5, "Threshold for local maxima."};
  
  dd4hep::DDSegmentation::BitFieldCoder* m_decoderECal;
  dd4hep::DDSegmentation::BitFieldCoder* m_decoderHCal;
                                                                                      
  /// no segmentation used in HCal
  Gaudi::Property<bool> m_noSegmentationHCal{this, "noSegmentationHCal", true, "HCal readout w/o eta-phi segementation?"};
  Gaudi::Property<int> m_lastECalLayer{this, "lastECalLayer", 7, "Layer id of last ECal layer"};
  Gaudi::Property<int> m_firstHCalLayer{this, "firstHCalLayer", 0, "Layer id of first HCal layer"};

  Gaudi::Property<uint> m_systemIdECal{this, "systemECal", 5, "System id of ECal"};
  Gaudi::Property<uint> m_systemIdHCal{this, "systemHCal", 8, "System id of HCal"};
  Gaudi::Property<std::string> m_readoutECal{this, "readoutECal", "Readout of ECal"};
  Gaudi::Property<std::string> m_readoutHCal{this, "readoutHCal", "Readout of HCal"};

};

#endif /* RECCALORIMETER_SPLITCLUSTERS_H */
