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
                  "Handle for tool to retrieve cell positions in HCal Barrel and ext Barrel");
  declareProperty("positionsHCalExtBarrelTool", m_cellPositionsHCalExtBarrelTool,
                  "Handle for tool to retrieve cell positions in HCal Barrel and ext Barrel");
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
    return StatusCode::FAILURE;
  }
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
  std::sort(m_firstSeeds.begin(), m_firstSeeds.end(),
            [](const std::pair<uint64_t, double>& lhs, const std::pair<uint64_t, double>& rhs) {
              return lhs.second < rhs.second;
            });

  std::map<uint, std::vector<std::pair<uint64_t, uint>>> preClusterCollection;
  CaloTopoCluster::buildingProtoCluster(m_neighbourSigma, m_lastNeighbourSigma, m_firstSeeds, m_allCells,
                                        preClusterCollection);
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
      auto newCell = edmClusterCells->create();
      newCell.core().energy = m_allCells[cellId];
      newCell.core().cellId = cellId;
      newCell.core().bits = pair.second;
      energy += newCell.core().energy;

      // get cell position by cellID
      // identify calo system
      m_decoder->setValue(cellId);
      auto systemId = (*m_decoder)["system"].value();
      dd4hep::Position posCell;
      if (systemId == 5)  // ECAL BARREL system id
        posCell = m_cellPositionsECalBarrelTool->xyzPosition(cellId);
      else if (systemId == 8)  // HCAL BARREL system id
        posCell = m_cellPositionsHCalBarrelTool->xyzPosition(cellId);
      else if (systemId == 9)  // HCAL EXT BARREL system id
        posCell = m_cellPositionsHCalExtBarrelTool->xyzPosition(cellId);
      else if (systemId == 6)  // EMEC system id
        posCell = m_cellPositionsEMECTool->xyzPosition(cellId);
      else if (systemId == 7)  // HEC system id
        posCell = m_cellPositionsHECTool->xyzPosition(cellId);
      else if (systemId == 10)  // EMFWD system id
        posCell = m_cellPositionsEMFwdTool->xyzPosition(cellId);
      else if (systemId == 11)  // HFWD system id
        posCell = m_cellPositionsHFwdTool->xyzPosition(cellId);
      else
        warning() << "No cell positions tool found for system id " << systemId << ". " << endmsg;

      posX += posCell.X() * newCell.core().energy;
      posY += posCell.Y() * newCell.core().energy;
      posZ += posCell.Z() * newCell.core().energy;
      // edmClusterCells->push_back(newCell);
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
                                   std::vector<std::pair<uint64_t, double>>& seeds) {
  for (const auto& cell : cells) {
    // retrieve the noise const assigned to cell
    double threshold = m_noiseTool->noise(cell.first) / dd4hep::MeV * nSigma;
    debug() << "seed threshold  = " << threshold << "MeV " << endmsg;
    if (abs(cell.second) / dd4hep::MeV > threshold) {  // seed threshold is set to 4*Sigma
      seeds.emplace_back(cell.first, cell.second);
    }
  }
}

void CaloTopoCluster::buildingProtoCluster(
    int nSigma,
    int lastNSigma,
    std::vector<std::pair<uint64_t, double>>& seeds,
    const std::map<uint64_t, double>& allCells,
    std::map<uint, std::vector<std::pair<uint64_t, uint>>>& preClusterCollection) {
  // Map of cellIds to clusterIds
  std::map<uint64_t, uint> clusterOfCell;

  // Loop over every seed in Calo to create first cluster
  uint iSeeds = 0;
  debug() << "seeds to loop over : " << seeds.size() << endmsg;
  for (auto& itSeed : seeds) {
    iSeeds++;
    debug() << "Seed num: " << iSeeds << endmsg;
    // auto seedCell = *itSeed;
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

      std::vector<std::vector<std::pair<uint64_t, uint>>> N2(1000);
      auto N1 = CaloTopoCluster::searchForNeighbours(seedId, clusterId, nSigma, allCells, clusterOfCell,
                                                     preClusterCollection);
      // first loop over seeds neighbours
      N2[0] = N1;
      debug() << "Found " << N2[0].size() << " neighbours.." << endmsg;
      int it = 0;
      while (N2[it].size() > 0) {
        it++;
        for (auto& id : N2[it - 1]) {
          debug() << "Next neighbours assigned to clusterId : " << id.second << endmsg;
          N2[it] = CaloTopoCluster::searchForNeighbours(id.first, clusterId, nSigma, allCells, clusterOfCell,
                                                        preClusterCollection);
        }
        debug() << "Found " << N2[it].size() << " more neighbours.." << endmsg;
      }
      // last try without condition on neighbours
      for (auto& id : N2[it]) {
        N2[it + 1] = CaloTopoCluster::searchForNeighbours(id.first, clusterId, lastNSigma, allCells, clusterOfCell,
                                                          preClusterCollection);
      }
    }
  }
}

std::vector<std::pair<uint64_t, uint>>
CaloTopoCluster::searchForNeighbours(const uint64_t id,
                                     uint& clusterID,
                                     int nSigma,
                                     const std::map<uint64_t, double>& allCells,
                                     std::map<uint64_t, uint>& clusterOfCell,
                                     std::map<uint, std::vector<std::pair<uint64_t, uint>>>& preClusterCollection) {
  // Fill vector to be returned, next cell ids and cluster id for which neighbours are found
  std::vector<std::pair<uint64_t, uint>> addedNeighbourIds;
  // Retrieve cellIds of neighbours
  auto neighboursVec = m_neighboursTool->neighbours(id);
  if (neighboursVec.size() == 0) {
    error() << "No neighbours for cellID found! " << endmsg;
    addedNeighbourIds.resize(0);
  } else {

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
        auto neighbouringCellEnergy = itAllCells->second;
        bool validatedNeighbour = false;
        if (nSigma == 0)  // no condition to be checked for neighbour
          validatedNeighbour = true;
        else {
          // retrieve the cell noise level [GeV]
          double thr = nSigma * m_noiseTool->noise(neighbourID);
          if (abs(neighbouringCellEnergy) > thr)
            validatedNeighbour = true;
          else
            validatedNeighbour = false;
        }
        // if neighbour is validated
        if (validatedNeighbour) {
          // retrieve the cell
          // add neighbour to cells for cluster
          // set Bits to 1 for neighbour cell
          preClusterCollection[clusterID].push_back(std::make_pair(neighbourID, 1));
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
                << clusterIDToMerge << " with ( " << preClusterCollection.find(clusterIDToMerge)->second.size()
                << " ). " << endmsg;
        // Fill all cells into cluster, and assigned cells to new cluster
        clusterOfCell[neighbourID] = clusterIDToMerge;
        for (auto& i : preClusterCollection.find(clusterID)->second) {
          clusterOfCell[i.first] = clusterIDToMerge;
          bool found = false;
          // make sure that already assigned cells are not added
          for (auto& j : preClusterCollection[clusterIDToMerge]) {
            if (j.first == i.first) found = true;
          }
          if (!found) {
            preClusterCollection[clusterIDToMerge].push_back(std::make_pair(i.first, i.second));
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
  }
  return addedNeighbourIds;
}

StatusCode CaloTopoCluster::finalize() { return GaudiAlgorithm::finalize(); }
