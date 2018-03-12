#include "CreateCaloClusters.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

// DD4hep
#include "DD4hep/Detector.h"

// datamodel
#include "datamodel/CaloCluster.h"
#include "datamodel/CaloClusterCollection.h"
#include "datamodel/CaloHit.h"
#include "datamodel/CaloHitCollection.h"

DECLARE_ALGORITHM_FACTORY(CreateCaloClusters)

CreateCaloClusters::CreateCaloClusters(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("clusters", m_clusters, "Input clusters (input)");
  declareProperty("outClusters", m_newClusters, "Output clusters (output)");
  declareProperty("outCells", m_newCells, "Output cells (output)");
  declareProperty("positionsECalTool", m_cellPositionsECalTool,
                  "Handle for tool to retrieve cell positions in ECal");
  declareProperty("positionsHCalTool", m_cellPositionsHCalTool,
                  "Handle for tool to retrieve cell positions in HCal");
 
  declareProperty("calibrate", m_doCalibration, "Clusters are going to be calibrated");
  declareProperty("ehECal", m_ehECal, "e/h of the ECal");
  declareProperty("ehHCal", m_ehHCal, "e/h of the HCal");

//  declareProperty("ECalBarrelNoiseTool", m_ecalBarrelNoiseTool, "Handle for the cells noise tool of Barrel ECal");
//  declareProperty("HCalBarrelNoiseTool", m_hcalBarrelNoiseTool, "Handle for the cells noise tool of Barrel HCal");
}

StatusCode CreateCaloClusters::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  info() << "CreateCaloClusters initialized" << endmsg;

  // // Pile-up noise tool
  // if (!m_ecalBarrelNoiseTool.retrieve() || !m_hcalBarrelNoiseTool.retrieve() ) {
  //   error() << "Unable to retrieve the calo clusters noise tool!!!" << endmsg;
  //   return StatusCode::FAILURE;
  //  }
  return StatusCode::SUCCESS;
}

StatusCode CreateCaloClusters::execute() {
  // Get the input collection with Geant4 hits
  const fcc::CaloClusterCollection* clusters = m_clusters.get();
  debug() << "Input Cluster collection size: " << clusters->size() << endmsg;
  // Output collections
  auto edmClusters = m_newClusters.createAndPut();
  auto edmClusterCells = m_newCells.createAndPut(); // new fcc::CaloHitCollection();

  int sharedClusters = 0;
  int clustersEM = 0;
  int clustersHad = 0;

  if(m_doCalibration) { 
    for (auto& cluster : *clusters) {
      // 1. Identify clusters with cells in different sub-systems
      bool cellsInBoth = false;
      float cellSystem = 0;
      std::map<uint,double> energyBoth;
      for (uint it = 0; it < cluster.hits_size(); it++){
	auto cellId = cluster.hits(it).core().cellId;
	auto cellEnergy = cluster.hits(it).core().energy;
	m_decoder->setValue(cellId);
	int systemId = (*m_decoder)["system"].value();
	
	if (systemId != cellSystem && cellSystem!=0){
	  cellsInBoth = true;
	  break;
	}
	cellSystem = systemId;
	
	energyBoth[systemId] += cellEnergy;
      }
      
      // check if cluster energy is equal to sum over cells
      if (static_cast<int>(cluster.core().energy*1000.0) != static_cast<int>((energyBoth[m_systemIdECal] + energyBoth[m_systemIdHCal])*1000.0))
	warning() << "The cluster energy is not equal to sum over cell energy: " << cluster.core().energy << ", " << (energyBoth[m_systemIdECal] + energyBoth[m_systemIdHCal]) << endmsg;
      
      // 2. Calibrate the cluster if it contains cells in both systems
      if(cellsInBoth) {
	sharedClusters ++;
	// Calculate the fraction of energy in ECal
	auto energyFraction = energyBoth[m_systemIdECal] / cluster.core().energy;
	bool calibECal = false;
	if (energyFraction >= m_fractionECal) {
	  // calibrate HCal cells to EM scale
	  // assuming HCal cells are calibrated to hadron scale
	  energyBoth[m_systemIdHCal] = energyBoth[m_systemIdHCal] / m_ehHCal;
	  clustersEM++;
	}
	else {
	  // calibrate ECal cells to hadron scale
	  // assuming ECal cells are calibrated to EM scale
	  energyBoth[m_systemIdECal] = energyBoth[m_systemIdECal] * m_ehECal;
	  calibECal = true;
	  clustersHad++;
	}
	// Create a new cluster
	fcc::CaloCluster cluster;
	double posX = 0.;
	double posY = 0.;
	double posZ = 0.;
	double energy = 0.;
 
	for (uint it = 0; it < cluster.hits_size(); it++){
	  fcc::CaloHit newCell;
	  
	  auto cellId = cluster.hits(it).core().cellId;
	  auto cellEnergy = cluster.hits(it).core().energy;
	  
	  newCell.core().cellId = cellId;
	  newCell.core().bits = cluster.hits(it).core().bits;
	  
	  m_decoder->setValue(cellId);
	  int systemId = (*m_decoder)["system"].value();
	  
	  dd4hep::Position posCell;
	  if (systemId == m_systemIdECal){  // ECAL system id
	    posCell = m_cellPositionsECalTool->xyzPosition(cellId);
	    if (calibECal)
	      cellEnergy = cellEnergy * m_ehECal;
	  }
	  else if (systemId == m_systemIdHCal){  // HCAL system id
	    posCell = m_cellPositionsHCalTool->xyzPosition(cellId);
	    if (!calibECal)
	      cellEnergy = cellEnergy / m_ehHCal;
	  }
	  newCell.core().energy = cellEnergy;
	  energy += cellEnergy;
	  posX += posCell.X() * cellEnergy;
	  posY += posCell.Y() * cellEnergy;
	  posZ += posCell.Z() * cellEnergy;
	  cluster.addhits(newCell);
	  edmClusterCells->push_back(newCell);
	}
	cluster.core().energy = energy;
	cluster.core().position.x = posX / energy;
	cluster.core().position.y = posY / energy;
	cluster.core().position.z = posZ / energy;
	edmClusters->push_back(cluster);
      }
      else { // Fill the unchanged cluster in output collection
	auto newCluster = cluster.clone();
	for (uint it = 0; it <cluster.hits_size(); it++){
	  auto newCell = edmClusterCells->create();
	  auto cellId = cluster.hits(it).core().cellId;
	  auto cellEnergy = cluster.hits(it).core().energy;
	  newCell.core().energy = cellEnergy;
	  newCell.core().cellId = cellId;
	  newCell.core().bits = cluster.hits(it).core().bits;
	  newCluster.addhits(newCell);
	}
	edmClusters->push_back(newCluster);
      }
    }
  }
  if (clusters->size() > 0)
    info() << "Fraction of re-calibrated clusters      : " << sharedClusters/clusters->size()*100 << " % " << endmsg;
  if (sharedClusters > 0){
    info() << "Fraction of clusters on EM scale       : " << clustersEM/sharedClusters*100 << " % " << endmsg;
    info() << "Fraction of clusters on hadron scale : " << clustersHad/sharedClusters*100 << " % " << endmsg;
  }
  debug() << "Output Cluster collection size: " << edmClusters->size() << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode CreateCaloClusters::finalize() { return GaudiAlgorithm::finalize(); }
