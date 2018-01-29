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
  declareProperty("neigboursTool", m_neighboursTool, "Handle for tool to retrieve cell neighbours");
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
  if (!m_neighboursTool.retrieve()) {
    error() << "Unable to retrieve the cells neighbours tool!!!" << endmsg;
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
  return StatusCode::SUCCESS;
}

StatusCode CaloTopoCluster::execute() {
  // Clear maps before filling
  m_allCells.clear();
  m_firstSeeds.clear();
  
  // Create output collections
  auto edmClusters = m_clusterCollection.createAndPut();
  fcc::CaloHitCollection* edmClusterCells = new fcc::CaloHitCollection();
  
  // get input cell map from input tool
  m_allCells = m_inputTool->cellIdMap();
  info() << "Active Cells          :    " << m_allCells.size() << endmsg;
  
  // Finds seeds and fills the list of allCells
  CaloTopoCluster::findingSeeds(m_allCells, m_seedSigma, m_firstSeeds);
  info() << "Number of seeds found :    " << m_firstSeeds.size() << endmsg;

  // decending order of seeds
  std::sort(m_firstSeeds.begin(), m_firstSeeds.end(), [](const std::pair<uint64_t, double>& lhs, const std::pair<uint64_t, double>& rhs){return lhs.second < rhs.second;});

  std::map<uint, std::vector<std::pair<uint64_t, uint> >> preClusterCollection;
      CaloTopoCluster::buildingProtoCluster(m_neighbourSigma, m_firstSeeds,
					    m_allCells, preClusterCollection);
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
    for (auto pair : i.second) {
      auto cellId = pair.first;
      // get CaloHit by cellID
      auto newCell = edmClusterCells->create(); //m_inputTool->cellByCellId(cellId);
      newCell.energy( m_allCells[cellId] );
      newCell.cellId( cellId );
      newCell.bits( pair.second );
      energy += newCell.energy();
      // get cell position by cellID
      DD4hep::Geometry::Position posCell = m_cellPositionsTool->getXYZPosition(cellId);
      posX += posCell.X() * newCell.energy();
      posY += posCell.Y() * newCell.energy();
      posZ += posCell.Z() * newCell.energy();
      //edmClusterCells->push_back(newCell);
      cluster.addhits(newCell);
      m_allCells.erase(cellId);
    }
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
				   std::vector<std::pair<uint64_t, double> >& seeds) {
  for (const auto& cell : cells) {
    // retrieve the noise const assigned to cell
    double threshold = m_noiseTool->getNoiseConstantPerCell(cell.first) * nSigma;
    debug() << "seed threshold  = " << threshold << "MeV " << endmsg;
    if (abs(cell.second) / dd4hep::MeV > threshold) {  // seed threshold is set to 4*Sigma
      seeds.emplace_back(cell.first, cell.second);
    }
  }
}

void CaloTopoCluster::buildingProtoCluster(
					   int nSigma,
					   std::vector<std::pair<uint64_t, double> >& seeds,
					   std::map<uint64_t, double>& allCells,
					   std::map<uint, std::vector<std::pair<uint64_t, uint> >>& preClusterCollection ) {
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
    auto cellInCluster = clusterOfCell.find(seedId);
    if (cellInCluster != clusterOfCell.end()) {
      debug() << "Seed is already assigned to another cluster!" << endmsg;
      continue;
    } else {
      // new cluster starts with seed
      // set cell Bits to 0 for seed cell
      preClusterCollection[iSeeds].push_back(std::make_pair(seedId, 0));
      uint clusterId = iSeeds;
      clusterOfCell[seedId] = clusterId;

      std::vector<std::vector<std::pair<uint64_t, uint> > > N2(1000);
      auto N1 = CaloTopoCluster::searchForNeighbours(
								      seedId, clusterId, nSigma, allCells, clusterOfCell, preClusterCollection);
      // first loop over seeds neighbours
      N2[0] = N1;
      debug() << "Found " << N2[0].size() << " neighbours.." << endmsg;
      int it = 0;
      while (N2[it].size() > 0) {
	it++;
	for (auto& id : N2[it - 1]) {
	  debug() << "Next neighbours assigned to clusterId : " << id.second << endmsg;
	  N2[it] = CaloTopoCluster::searchForNeighbours(
							id.first, clusterId, nSigma, allCells, clusterOfCell, preClusterCollection);
	}
	debug() << "Found " << N2[it].size() << " more neighbours.." << endmsg;
      }
      // last try without condition on neighbours 
      for (auto& id : N2[it]) {
	N2[it+1] = CaloTopoCluster::searchForNeighbours(
							id.first, clusterId, 0., allCells, clusterOfCell, preClusterCollection);
      }    
    }
  }
}

std::vector<std::pair<uint64_t, uint> >
CaloTopoCluster::searchForNeighbours(const uint64_t id,
				     uint& clusterID,
				     int nSigma,
				     const std::map<uint64_t, double>& allCells,
				     std::map<uint64_t, uint>& clusterOfCell,
				     std::map<uint, std::vector<std::pair<uint64_t,uint> >>& preClusterCollection ) {
  // Fill vector to be returned, next cell ids and cluster id for which neighbours are found
  std::vector<std::pair<uint64_t,uint> > addedNeighbourIds;
  // Retrieve cellIds of neighbours
  auto neighboursVec = m_neighboursTool->neighbours(id);
  if (neighboursVec.size() == 0) { warning() << "No neighbours for cellID found! " << endmsg;      
    addedNeighbourIds.resize(0);
    return addedNeighbourIds;
  }
  else{
    
    debug() << "For cluster: " << clusterID << endmsg;
    // loop over neighbours
    for (auto& itr : neighboursVec) {
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
	  double thr = nSigma * m_noiseTool->getNoiseConstantPerCell(neighbouringCellId);
	  if (abs(neighbouringCellEnergy) >= thr)
	    validatedNeighbour = true;
	  else
	    validatedNeighbour = false;
	}
	// if neighbour is validated
	if (validatedNeighbour) {
	  // retrieve the cell
	  // add neighbour to cells for cluster
	  // set Bits to 1 for neighbour cell
	  preClusterCollection[clusterID].push_back(std::make_pair(neighbouringCellId, 1));
	  clusterOfCell[neighbourID] = clusterID;
	  addedNeighbourIds.push_back(std::make_pair(neighbourID, clusterID));
	}
      }
      // If cell is hit.. but is assigned to another cluster
      else if (itAllUsedCells != clusterOfCell.end() && itAllUsedCells->second != clusterID) {
	uint clusterIDToMerge = itAllUsedCells->second;
	debug() << "This neighbour was found in cluster " << clusterIDToMerge << ", cluster " << clusterID
		<< " will be merged!" << endmsg;
	debug() << "Assigning all cells ( " << preClusterCollection.find(clusterID)->second.size() << " ) to Cluster "
		<< clusterIDToMerge << " with ( "
		<< preClusterCollection.find(clusterIDToMerge)->second.size() << " ). " << endmsg;
	// Fill all cells into cluster, and assigned cells to new cluster
	clusterOfCell[neighbourID] = clusterIDToMerge;
	for (auto& i : preClusterCollection.find(clusterID)->second) {
	  clusterOfCell[i.first] = clusterIDToMerge;
	  bool found = false;
	  // make sure that already assigned cells are not added
	  for (auto& j : preClusterCollection[clusterIDToMerge]) {
	    if (j.first == i.first) found = true;
	  }
	  if (!found){
	    preClusterCollection[clusterIDToMerge].push_back(std::make_pair(i.first, 2));
	  }
	}
	preClusterCollection.erase(clusterID);
	// changed clusterId -> if more neighbours are found, correct assignment
	debug() << "Cluster Id changed to " << clusterIDToMerge << endmsg;
	clusterID = clusterIDToMerge;
	// found neighbour for next search
	addedNeighbourIds.push_back(std::make_pair(neighbourID, clusterID));
	// end loop to ensure correct cluster assignment
	break;
      }
    }
    return addedNeighbourIds;
  }
}

StatusCode CaloTopoCluster::finalize() { return GaudiAlgorithm::finalize(); }
