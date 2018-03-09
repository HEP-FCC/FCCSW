#include "CreateCaloClusters.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

// DD4hep
#include "DD4hep/Detector.h"

// our EDM
#include "datamodel/CaloHit.h"
#include "datamodel/CaloHitCollection.h"

DECLARE_ALGORITHM_FACTORY(CreateCaloClusters)

CreateCaloClusters::CreateCaloClusters(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("clusters", m_clusters, "Clusters on which to add pile-up noise (input)");

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
  int sharedClusters = 0;

  if(m_doCalibration) { 
    for (const auto& cluster : clusters) {
      // 1. Identify clusters with cells in different sub-systems
      bool cellsInBoth = false;
      float cellSystem = 0;
      std::map<uint,double> energyBoth;
      for (uint it = 0; it < cluster.hits_size(); it++){
	auto cellId = cluster.hits(it).core().cellId;
	auto cellEnergy = cluster.hits(it).core().energy;
	m_decoder->SetValue(cellId);
	int systemId = (*m_decoder)["system"].value();
	
	if (systemId != cellSystem && cellSystem!=0){
	  cellsInBoth = true;
	}
	cellSystem = systemId;
	
	energyBoth[systemId] += cellEnergy;
      }
      
      // check if cluster energy is equal to sum over cells
      if (cluster.core().energy != (energyBoth[m_systemIdECal] + energyBoth[m_systemIdHCal]))
	warning() << "The cluster energy is not equal to sum over cell energy: " << cluster.core().energy << ", " << (energyBoth[m_systemIdECal] + energyBoth[m_systemIdHCal]) << endmsg;
      
      // 2. Calibrate the cluster if it contains cells in both systems
      if(cellsInBoth) {
	// Calculate the fraction of energy in ECal
	auto energyFraction = energyBoth[m_systemIdECal] / cluster.core().energy;
	if (energyFraction >= m_fractionECal) {
	  // calibrate HCal cells to EM scale
	  // assuming HCal cells are calibrated to hadron scale
	  energyBoth[m_systemIdHCal] = energyBoth[m_systemIdHCal] / m_ehHCal;
	}
	else {
	  // calibrate ECal cells to hadron scale
	  // assuming ECal cells are calibrated to EM scale
	  energyBoth[m_systemIdECal] = energyBoth[m_systemIdECal] * m_ehECal;
	}
	// Set the cluster energy to sum of energy in E and HCal
	cluster.core().energy =  energyBoth[m_systemIdECal] + energyBoth[m_systemIdHCal];
      }
    }
  }

  // push the CaloHitCollection to event store
  //m_cells.put(edmClustersCollection);
  //debug() << "Output Cell collection size: " << edmClustersCollection->size() << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode CreateCaloClusters::finalize() { return GaudiAlgorithm::finalize(); }
