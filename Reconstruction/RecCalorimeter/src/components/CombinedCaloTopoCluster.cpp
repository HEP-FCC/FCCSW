#include "CombinedCaloTopoCluster.h"
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

DECLARE_ALGORITHM_FACTORY(CombinedCaloTopoCluster)

CombinedCaloTopoCluster::CombinedCaloTopoCluster(const std::string& name, ISvcLocator* svcLoc)
: GaudiAlgorithm(name, svcLoc) {
  declareProperty("ECalCells", m_ECalCells, "calo/ECalCells (input)");
  declareProperty("HCalCells", m_HCalCells, "calo/HCalCells (input)");
  declareProperty("ECalReadoutName", m_ECalReadoutName);
  declareProperty("HCalReadoutName", m_HCalReadoutName);
  declareProperty("noiseToolECal", m_noiseToolECal, "Handle for the ECal cells noise tool");
  declareProperty("noiseToolHCal", m_noiseToolHCal, "Handle for the HCal cells noise tool");
  declareProperty("positionsToolECal", m_cellPositionsToolECal, "Handle for tool to retrieve cell positions in ECal");
  declareProperty("positionsToolHCal", m_cellPositionsToolHCal, "Handle for tool to retrieve cell positions in HCal");
  declareProperty("neighboursRange", m_range);
  declareProperty("clusters", m_clusterCollection, "Handle for calo clusters (output collection)");
  declareProperty("clusterCellsECal", m_clusterECalCellsCollection, "Handle for ECal clusters (output collection)");
  declareProperty("clusterCellsHCal", m_clusterHCalCellsCollection, "Handle for HCal clusters (output collection)");
}

StatusCode CombinedCaloTopoCluster::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) return StatusCode::FAILURE;
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  // check if readouts exist
  if (m_geoSvc->lcdd()->readouts().find(m_ECalReadoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout " << m_ECalReadoutName << " does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_geoSvc->lcdd()->readouts().find(m_HCalReadoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout " << m_HCalReadoutName << " does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  // retrieve PhiEta segmentation
  m_ECalSegmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(
									 m_geoSvc->lcdd()->readout(m_ECalReadoutName).segmentation().segmentation());
  if (m_ECalSegmentation == nullptr) {
    error() << "There is no phi-eta segmentation in the electromagnetic calorimeter." << endmsg;
    return StatusCode::FAILURE;
  }
  m_HCalSegmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(
									 m_geoSvc->lcdd()->readout(m_HCalReadoutName).segmentation().segmentation());
  if (m_HCalSegmentation == nullptr) {
    error() << "There is no segmentation in the hadronic calorimeter." << endmsg;
    return StatusCode::FAILURE;
  }

  // Check if noise tool is available for HCal, only used for simulations incl. noise before
  if (!m_noiseECal) {
    info() << "No noise in ECal." << endmsg;
  }
  if (m_noiseECal) {
    if (!m_noiseToolECal.retrieve()) {
      error() << "Unable to retrieve the ECal cells noise tool!!!" << endmsg;
      return StatusCode::FAILURE;
    }
    info() << "ECal cells noise added and thresholds adjusted!" << endmsg;
  }

  // Check if noise tool is available for HCal, only used for simulations incl. noise before
  if (!m_noiseHCal) {
    info() << "No noise in HCal." << endmsg;
  }
  if (m_noiseHCal) {
    if (!m_noiseToolHCal.retrieve()) {
      error() << "Unable to retrieve the HCal cells noise tool!!!" << endmsg;
      return StatusCode::FAILURE;
    }
    info() << "HCal cells noise added and thresholds adjusted!" << endmsg;
  }

  // Check if cell position tool available
  if (!m_cellPositionsToolECal.retrieve()) {
    error() << "Unable to retrieve the cell positions tool for ECal!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_cellPositionsToolHCal.retrieve()) {
    error() << "Unable to retrieve the cell positions tool for HCal!!!" << endmsg;
    return StatusCode::FAILURE;
  }

  // Take readout bitfield decoder from GeoSvc
  m_decoderECal = m_geoSvc->lcdd()->readout(m_ECalReadoutName).segmentation().segmentation()->decoder();
  m_decoderHCal = m_geoSvc->lcdd()->readout(m_HCalReadoutName).segmentation().segmentation()->decoder();

  m_fieldExtremesECal = det::utils::bitfieldExtremes((*m_decoderECal), m_fieldNamesECal);
  m_fieldExtremesHCal = det::utils::bitfieldExtremes((*m_decoderHCal), m_fieldNamesHCal);

  return StatusCode::SUCCESS;
}

StatusCode CombinedCaloTopoCluster::execute() {
  // Clear maps before filling
  m_neighboursMapECal.clear();
  m_neighboursMapHCal.clear();
  m_allCellsECal.clear();
  m_allCellsHCal.clear();
  m_firstSeedsECal.clear();
  m_firstSeedsHCal.clear();
 
  // Create output collections
  auto edmClusters = m_clusterCollection.createAndPut();
  fcc::CaloHitCollection* edmClusterCellsECal = new fcc::CaloHitCollection();
  fcc::CaloHitCollection* edmClusterCellsHCal = new fcc::CaloHitCollection();

  const fcc::CaloHitCollection* ECalCells = m_ECalCells.get();
  const fcc::CaloHitCollection* HCalCells = m_HCalCells.get();

  // Filling of map for each Cell to its neighbours
  int numN_ECal = 0, numN_HCal = 0;
  for (const auto& itCells : *ECalCells) {
    uint64_t cellID = itCells.cellId();
    std::vector<uint64_t> NeighboursVec =
      det::utils::neighbours(*m_decoderECal, m_fieldNamesECal, m_fieldExtremesECal, cellID);
    m_neighboursMapECal.emplace(cellID, NeighboursVec);
    numN_ECal = NeighboursVec.size();
  }
  debug() << "Number of entries in neighbor map ECal:           " << m_neighboursMapECal.size() << endmsg;
  debug() << "Number neighbors per Cell ECal:                   " << numN_ECal << endmsg;

  for (const auto& itCells : *HCalCells) {
    uint64_t cellID = itCells.cellId();
    std::vector<uint64_t> NeighboursVec =
      det::utils::neighbours(*m_decoderHCal, m_fieldNamesHCal, m_fieldExtremesHCal, cellID);
    m_neighboursMapHCal.emplace(cellID, NeighboursVec);
    numN_HCal = NeighboursVec.size();
  }
  debug() << "Number of entries in neighbor map HCal:           " << m_neighboursMapHCal.size() << endmsg;
  debug() << "Number neighbors per Cell HCal:                   " << numN_HCal << endmsg;

  // Finds seeds and fills the list of allCells
  if (m_noiseECal)
    CombinedCaloTopoCluster::findingSeedsWNoise(ECalCells, m_seedThr_ECal, m_firstSeedsECal, m_allCellsECal,
                                                *m_noiseToolECal);
  else
    CombinedCaloTopoCluster::findingSeeds(ECalCells, m_seedThr_ECal, m_firstSeedsECal, m_allCellsECal);

  if (m_noiseHCal)
    CombinedCaloTopoCluster::findingSeedsWNoise(HCalCells, m_seedThr_HCal, m_firstSeedsHCal, m_allCellsHCal,
                                                *m_noiseToolHCal);
  else
    CombinedCaloTopoCluster::findingSeeds(HCalCells, m_seedThr_HCal, m_firstSeedsHCal, m_allCellsHCal);

  info() << "Number of seeds found in ECal :    " << m_firstSeedsECal.size() << endmsg;
  info() << "Number of seeds found in HCal :    " << m_firstSeedsHCal.size() << endmsg;
  info() << "Active Cells in ECal          :    " << m_allCellsECal.size() << endmsg;
  info() << "Active Cells in HCal          :    " << m_allCellsHCal.size() << endmsg;

  // decending order of seeds
  std::sort(m_firstSeedsECal.begin(), m_firstSeedsECal.end(), []( const fcc::CaloHit& hit1, const fcc::CaloHit& hit2)
	    { 
	      return hit1.core().energy < hit2.core().energy;
	    });
  std::sort(m_firstSeedsHCal.begin(), m_firstSeedsHCal.end(), []( const fcc::CaloHit& hit1, const fcc::CaloHit& hit2)
	    { 
	      return hit1.core().energy < hit2.core().energy;
	    });

  std::map<uint, std::vector<fcc::CaloHit>> preClusterCollectionECal;
  std::map<uint, std::vector<fcc::CaloHit>> preClusterCollectionHCal;

  // Finding the Clusters in ECal calling  searchForNeighbours function
  if (m_noiseECal)
    CombinedCaloTopoCluster::buildingProtoClusterWNoise(m_neighbourThr_ECal, m_neighboursMapECal, m_firstSeedsECal,
                                                        m_allCellsECal, preClusterCollectionECal, *m_noiseToolECal);
  else
    CombinedCaloTopoCluster::buildingProtoCluster(m_neighbourThr_ECal, m_neighboursMapECal, m_firstSeedsECal,
                                                  m_allCellsECal, preClusterCollectionECal);

  // Build ECal Clusters in edm
  info() << "Building " << preClusterCollectionECal.size() << " ECal cluster." << endmsg;
  std::map<fcc::CaloCluster, std::pair<double, double>> mergeClustersEtaMap;
  std::map<fcc::CaloCluster, std::pair<double, double>> mergeClustersPhiMap;
  double checkTotEnergy = 0.;
  for (auto i : preClusterCollectionECal) {
    fcc::CaloCluster cluster;
    auto& clusterCore = cluster.core();
    std::vector<double> posEta;
    std::vector<double> posPhi;
    double posX = 0.;
    double posY = 0.;
    double posZ = 0.;
    double energy = 0.;
    bool clusterToBeMerged = false;
    double meanEtaLastLayer = 0;
    double meanPhiLastLayer = 0;
    int numCellsLastLayer = 0;
    for (auto cell : i.second) {
      energy += cell.energy();
      fcc::CaloHit newCell = edmClusterCellsECal->create();
      newCell.core().cellId = cell.cellId();
      newCell.core().energy = cell.energy();
      newCell.core().time = cell.time();
      cluster.addhits(newCell);
      m_allCellsECal.erase(cell.cellId());
      DD4hep::Geometry::Position posCell = m_cellPositionsToolECal->getXYZPosition(cell);
      posX += posCell.X() * cell.energy();
      posY += posCell.Y() * cell.energy();
      posZ += posCell.Z() * cell.energy();
      posEta.push_back(posCell.Eta());
      posPhi.push_back(posCell.Phi());
      m_decoderECal->setValue(cell.cellId());
      auto layerNewCell = (*m_decoderECal)["layer"].value();
      if (layerNewCell == m_lastECalLayer) {
        clusterToBeMerged = true;
        meanEtaLastLayer += posCell.Eta();
        meanPhiLastLayer += posCell.Phi();
        numCellsLastLayer++;
      }
    }
    clusterCore.energy = energy;
    clusterCore.position.x = posX / energy;
    clusterCore.position.y = posY / energy;
    clusterCore.position.z = posZ / energy;
    debug() << "Cluster energy:     " << clusterCore.energy << endmsg;
    checkTotEnergy += clusterCore.energy;
    if (clusterToBeMerged) {
      meanEtaLastLayer = meanEtaLastLayer / numCellsLastLayer;
      meanPhiLastLayer = meanPhiLastLayer / numCellsLastLayer;
      double etaMin = meanEtaLastLayer - 2. * 0.025;
      double etaMax = meanEtaLastLayer + 2. * 0.025;
      double phiMin = meanPhiLastLayer - 2. * std::abs(m_HCalSegmentation->gridSizePhi());
      double phiMax = meanPhiLastLayer + 2. * std::abs(m_HCalSegmentation->gridSizePhi());
      debug() << "Eta-/+rms:     " << etaMin << "/" << etaMax << endmsg;
      debug() << "Phi-/+rms:     " << phiMin << "/" << phiMax << endmsg;
      mergeClustersEtaMap.emplace(cluster, std::make_pair(etaMin, etaMax));
      mergeClustersPhiMap.emplace(cluster, std::make_pair(phiMin, phiMax));
    } else {
      edmClusters->push_back(cluster);
    }
  }
  m_clusterECalCellsCollection.put(edmClusterCellsECal);

  info() << "Clusters in ECal to be merged with HCal:     " << mergeClustersEtaMap.size() << endmsg;
  info() << "Total energy of clusters in ECal:            " << checkTotEnergy << endmsg;
  info() << "Leftover cells in ECal:                      " << m_allCellsECal.size() << endmsg;

  // Finding the Clusters in HCal calling  searchForNeighbours function
  if (m_noiseHCal)
    CombinedCaloTopoCluster::buildingProtoClusterWNoise(m_neighbourThr_HCal, m_neighboursMapHCal, m_firstSeedsHCal,
                                                        m_allCellsHCal, preClusterCollectionHCal, *m_noiseToolHCal);
  else
    CombinedCaloTopoCluster::buildingProtoCluster(m_neighbourThr_HCal, m_neighboursMapHCal, m_firstSeedsHCal,
                                                  m_allCellsHCal, preClusterCollectionHCal);

  // Build HCal Clusters in edm
  info() << "Building " << preClusterCollectionHCal.size() << " HCal cluster." << endmsg;
  for (auto i : preClusterCollectionHCal) {
    fcc::CaloCluster cluster;
    auto& clusterCore = cluster.core();
    double posX = 0.;
    double posY = 0.;
    double posZ = 0.;
    double energy = 0.;
    bool clusterToBeMerged = false;
    double meanEtaFirstLayer = 0;
    double meanPhiFirstLayer = 0;
    int numCellsFirstLayer = 0;
    for (auto cell : i.second) {
      energy += cell.energy();
      fcc::CaloHit newCell = edmClusterCellsHCal->create();
      newCell.core().cellId = cell.cellId();
      newCell.core().energy = cell.energy();
      newCell.core().time = cell.time();
      DD4hep::Geometry::Position posCell = m_cellPositionsToolHCal->getXYZPosition(cell);
      posX += posCell.X() * cell.energy();
      posY += posCell.Y() * cell.energy();
      posZ += posCell.Z() * cell.energy();
      cluster.addhits(newCell);
      m_allCellsHCal.erase(cell.cellId());

      m_decoderHCal->setValue(cell.cellId());
      auto layerCell = (*m_decoderHCal)["layer"].value();
      if (layerCell == 0) {
        clusterToBeMerged = true;
        meanEtaFirstLayer += posCell.Eta();
        meanPhiFirstLayer += posCell.Phi();
        numCellsFirstLayer++;
      }
    }
    clusterCore.energy = energy;
    clusterCore.position.x = posX / energy;
    clusterCore.position.y = posY / energy;
    clusterCore.position.z = posZ / energy;
    debug() << "Cluster energy:       " << clusterCore.energy << endmsg;
    debug() << "Cluster position:     " << clusterCore.position.x << ", " << clusterCore.position.y << ", " << clusterCore.position.z << endmsg;

    // Check if this cluster is a canditate to be merged with one in the ECal
    if (clusterToBeMerged) {
      debug() << "Cluster with cell in first HCal layer found!" << endmsg;
      meanEtaFirstLayer = meanEtaFirstLayer / numCellsFirstLayer;
      meanPhiFirstLayer = meanPhiFirstLayer / numCellsFirstLayer;
      // use cell positions (layer==0) in eta/phi
      double eta = meanEtaFirstLayer;
      double phi = meanPhiFirstLayer;
      debug() << "Loop over " << mergeClustersEtaMap.size() << " ECal cluster to find match. " << endmsg;
      // Loop through ECalCluster
      for (auto const& clusterECalEta : mergeClustersEtaMap) {
        // Check Eta region
        if (eta >= clusterECalEta.second.first && eta <= clusterECalEta.second.second) {
          // Check Phi region
          if (phi >= mergeClustersPhiMap[clusterECalEta.first].first &&
              phi <= mergeClustersPhiMap[clusterECalEta.first].second) {
            debug() << "Found cluster to merge!" << endmsg;
            // Adding all ECal cells to the cluster
            for (auto itECalCells = mergeClustersPhiMap.find(clusterECalEta.first)->first.hits_begin();
                 itECalCells != mergeClustersPhiMap.find(clusterECalEta.first)->first.hits_end();
                 ++itECalCells) {
              cluster.addhits(*itECalCells);
            }
            // Setting new cluster poperties
            // Energy weighted positions and energy sum
            clusterCore.position.x = ((clusterCore.position.x * clusterCore.energy) +
                                      (clusterECalEta.first.core().position.x * clusterECalEta.first.core().energy)) /
	      (clusterCore.energy + clusterECalEta.first.core().energy);
            clusterCore.position.y = ((clusterCore.position.y * clusterCore.energy) +
                                      (clusterECalEta.first.core().position.y * clusterECalEta.first.core().energy)) /
	      (clusterCore.energy + clusterECalEta.first.core().energy);
            clusterCore.position.z = ((clusterCore.position.z * clusterCore.energy) +
                                      (clusterECalEta.first.core().position.z * clusterECalEta.first.core().energy)) /
	      (clusterCore.energy + clusterECalEta.first.core().energy);
            clusterCore.energy = clusterCore.energy + clusterECalEta.first.core().energy;
            // Erase the ECal cluster from maps
            mergeClustersPhiMap.erase(clusterECalEta.first);
            mergeClustersEtaMap.erase(clusterECalEta.first);
          }
        }
      }
    }
    checkTotEnergy += clusterCore.energy;
    edmClusters->push_back(cluster);
  }
  m_clusterHCalCellsCollection.put(edmClusterCellsHCal);
  info() << "Leftover ECal clusters :                " << mergeClustersPhiMap.size() << endmsg;
  // Add to edm the ECal cluster without partner in HCal
  for (auto const& i : mergeClustersPhiMap) {
    fcc::CaloCluster cluster = i.first;
    checkTotEnergy += cluster.core().energy;
    edmClusters->push_back(cluster);
  }
  info() << "Total energy merged into clusters :     " << checkTotEnergy << endmsg;
  info() << "Leftover cells in HCal :                " << m_allCellsHCal.size() << endmsg;

  info() << "Number of total reconstructed clusters: " << edmClusters->size() << endmsg;

 return StatusCode::SUCCESS;
}

void CombinedCaloTopoCluster::findingSeedsWNoise(const fcc::CaloHitCollection* cells,
                                                 double threshold,
                                                 std::vector<fcc::CaloHit>& seeds,
                                                 std::map<uint64_t, fcc::CaloHit>& allCells,
                                                 IReadNoiseFileTool& aNoiseTool) {
  for (const auto& cell : *cells) {
    allCells[cell.cellId()] = cell;
    // retrieve the noise const assigned to cell
    threshold = aNoiseTool.getNoiseConstantPerCell(cell.cellId());
    debug() << "seed threshold  = " << threshold * 4 << "MeV " << endmsg;
    if (cell.core().energy / dd4hep::MeV > threshold * 4.) {  // seed threshold is set to 4*Sigma
      seeds.push_back(cell);
    }
  }
}
void CombinedCaloTopoCluster::findingSeeds(const fcc::CaloHitCollection* cells,
                                           double threshold,
                                           std::vector<fcc::CaloHit>& seeds,
                                           std::map<uint64_t, fcc::CaloHit>& allCells) {
  debug() << "seed threshold  = " << threshold * 4 << "MeV " << endmsg;
  for (const auto& cell : *cells) {
    allCells[cell.cellId()] = cell;
    if (cell.core().energy / dd4hep::MeV > threshold * 4.) {  // seed threshold is set to 4*Sigma
      seeds.push_back(cell);
    }
  }
}

void CombinedCaloTopoCluster::buildingProtoClusterWNoise(
							 double neighbourThr,
							 const std::unordered_map<uint64_t, std::vector<uint64_t>>
							 neighboursMap,
							 std::vector<fcc::CaloHit>& seeds,
							 std::map<uint64_t, fcc::CaloHit>& allCells,
							 std::map<uint, std::vector<fcc::CaloHit>>& preClusterCollection,
							 IReadNoiseFileTool& aNoiseTool) {
  // Map of cellIds to clusterIds
  std::map<uint64_t, uint> clusterOfCell;
  // New seed list for neighbours of original seeds
  std::vector<fcc::CaloHit> newSeeds;

  // Loop over every seed in Calo to create first cluster
  uint iSeeds = 0;
  debug() << "seeds to loop over : " << seeds.size() << endmsg;
  for (auto itSeed = seeds.begin(); itSeed != seeds.end(); ++itSeed) {
    iSeeds++;
    debug() << "Seed num: " << iSeeds << endmsg;
    auto seedCell = *itSeed;
    auto seedId = seedCell.cellId();
    debug() << "Seeds Cell id :          " << seedId << endmsg;
    seedCell.core().bits = 0;  // set cell Bits to 0 for seed cell

    auto cellInCluster = clusterOfCell.find(seedId);
    if (cellInCluster != clusterOfCell.end()) {
      debug() << "Seed is already assigned to another cluster!" << endmsg;
      continue;
    } else {
      // new cluster starts with seed
      preClusterCollection[iSeeds].push_back(seedCell);
      clusterOfCell[seedId] = iSeeds;

      uint clusterId = iSeeds;
      std::vector<std::vector<uint64_t>> N2(1000);
      std::vector<uint64_t> N1 = CombinedCaloTopoCluster::searchForNeighboursWNoise(
										    seedId, clusterId, neighboursMap, neighbourThr, allCells, clusterOfCell, preClusterCollection, aNoiseTool);
      N2[0] = N1;
      debug() << "Found " << N2[0].size() << " neighbours.." << endmsg;
      for (int it = 1; it < 1000; it++) {
        for (auto& id : N2[it - 1]) {
          N2[it] = CombinedCaloTopoCluster::searchForNeighboursWNoise(
								      id, clusterId, neighboursMap, neighbourThr, allCells, clusterOfCell, preClusterCollection, aNoiseTool);
          if (N2[it].size() == 0) {
            // try with neighbour threshold=0
            double thr = 0.;
            N2[it] = CombinedCaloTopoCluster::searchForNeighboursWNoise(
									id, clusterId, neighboursMap, thr, allCells, clusterOfCell, preClusterCollection, aNoiseTool);
            if (N2[it].size() == 0) break;
          }
          debug() << "Found " << N2[it].size() << " more neighbours.." << endmsg;
        }
      }
    }
  }
}
void CombinedCaloTopoCluster::buildingProtoCluster(double neighbourThr,
                                                   const std::unordered_map<uint64_t, std::vector<uint64_t>>
						   neighboursMap,
                                                   std::vector<fcc::CaloHit>& seeds,
                                                   std::map<uint64_t, fcc::CaloHit>& allCells,
                                                   std::map<uint, std::vector<fcc::CaloHit>>& preClusterCollection) {
  // Map of cellIds to clusterIds
  std::map<uint64_t, uint> clusterOfCell;
  // New seed list for neighbours of original seeds
  std::vector<fcc::CaloHit> newSeeds;

  // Loop over every seed in Calo to create first cluster
  uint iSeeds = 0;
  debug() << "seeds to loop over : " << seeds.size() << endmsg;
  for (auto itSeed = seeds.begin(); itSeed != seeds.end(); ++itSeed) {
    iSeeds++;
    debug() << "Seed num: " << iSeeds << endmsg;
    auto seedCell = *itSeed;
    auto seedId = seedCell.cellId();
    debug() << "Seeds Cell id :          " << seedId << endmsg;
    seedCell.core().bits = 0;  // set cell Bits to 0 for seed cell

    auto cellInCluster = clusterOfCell.find(seedId);
    if (cellInCluster != clusterOfCell.end()) {
      debug() << "Seed is already assigned to another cluster!" << endmsg;
      continue;
    } else {
      // new cluster starts with seed
      preClusterCollection[iSeeds].push_back(seedCell);
      clusterOfCell[seedId] = iSeeds;

      uint clusterId = iSeeds;
      // int maxN = neighboursMap.find(seedId)->second.size();
      std::vector<std::vector<uint64_t>> N2(100);
      std::vector<uint64_t> N1 = CombinedCaloTopoCluster::searchForNeighbours(
									      seedId, clusterId, neighboursMap, neighbourThr, allCells, clusterOfCell, preClusterCollection);
      N2[0] = N1;
      debug() << "Found " << N2[0].size() << " neighbours.." << endmsg;
      for (int it = 1; it < 100; it++) {
        for (auto& id : N2[it - 1]) {
          N2[it] = CombinedCaloTopoCluster::searchForNeighbours(id, clusterId, neighboursMap, neighbourThr, allCells,
                                                                clusterOfCell, preClusterCollection);
	  if (N2[it].size() == 0) break;
	}
	debug() << "Found " << N2[it].size() << " more neighbours.." << endmsg;
      }
    }
  }
}

std::vector<uint64_t>
CombinedCaloTopoCluster::searchForNeighboursWNoise(const uint64_t id,
                                                   uint& clusterNum,
                                                   const std::unordered_map<uint64_t, std::vector<uint64_t>>
						   neighboursMap,
                                                   double noiseSigma,
                                                   std::map<uint64_t, fcc::CaloHit>& allCells,
                                                   std::map<uint64_t, uint>& clusterOfCell,
                                                   std::map<uint, std::vector<fcc::CaloHit>>& preClusterCollection,
                                                   IReadNoiseFileTool& aNoiseTool) {
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
	auto neighbouringCell = itAllCells->second;
	// retrieve the cell noise level
	noiseSigma = aNoiseTool.getNoiseConstantPerCell(neighbouringCell.cellId());
	// if by 2 Sigma above noise level
	if (neighbouringCell.core().energy > noiseSigma * 2) {
	  // retrieve the cell
	  // add neighbour to cells for cluster
	  neighbouringCell.core().bits = 1;  // set Bits to 1 for neighbour cell
	  preClusterCollection[clusterNum].push_back(neighbouringCell);
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
	clusterOfCell[i.cellId()] = addFoundNeighboursToCluster;
	bool found = false;
	// make sure that already assigned cells are not added
	for (auto& j : preClusterCollection[addFoundNeighboursToCluster]) {
	  if (j.cellId() == i.cellId()) found = true;
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
std::vector<uint64_t>
CombinedCaloTopoCluster::searchForNeighbours(const uint64_t id,
                                             uint& clusterNum,
                                             const std::unordered_map<uint64_t, std::vector<uint64_t>>
					     neighboursMap,
                                             double noiseSigma,
                                             std::map<uint64_t, fcc::CaloHit>& allCells,
                                             std::map<uint64_t, uint>& clusterOfCell,
                                             std::map<uint, std::vector<fcc::CaloHit>>& preClusterCollection) {
  // Fill vector to be returned, next ids for which neighbours are found
  std::vector<uint64_t> addedNeighbourIds;
  auto search = neighboursMap.find(id);
  if (search == neighboursMap.end()){ warning() << "Cannot find cellID in map to neighbours! " << endmsg;     
    addedNeighbourIds.resize(0);
    return addedNeighbourIds;}
  else {
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
	auto neighbouringCell = itAllCells->second;
	// by 2 Sigma above noise level
	if (neighbouringCell.core().energy > noiseSigma * 2) {
	  // retrieve the cell
	  // add neighbour to cells for cluster
	  neighbouringCell.core().bits = 1;  // set Bits to 1 for neighbour cell
	  preClusterCollection[clusterNum].push_back(neighbouringCell);
	  clusterOfCell[neighbourID] = clusterNum;
	  addedNeighbourIds.push_back(neighbourID);
	}
      }
      // neighbour is assigned to a different cluster
      else if (itAllUsedCells != clusterOfCell.end() && itAllUsedCells->second != clusterNum) {
	auto clusterNumToMerge = itAllUsedCells->second;
	debug() << "This neighbour was found in cluster " << clusterNumToMerge << ", mark cluster " << clusterNum
		<< " to be merged!" << endmsg;
	// Mark cluster to be removed and set the next neighbours to be assigned to found cluster
	rmCluster = true;
	addFoundNeighboursToCluster = clusterNumToMerge;
      }
    }
    // If the current cluster was assigned to another it is removed from the map
    if (rmCluster) {
      debug() << "Assigning all cells ( " << preClusterCollection.find(clusterNum)->second.size() << " ) to Cluster "
	      << addFoundNeighboursToCluster << " with ( "
	      << preClusterCollection.find(addFoundNeighboursToCluster)->second.size() << " ). " << endmsg;
      // Fill all cells into cluster, and assigned cells to new cluster
      for (auto& i : preClusterCollection.find(clusterNum)->second) {
	clusterOfCell[i.cellId()] = addFoundNeighboursToCluster;
	bool found = false;
	// make sure that already assigned cells are not added
	for (auto& j : preClusterCollection[addFoundNeighboursToCluster]) {
	  if (j.cellId() == i.cellId()) found = true;
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

StatusCode CombinedCaloTopoCluster::finalize() { return GaudiAlgorithm::finalize(); }
