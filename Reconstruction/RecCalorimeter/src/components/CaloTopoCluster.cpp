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
#include "DD4hep/LCDD.h"
#include "DD4hep/Readout.h"

#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <numeric>

DECLARE_ALGORITHM_FACTORY(CaloTopoCluster)

CaloTopoCluster::CaloTopoCluster(const std::string& name, ISvcLocator* svcLoc)
: GaudiAlgorithm(name, svcLoc) {
  declareProperty("TopoClusterInput", m_inputTool, "Handle for input map of cells");
  declareProperty("noiseTool", m_noiseTool, "Handle for the cells noise tool");
  declareProperty("positionsTool", m_cellPositionsTool, "Handle for tool to retrieve cell positions");
  declareProperty("neighboursRange", m_range);
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
  // retrieve PhiEta segmentation
  m_Segmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(
								     m_geoSvc->lcdd()->readout(m_ReadoutName).segmentation().segmentation());
  if (m_Segmentation == nullptr) {
    error() << "There is no phi-eta segmentation in the electromagnetic calorimeter." << endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_noiseTool.retrieve()) {
    error() << "Unable to retrieve the cells noise tool!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  // Check if cell position tool available
  if (!m_cellPositionsTool.retrieve()) {
    error() << "Unable to retrieve the cell positions tool!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  // Take readout bitfield decoder from GeoSvc
  m_decoder = m_geoSvc->lcdd()->readout(m_ReadoutName).segmentation().segmentation()->decoder();
  m_fieldExtremes = det::utils::bitfieldExtremes((*m_decoder), m_fieldNames);
  return StatusCode::SUCCESS;
}

StatusCode CaloTopoCluster::execute() {
  // Clear maps before filling
  m_neighboursMap.clear();
  m_allCells.clear();
  m_firstSeeds.clear();
  
  // Create output collections
  auto edmClusters = m_clusterCollection.createAndPut();
  fcc::CaloHitCollection* edmClusterCells = new fcc::CaloHitCollection();
  
  // get input cell map from input tool
  m_allCells = m_inputTool->cellIdMap();
  info() << "Active Cells          :    " << m_allCells.size() << endmsg;

  // Filling of map for each Cell to its neighbours
  int numN = 0;
  for (const auto& itCells : m_allCells) {
    uint64_t cellID = itCells.second;
    std::vector<uint64_t> NeighboursVec =
      det::utils::neighbours(*m_decoder, m_fieldNames, m_fieldExtremes, cellID, m_range);
    m_neighboursMap.emplace(cellID, NeighboursVec);
    numN = NeighboursVec.size();
  }
  debug() << "Number of entries in neighbor map:           " << m_neighboursMap.size() << endmsg;
  debug() << "Number neighbors per Cell:                   " << numN << endmsg;

  // Finds seeds and fills the list of allCells
  CaloTopoCluster::findingSeeds(m_allCells, m_seedSigma, m_firstSeeds, *m_noiseTool);
  info() << "Number of seeds found :    " << m_firstSeeds.size() << endmsg;

  // decending order of seeds
  std::sort(m_firstSeeds.begin(), m_firstSeeds.end(), [](const std::pair<uint64_t, double>& lhs, const std::pair<uint64_t, double>& rhs){return lhs.second < rhs.second;});

  std::map<uint, std::vector<uint64_t>> preClusterCollection;
      CaloTopoCluster::buildingProtoCluster(m_neighbourSigma, m_neighboursMap, m_firstSeeds,
					    m_allCells, preClusterCollection, *m_noiseTool);
  // Build Clusters in edm
  info() << "Building " << preClusterCollection.size() << " cluster." << endmsg;
  double checkTotEnergy = 0.;
  for (auto i : preClusterCollection) {
    fcc::CaloCluster cluster;
    auto& clusterCore = cluster.core();
    double posX = 0.;
    double posY = 0.;
    double posZ = 0.;
    double energy = 0.;
    //    for (auto cellId : i.second) {
//      for (const auto& iCell : *Cells) {
//	auto iCellId = iCell.cellId();
//	if (cellId == iCellId) { // found CellId from preClustering in input CellCollection
//	  fcc::CaloHit newCell = edmClusterCells->create();
//	  newCell.core().cellId = iCellId;
//	  newCell.core().energy = iCell.energy();
//	  newCell.core().time = iCell.time();
//	  cluster.addhits(newCell);
//	  energy += iCell.energy();
//	  DD4hep::Geometry::Position posCell = m_cellPositionsTool->getXYZPosition(iCellId);
//	  posX += posCell.X() * iCell.energy();
//	  posY += posCell.Y() * iCell.energy();
//	  posZ += posCell.Z() * iCell.energy();
//	  m_allCells.erase(cellId);
//	}
//      }
    //   }
    clusterCore.energy = energy;
    clusterCore.position.x = posX / energy;
    clusterCore.position.y = posY / energy;
    clusterCore.position.z = posZ / energy;
    debug() << "Cluster energy:     " << clusterCore.energy << endmsg;
    checkTotEnergy += clusterCore.energy;
    
    edmClusters->push_back(cluster);
  }
  m_clusterCellsCollection.put(edmClusterCells);
  info() << "Total energy of clusters:            " << checkTotEnergy << endmsg;
  info() << "Leftover cells :                      " << m_allCells.size() << endmsg;
  return StatusCode::SUCCESS;
}

void CaloTopoCluster::findingSeeds(const std::map<uint64_t, double>& cells,
				   int nSigma,
				   std::vector<std::pair<uint64_t, double> >& seeds,
				   INoiseConstTool& aNoiseTool) {
  for (const auto& cell : cells) {
    // retrieve the noise const assigned to cell
    double threshold = aNoiseTool.getNoiseConstantPerCell(cell.first) * nSigma;
    debug() << "seed threshold  = " << threshold << "MeV " << endmsg;
    if (abs(cell.second) / dd4hep::MeV > threshold) {  // seed threshold is set to 4*Sigma
      seeds.emplace_back(cell.first, cell.second);
    }
  }
}

void CaloTopoCluster::buildingProtoCluster(
					   int nSigma,
					   const std::unordered_map<uint64_t, std::vector<uint64_t>>
					   neighboursMap,
					   std::vector<std::pair<uint64_t, double> >& seeds,
					   std::map<uint64_t, double>& allCells,
					   std::map<uint, std::vector<uint64_t> >& preClusterCollection,
					   INoiseConstTool& aNoiseTool) {
  // Map of cellIds to clusterIds
  std::map<uint64_t, uint> clusterOfCell;

  // Loop over every seed in Calo to create first cluster
  uint iSeeds = 0;
  debug() << "seeds to loop over : " << seeds.size() << endmsg;
  for (auto& itSeed : seeds) {
    iSeeds++;
    debug() << "Seed num: " << iSeeds << endmsg;
    //auto seedCell = *itSeed;
    auto seedId = itSeed.first;
    debug() << "Seeds Cell id :          " << seedId << endmsg;
    //seedCell.core().bits = 0;  // set cell Bits to 0 for seed cell

    auto cellInCluster = clusterOfCell.find(seedId);
    if (cellInCluster != clusterOfCell.end()) {
      debug() << "Seed is already assigned to another cluster!" << endmsg;
      continue;
    } else {
      // new cluster starts with seed
      preClusterCollection[iSeeds].push_back(seedId);
      uint clusterId = iSeeds;
      clusterOfCell[seedId] = clusterId;

      std::vector<std::vector<uint64_t>> N2(1000);
      std::vector<uint64_t> N1 = CaloTopoCluster::searchForNeighbours(
								      seedId, clusterId, neighboursMap, nSigma, allCells, clusterOfCell, preClusterCollection, aNoiseTool);
      // first loop over seeds neighbours
      N2[0] = N1;
      debug() << "Found " << N2[0].size() << " neighbours.." << endmsg;
      int it = 0;
      while (N2[it].size() > 0) {
	it++;
	for (auto& id : N2[it - 1]) {
	  N2[it] = CaloTopoCluster::searchForNeighbours(
							id, clusterId, neighboursMap, nSigma, allCells, clusterOfCell, preClusterCollection, aNoiseTool);
	}
	debug() << "Found " << N2[it].size() << " more neighbours.." << endmsg;
      }
      // last try without condition on neighbours 
      for (auto& id : N2[it]) {
	N2[it+1] = CaloTopoCluster::searchForNeighbours(
							id, clusterId, neighboursMap, 0., allCells, clusterOfCell, preClusterCollection, aNoiseTool);
      }    
    }
  }
}

std::vector<uint64_t>
CaloTopoCluster::searchForNeighbours(const uint64_t id,
				     uint& clusterNum,
				     const std::unordered_map<uint64_t, std::vector<uint64_t>>
				     neighboursMap,
				     int nSigma,
				     const std::map<uint64_t, double>& allCells,
				     std::map<uint64_t, uint>& clusterOfCell,
				     std::map<uint, std::vector<uint64_t>>& preClusterCollection,
				     INoiseConstTool& aNoiseTool) {
  // Fill vector to be returned, next ids for which neighbours are found
  std::vector<uint64_t> addedNeighbourIds;
  auto search = neighboursMap.find(id);
  if (search == neighboursMap.end()) { warning() << "Cannot find cellID in map to neighbours! " << endmsg;      
    addedNeighbourIds.resize(0);
    return addedNeighbourIds;
  }
  else{

    debug() << "For cluster: " << clusterNum << endmsg;
    bool rmCluster = false;
    int addFoundNeighboursToCluster = 0;

    // loop over neighbours
    for (auto& itr : search->second) {
      auto neighbourID = itr;
      // Find the neighbour in the Calo cells list
      auto itAllCells = allCells.find(neighbourID);
      auto itAllUsedCells = clusterOfCell.find(neighbourID);

      // If cell is hit.. and is not assigned to a cluster
      if (itAllCells != allCells.end() && itAllUsedCells == clusterOfCell.end()) {
	debug() << "Found neighbour with CellID: " << neighbourID << endmsg;
	auto neighbouringCellId = itAllCells->first;
	auto neighbouringCellEnergy = itAllCells->second;
	bool validatedNeighbour = false;
	if ( nSigma == 0 ) // no condition to be checked for neighbour
	  validatedNeighbour = true;
	else {
	  // retrieve the cell noise level
	  double thr = nSigma * aNoiseTool.getNoiseConstantPerCell(neighbouringCellId);
	  if (abs(neighbouringCellEnergy) > thr)
	    validatedNeighbour = true;
	  else
	    validatedNeighbour = false;
	}
	// if neighbour is validated
	if (validatedNeighbour ) {
	  // retrieve the cell
	  // add neighbour to cells for cluster
	  //neighbouringCell.core().bits = 1;  // set Bits to 1 for neighbour cell
	  preClusterCollection[clusterNum].push_back(neighbouringCellId);
	  clusterOfCell[neighbourID] = clusterNum;
	  addedNeighbourIds.push_back(neighbourID);
	}
      }
      // If cell is hit.. but is assigned to another cluster
      else if (itAllUsedCells != clusterOfCell.end() && itAllUsedCells->second != clusterNum) {
	auto clusterNumToMerge = itAllUsedCells->second;
	debug() << "This neighbour was found in cluster " << clusterNumToMerge << ", mark cluster " << clusterNum
		<< " to be merged!" << endmsg;
	// Mark cluster to be removed and set the next neighbours to be assigned to found cluster
	rmCluster = true;
	addFoundNeighboursToCluster = clusterNumToMerge;
      } else  // cell is either not a hit or already added to cluster
	continue;
    }
    // If the current cluster was assigned to another it is removed from the map
    if (rmCluster) {
      debug() << "Assigning all cells ( " << preClusterCollection.find(clusterNum)->second.size() << " ) to Cluster "
	      << addFoundNeighboursToCluster << " with ( "
	      << preClusterCollection.find(addFoundNeighboursToCluster)->second.size() << " ). " << endmsg;
      // Fill all cells into cluster, and assigned cells to new cluster
      for (auto& i : preClusterCollection.find(clusterNum)->second) {
	clusterOfCell[i] = addFoundNeighboursToCluster;
	bool found = false;
	// make sure that already assigned cells are not added
	for (auto& j : preClusterCollection[addFoundNeighboursToCluster]) {
	  if (j == i) found = true;
	}
	if (found) continue;
	preClusterCollection[addFoundNeighboursToCluster].push_back(i);
      }
      debug() << "Cluster " << clusterNum << " is removed!" << endmsg;
      preClusterCollection.erase(clusterNum);
      // changed clusterId -> if more neighbours are found, correct assignment
      clusterNum = addFoundNeighboursToCluster;
    }
    return addedNeighbourIds;
  }
}

StatusCode CaloTopoCluster::finalize() { return GaudiAlgorithm::finalize(); }
