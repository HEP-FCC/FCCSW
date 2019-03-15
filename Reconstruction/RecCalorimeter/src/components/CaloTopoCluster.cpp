#include "CaloTopoCluster.h"
#include "NoiseCaloCellsFromFileTool.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

// datamodel
#include "datamodel/CaloCluster.h"
#include "datamodel/CaloClusterCollection.h"
#include "datamodel/CaloHit.h"
#include "datamodel/CaloHitCollection.h"

// DD4hep
#include "DD4hep/Detector.h"
#include "DD4hep/Readout.h"

#include <algorithm>
#include <map>
#include <numeric>
#include <unordered_map>
#include <vector>

DECLARE_ALGORITHM_FACTORY(CaloTopoCluster)

CaloTopoCluster::CaloTopoCluster(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("TopoClusterInput", m_inputTool, "Handle for input map of cells");
  declareProperty("noiseTool", m_noiseTool, "Handle for the cells noise tool");
  declareProperty("neigboursTool", m_neighboursTool, "Handle for tool to retrieve cell neighbours");
  declareProperty("positionsECalBarrelTool", m_cellPositionsECalBarrelTool,
                  "Handle for tool to retrieve cell positions in ECal Barrel");
  declareProperty("positionsHCalBarrelTool", m_cellPositionsHCalBarrelTool,
                  "Handle for tool to retrieve cell positions in HCal Barrel");
  declareProperty("positionsHCalBarrelNoSegTool", m_cellPositionsHCalBarrelNoSegTool,
                  "Handle for tool to retrieve cell positions in HCal Barrel without DD4hep segmentation");
  declareProperty("positionsHCalExtBarrelTool", m_cellPositionsHCalExtBarrelTool,
                  "Handle for tool to retrieve cell positions in HCal ext Barrel");
  declareProperty("positionsEMECTool", m_cellPositionsEMECTool, "Handle for tool to retrieve cell positions in EMEC");
  declareProperty("positionsHECTool", m_cellPositionsHECTool, "Handle for tool to retrieve cell positions in HEC");
  declareProperty("positionsEMFwdTool", m_cellPositionsEMFwdTool, "Handle for tool to retrieve cell positions EM Fwd");
  declareProperty("positionsHFwdTool", m_cellPositionsHFwdTool, "Handle for tool to retrieve cell positions Had Fwd");
  declareProperty("clusters", m_clusterCollection, "Handle for calo clusters (output collection)");
  declareProperty("clusterCells", m_clusterCellsCollection, "Handle for clusters (output collection)");
}
StatusCode CaloTopoCluster::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) return StatusCode::FAILURE;
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_inputTool.retrieve()) {
    error() << "Unable to retrieve the topo cluster input tool!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_neighboursTool.retrieve()) {
    error() << "Unable to retrieve the cells neighbours tool!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_noiseTool.retrieve()) {
    error() << "Unable to retrieve the cells noise tool!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  // Check if cell position ECal Barrel tool available
  if (!m_cellPositionsECalBarrelTool.retrieve()) {
    error() << "Unable to retrieve ECal Barrel cell positions tool!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  // Check if cell position HCal Barrel tool available
  if (!m_cellPositionsHCalBarrelTool.retrieve()) {
    error() << "Unable to retrieve HCal Barrel cell positions tool!!!" << endmsg;
    if (!m_cellPositionsHCalBarrelNoSegTool.retrieve()) {
      error() << "Also unable to retrieve HCal Barrel no segmentation cell positions tool!!!" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode CaloTopoCluster::execute() {
  
  std::map<uint64_t, double> allCells;
  std::vector<std::pair<uint64_t, double>> firstSeeds;
  
  // get input cell map from input tool
  StatusCode sc_prepareCellMap = m_inputTool->cellIdMap(allCells);
  if (sc_prepareCellMap.isFailure()) {
    error() << "Unable to create cell map!" << endmsg;
    return StatusCode::FAILURE;
  }
  debug() << "Active Cells          :    " << allCells.size() << endmsg;
 
  // Create output collections
  auto edmClusters = m_clusterCollection.createAndPut();
  fcc::CaloHitCollection* edmClusterCells = new fcc::CaloHitCollection();

  // Finds seeds
  CaloTopoCluster::findingSeeds(allCells, m_seedSigma, firstSeeds);
  debug() << "Number of seeds found :    " << firstSeeds.size() << endmsg;

  // decending order of seeds
  std::sort(firstSeeds.begin(), firstSeeds.end(),
            [](const std::pair<uint64_t, double>& lhs, const std::pair<uint64_t, double>& rhs) {
              return lhs.second < rhs.second;
            });

  std::map<uint, std::vector<std::pair<uint64_t, uint>>> preClusterCollection;
  CaloTopoCluster::buildingProtoCluster(m_neighbourSigma, m_lastNeighbourSigma, firstSeeds, allCells,
                                        preClusterCollection);
  // Build Clusters in edm
  debug() << "Building " << preClusterCollection.size() << " cluster." << endmsg;
  double checkTotEnergy = 0.;
  int clusterWithMixedCells = 0;
  for (auto i : preClusterCollection) {
    fcc::CaloCluster cluster;
    auto& clusterCore = cluster.core();
    double posX = 0.;
    double posY = 0.;
    double posZ = 0.;
    double energy = 0.;
    std::map<int,int> system;

    for (auto pair : i.second) {
      dd4hep::DDSegmentation::CellID cID = pair.first;
	//      auto cellId = pair.first;
      // get CaloHit by cellID
      auto newCell = edmClusterCells->create();
      newCell.core().energy = allCells[cID];
      newCell.core().cellId = cID;
      newCell.core().bits = pair.second;
      energy += newCell.core().energy;

      // get cell position by cellID
      // identify calo system
      auto systemId = m_decoder->get(cID, "system");
      system[int(systemId)]++;
      dd4hep::Position posCell;
      if (systemId == 5)  // ECAL BARREL system id
        posCell = m_cellPositionsECalBarrelTool->xyzPosition(cID);
      else if (systemId == 8){  // HCAL BARREL system id
	if (m_noSegmentationHCalUsed)
	  posCell = m_cellPositionsHCalBarrelNoSegTool->xyzPosition(cID);
	else{
	  posCell = m_cellPositionsHCalBarrelTool->xyzPosition(cID);
	}}
      else if (systemId == 9)  // HCAL EXT BARREL system id
        posCell = m_cellPositionsHCalExtBarrelTool->xyzPosition(cID);
      else if (systemId == 6)  // EMEC system id
        posCell = m_cellPositionsEMECTool->xyzPosition(cID);
      else if (systemId == 7)  // HEC system id
        posCell = m_cellPositionsHECTool->xyzPosition(cID);
      else if (systemId == 10)  // EMFWD system id
        posCell = m_cellPositionsEMFwdTool->xyzPosition(cID);
      else if (systemId == 11)  // HFWD system id
        posCell = m_cellPositionsHFwdTool->xyzPosition(cID);
      else
        warning() << "No cell positions tool found for system id " << systemId << ". " << endmsg;

      posX += posCell.X() * newCell.core().energy;
      posY += posCell.Y() * newCell.core().energy;
      posZ += posCell.Z() * newCell.core().energy;
      // edmClusterCells->push_back(newCell);
      cluster.addhits(newCell);
      allCells.erase(cID);
    }
    clusterCore.energy = energy;
    clusterCore.position.x = posX / energy;
    clusterCore.position.y = posY / energy;
    clusterCore.position.z = posZ / energy;
    verbose() << "Cluster energy:     " << clusterCore.energy << endmsg;
    checkTotEnergy += clusterCore.energy;

    edmClusters->push_back(cluster);
    if (system.size() > 1)
      clusterWithMixedCells++;
  }
  m_clusterCellsCollection.put(edmClusterCells);
  debug() << "Number of clusters with cells in E and HCal:        " << clusterWithMixedCells << endmsg;
  debug() << "Total energy of clusters:                                      " << checkTotEnergy << endmsg;
  debug() << "Leftover cells :                                                     " << allCells.size() << endmsg;
  return StatusCode::SUCCESS;
}

void CaloTopoCluster::findingSeeds(const std::map<uint64_t, double>& aCells,
                                   int aNumSigma,
                                   std::vector<std::pair<uint64_t, double>>& aSeeds) {
  for (const auto& cell : aCells) {
    // retrieve the noise const and offset assigned to cell
    double threshold = m_noiseTool->noiseOffset(cell.first) + ( m_noiseTool->noiseRMS(cell.first) * aNumSigma);
    if (msgLevel() <= MSG::VERBOSE){
      verbose() << "noise offset    = " << m_noiseTool->noiseOffset(cell.first) << "GeV " << endmsg;
      verbose() << "noise rms       = " << m_noiseTool->noiseRMS(cell.first) << "GeV " << endmsg;
      verbose() << "seed threshold  = " << threshold << "GeV " << endmsg;
    }
    if (abs(cell.second) > threshold) {
      aSeeds.emplace_back(cell.first, cell.second);
    }
  }
}

StatusCode CaloTopoCluster::buildingProtoCluster(
    int aNumSigma,
    int aLastNumSigma,
    std::vector<std::pair<uint64_t, double>>& aSeeds,
    const std::map<uint64_t, double>& aCells,
    std::map<uint, std::vector< std::pair<uint64_t, uint>>>& aPreClusterCollection) {
  // Map of cellIds to clusterIds
  std::map<uint64_t, uint> clusterOfCell;

  // Loop over every seed in Calo to create first cluster
  uint iSeeds = 0;
  verbose() << "seeds to loop over : " << aSeeds.size() << endmsg;
  for (auto& itSeed : aSeeds) {
    iSeeds++;
    verbose() << "Seed num: " << iSeeds << endmsg;
    auto seedId = itSeed.first;
    auto cellInCluster = clusterOfCell.find(seedId);
    if (cellInCluster != clusterOfCell.end()) {
      verbose() << "Seed is already assigned to another cluster!" << endmsg;
      continue;
    } else {
      // new cluster starts with seed
      // set cell Bits to 1 for seed cell
      aPreClusterCollection[iSeeds].push_back(std::make_pair(seedId, 1));
      uint clusterId = iSeeds;
      clusterOfCell[seedId] = clusterId;

      std::vector<std::vector<std::pair<uint64_t, uint>>> vecNextNeighbours(100000);
      vecNextNeighbours[0] = CaloTopoCluster::searchForNeighbours(seedId, clusterId, aNumSigma, aCells, clusterOfCell,
                                                     aPreClusterCollection, true);
      // first loop over seeds neighbours
      verbose() << "Found " << vecNextNeighbours[0].size() << " neighbours.." << endmsg;
      int it = 0;
      while (vecNextNeighbours[it].size() > 0) {
        it++;
        for (auto& id : vecNextNeighbours[it - 1]) {
	  if (id.first == 0){
	    error() << "Building of cluster is stopped due to missing id in neighbours map." << endmsg;
	    return StatusCode::FAILURE;
	  }
          verbose() << "Next neighbours assigned to clusterId : " << clusterId << endmsg;
          vecNextNeighbours[it] = CaloTopoCluster::searchForNeighbours(id.first, clusterId, aNumSigma, aCells, clusterOfCell,
								       aPreClusterCollection, true);
        }
        verbose() << "Found " << vecNextNeighbours[it].size() << " more neighbours.." << endmsg;
      }
      // last try with different condition on neighbours
      if (vecNextNeighbours[it].size() == 0) {
	auto clusteredCells = aPreClusterCollection[clusterId];
	// loop over all clustered cells
	for (auto& id : clusteredCells) {
	  if (id.second <= 2){
	    verbose() << "Add neighbours of " << id.first << " in last round with thr = " << aLastNumSigma << " x sigma." << endmsg;
	    auto lastNeighours = CaloTopoCluster::searchForNeighbours(id.first, clusterId, aLastNumSigma, aCells, clusterOfCell,
								      aPreClusterCollection, false);
	  }
	}
      }
    }
  }
  return StatusCode::SUCCESS;
}

std::vector<std::pair<uint64_t, uint> >
CaloTopoCluster::searchForNeighbours(const uint64_t aCellId,
                                     uint& aClusterID,
                                     int aNumSigma,
                                     const std::map<uint64_t, double>& aCells,
                                     std::map<uint64_t, uint>& aClusterOfCell,
                                     std::map<uint, std::vector<std::pair<uint64_t, uint>>>& aPreClusterCollection,
				     bool aAllowClusterMerge) {
  // Fill vector to be returned, next cell ids and cluster id for which neighbours are found
  std::vector<std::pair<uint64_t, uint>> addedNeighbourIds;
  // Retrieve cellIds of neighbours
  auto neighboursVec = m_neighboursTool->neighbours(aCellId);
  if (neighboursVec.size() == 0) {
    error() << "No neighbours for cellID found! " << endmsg;
    error() << "to cellID :  " << aCellId << endmsg;
    error() << "in system:   " << m_decoder->get(aCellId, "system") << endmsg;
    addedNeighbourIds.resize(0);
    addedNeighbourIds.push_back(std::make_pair(0, 0));
  } else {

    verbose() << "For cluster: " << aClusterID << endmsg;
    // loop over neighbours
    for (auto& itr : neighboursVec) {
      auto neighbourID = itr;
      // Find the neighbour in the Calo cells list
      auto itAllCells = aCells.find(neighbourID);
      auto itAllUsedCells = aClusterOfCell.find(neighbourID);

      // If cell is hit.. and is not assigned to a cluster
      if (itAllCells != aCells.end() && itAllUsedCells == aClusterOfCell.end()) {
        verbose() << "Found neighbour with CellID: " << neighbourID << endmsg;
        auto neighbouringCellEnergy = itAllCells->second;
        bool addNeighbour = false;
	int cellType = 2;
	// retrieve the cell noise level [GeV]
        double thr = m_noiseTool->noiseOffset(neighbourID) + (aNumSigma * m_noiseTool->noiseRMS(neighbourID));
	if (abs(neighbouringCellEnergy) > thr)
	  addNeighbour = true;
	else
	  addNeighbour = false;
	// give cell type according to threshold
	if (aNumSigma == m_lastNeighbourSigma){
	  cellType = 3;
	}
	// if threshold is 0, collect the cell independent on its energy
	if (aNumSigma == 0){
	  addNeighbour = true;
	}
        // if neighbour is validated
        if (addNeighbour) {
          // retrieve the cell
          // add neighbour to cells for cluster
          aPreClusterCollection[aClusterID].push_back(std::make_pair(neighbourID, cellType));
          aClusterOfCell[neighbourID] = aClusterID;
          addedNeighbourIds.push_back(std::make_pair(neighbourID, aClusterID));
        }
      }
      // If cell is hit.. but is assigned to another cluster
      else if (itAllUsedCells != aClusterOfCell.end() && itAllUsedCells->second != aClusterID && aAllowClusterMerge) {
        uint clusterIDToMerge = itAllUsedCells->second;
	if (msgLevel() <= MSG::VERBOSE){
	  verbose() << "This neighbour was found in cluster " << clusterIDToMerge << ", cluster " << aClusterID
		    << " will be merged!" << endmsg;
	  verbose() << "Assigning all cells ( " << aPreClusterCollection.find(aClusterID)->second.size() << " ) to Cluster "
		    << clusterIDToMerge << " with ( " << aPreClusterCollection.find(clusterIDToMerge)->second.size()
		    << " ). " << endmsg;
	}
        // Fill all cells into cluster, and assigned cells to new cluster
        aClusterOfCell[neighbourID] = clusterIDToMerge;
        for (auto& i : aPreClusterCollection.find(aClusterID)->second) {
          aClusterOfCell[i.first] = clusterIDToMerge;
          bool found = false;
          // make sure that already assigned cells are not added
          for (auto& j : aPreClusterCollection[clusterIDToMerge]) {
            if (j.first == i.first) found = true;
          }
          if (!found) {
            aPreClusterCollection[clusterIDToMerge].push_back(std::make_pair(i.first, i.second));
          }
        }
        aPreClusterCollection.erase(aClusterID);
        // changed clusterId -> if more neighbours are found, correct assignment
        verbose() << "Cluster Id changed to " << clusterIDToMerge << endmsg;
        aClusterID = clusterIDToMerge;
        // found neighbour for next search
        addedNeighbourIds.push_back(std::make_pair(neighbourID, aClusterID));
        // end loop to ensure correct cluster assignment
        break;
      }
    }
  }
  return std::move(addedNeighbourIds);
}

StatusCode CaloTopoCluster::finalize() { return GaudiAlgorithm::finalize(); }
