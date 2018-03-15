#include "CreateCaloClusters.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

// DD4hep
#include "DD4hep/Detector.h"

#include "TH1F.h"
#include "TH2F.h"

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
  declareProperty("cryoCorrection", m_doCryoCorrection, "Correction of lost energy between E and HCal");
  declareProperty("ehECal", m_ehECal, "e/h of the ECal");
  declareProperty("ehHCal", m_ehHCal, "e/h of the HCal");

  declareProperty("addPileupNoise", m_addNoise, "Add pile up noise?");
  declareProperty("noiseECalTool", m_noiseECalTool, "Handle for the cells noise tool of ECal");
  declareProperty("noiseHCalTool", m_noiseHCalTool, "Handle for the cells noise tool of HCal");
}

StatusCode CreateCaloClusters::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry service." << endmsg;
    return StatusCode::FAILURE;
  }
  
  // Histogram service
  if (service("THistSvc", m_histSvc).isFailure()) {
    error() << "Unable to locate Histogram Service" << endmsg;
    return StatusCode::FAILURE;
  }
  m_totEnergy = new TH1F("totalEnergy", "total energy in all clusters per event",  10000, 0, 10000 );
  if (m_histSvc->regHist("/rec/totEnergy", m_totEnergy).isFailure()) {
    error() << "Couldn't register hist of total energy" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_totCalibEnergy = new TH1F("totalCalibEnergy", "total energy in all clusters after calobration per event",  10000, 0, 10000 );
  if (m_histSvc->regHist("/rec/totCalibEnergy", m_totCalibEnergy).isFailure()) {
    error() << "Couldn't register hist of total energy after calibration" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_totBenchmarkEnergy = new TH1F("totBenchmarkEnergy", "total energy in all clusters after calobration and correction for lost energy in cryostat per event",  10000, 0, 10000 );
  if (m_histSvc->regHist("/rec/totBenchmarkEnergy", m_totBenchmarkEnergy).isFailure()) {
    error() << "Couldn't register hist of total energy after calibration and cryo correction" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_clusterEnergy = new TH1F("clusterEnergy", "energy of cluster",  10000, 0, 10000 );
  if (m_histSvc->regHist("/rec/clusterEnergy", m_clusterEnergy).isFailure()) {
    error() << "Couldn't register hist" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_clusterEnergyCalibrated = new TH1F("clusterEnergyCalibrated", "energy of calibrated cluster",  10000, 0, 10000 );
  if (m_histSvc->regHist("/rec/clusterEnergyCalibrated", m_clusterEnergyCalibrated).isFailure()) {
    error() << "Couldn't register hist" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_clusterEnergyBenchmark = new TH1F("clusterEnergyBenchmark", "energy of calibrated and energy loss corrected cluster",  10000, 0, 10000 );
  if (m_histSvc->regHist("/rec/clusterEnergyBenchmark", m_clusterEnergyBenchmark).isFailure()) {
    error() << "Couldn't register hist" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_energyScale = new TH1F("energyScale", "energy scale of cluster", 3, 0., 3. );
  if (m_histSvc->regHist("/rec/energyScale", m_energyScale).isFailure()) {
    error() << "Couldn't register hist" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_energyScaleVsClusterEnergy = new TH2F("energyScaleVsClusterEnergy", "energy scale of cluster versus energy of cluster", 3, 0., 3. , 10000, 0, 10000 ); 
  if (m_histSvc->regHist("/rec/energyScaleVsClusterEnergy", m_energyScaleVsClusterEnergy).isFailure()) {
    error() << "Couldn't register 2D hist" << endmsg;
    return StatusCode::FAILURE;
  }

  m_decoderECal = m_geoSvc->lcdd()->readout(m_readoutECal).idSpec().decoder();  
  m_decoderHCal = m_geoSvc->lcdd()->readout(m_readoutHCal).idSpec().decoder();  

 // Cell noise tool
  if (m_addNoise) {
    if (!m_noiseECalTool.retrieve()) {
      error() << "Unable to retrieve the ECal noise tool!!!" << endmsg;
      return StatusCode::FAILURE;
    }
    if (!m_noiseHCalTool.retrieve()) {
      error() << "Unable to retrieve the HCal noise tool!!!" << endmsg;
      return StatusCode::FAILURE;
    }
  }
 
  info() << "CreateCaloClusters initialized" << endmsg;
  
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

  float totClusterEnergy = 0.;
  float totCalibClusterEnergy = 0.;
  float totBenchmarkEnergy = 0.;

  if(m_doCalibration) { 
    for (auto& cluster : *clusters) {
      // 1. Identify clusters with cells in different sub-systems
      bool cellsInBoth = false;
      std::map<uint,double> energyBoth;
      // sum energies in E and HCal layers for benchmark correction
      double energyLastECal = 0.;
      double energyFirstHCal = 0.;

      // Loop over cluster cells 
      for (uint it = 0; it < cluster.hits_size(); it++){
	auto cellId = cluster.hits(it).core().cellId;
	auto cellEnergy = cluster.hits(it).core().energy;
	m_decoder->setValue(cellId);
	uint systemId = (*m_decoder)["system"].value();
	int layerId;
	if (systemId == m_systemIdECal){
	  layerId = (*m_decoderECal)["layer"].value();
	  if( layerId == m_lastECalLayer) 
	    energyLastECal += cellEnergy;
	}
	else{
	  layerId = (*m_decoderHCal)["layer"].value();
	  if ( layerId == m_firstHCalLayer)
	    energyFirstHCal += cellEnergy;
	}
	energyBoth[systemId] += cellEnergy;
      }
      
      if (energyBoth.size() > 1){
	cellsInBoth = true;
	// Fill histogram with un-calibrated energy
	m_clusterEnergy->Fill(cluster.core().energy);
      }
      
      // check if cluster energy is equal to sum over cells
      if (static_cast<int>(cluster.core().energy*100.0) != static_cast<int>((energyBoth[m_systemIdECal] + energyBoth[m_systemIdHCal])*100.0))
	warning() << "The cluster energy is not equal to sum over cell energy: " << cluster.core().energy << ", " << (energyBoth[m_systemIdECal] + energyBoth[m_systemIdHCal]) << endmsg;
      
      // 2. Calibrate the cluster if it contains cells in both systems
      if(cellsInBoth) {
	sharedClusters ++;
	// Calculate the fraction of energy in ECal
	auto energyFraction = energyBoth[m_systemIdECal] / cluster.core().energy;
	debug() << "Energy fraction in ECal : " << energyFraction << endmsg;
	bool calibECal = false;
	if (energyFraction >= m_fractionECal) {
	  // calibrate HCal cells to EM scale
	  // assuming HCal cells are calibrated to hadron scale
	  energyBoth[m_systemIdHCal] = energyBoth[m_systemIdHCal] / m_ehHCal;
	  clustersEM++;
	  m_energyScale->Fill(0);
	  m_energyScaleVsClusterEnergy->Fill(0.,cluster.core().energy);
	  totClusterEnergy += cluster.core().energy;
	}
	else {
	  // calibrate ECal cells to hadron scale
	  // assuming ECal cells are calibrated to EM scale
	  energyBoth[m_systemIdECal] = energyBoth[m_systemIdECal] * m_ehECal;
	  calibECal = true;
	  clustersHad++;
	  m_energyScale->Fill(1);
	  m_energyScaleVsClusterEnergy->Fill(1.,cluster.core().energy);
	  totClusterEnergy += cluster.core().energy;
	}
	// Create a new cluster
	fcc::CaloCluster newCluster;
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
	  uint systemId = (*m_decoder)["system"].value();
	  
	  dd4hep::Position posCell;
	  if (systemId == m_systemIdECal){  // ECAL system id
	    posCell = m_cellPositionsECalTool->xyzPosition(cellId);
	    if (calibECal)
	      cellEnergy = cellEnergy / m_ehECal;
	    if ( m_addNoise ) {
	      // Scale the cell energy by factor determined from he correlated pile-up studies
	      cellEnergy = cellEnergy * m_noiseECalTool->getNoiseConstantPerCell(cellId);
	    }
	  }
	  else if (systemId == m_systemIdHCal){  // HCAL system id
	    posCell = m_cellPositionsHCalTool->xyzPosition(cellId);
	    if (!calibECal)
	      cellEnergy = cellEnergy * m_ehHCal;
	    if ( m_addNoise ) {
	      // Scale the cell energy by factor determined from he correlated pile-up studies
	      cellEnergy = cellEnergy * m_noiseHCalTool->getNoiseConstantPerCell(cellId);
	    }
	  }

	  newCell.core().energy = cellEnergy;
	  posX += posCell.X() * cellEnergy;
	  posY += posCell.Y() * cellEnergy;
	  posZ += posCell.Z() * cellEnergy;
	  newCluster.addhits(newCell);
	  edmClusterCells->push_back(newCell);
	  energy += cellEnergy;
	}
	// Fill histogram with calibrated energy
	m_clusterEnergyCalibrated->Fill(energy);
	totCalibClusterEnergy += energy;

	// Correct for lost energy in cryostat
	if ( m_doCryoCorrection ){
	  double corr = m_b*sqrt(fabs(energyLastECal*m_a*energyFirstHCal));
	  energy += corr;
	  
	  // Fill histogram with corrected energy
	  m_clusterEnergyBenchmark->Fill(energy);
	  totBenchmarkEnergy += energy;
	}

	newCluster.core().energy = energy;
	newCluster.core().position.x = posX / energy;
	newCluster.core().position.y = posY / energy;
	newCluster.core().position.z = posZ / energy;
	edmClusters->push_back(newCluster);
      }
      else { // Fill the unchanged cluster in output collection
	auto newCluster = cluster.clone();
	for (uint it = 0; it <cluster.hits_size(); it++){
	  auto newCell = edmClusterCells->create();
	  auto cellId = cluster.hits(it).core().cellId;
	  auto cellEnergy = cluster.hits(it).core().energy;
	  m_decoder->setValue(cellId);
	  uint systemId = (*m_decoder)["system"].value();
	  if ( m_addNoise ) {
	    // Scale the cell energy by factor determined from he correlated pile-up studies
	    if(systemId == m_systemIdECal)
	      cellEnergy = cellEnergy * m_noiseECalTool->getNoiseConstantPerCell(cellId);
	    else 
	      cellEnergy = cellEnergy * m_noiseHCalTool->getNoiseConstantPerCell(cellId);
	  }
	  newCell.core().energy = cellEnergy;
	  newCell.core().cellId = cellId;
	  newCell.core().bits = cluster.hits(it).core().bits;
	  newCluster.addhits(newCell);
	}
	edmClusters->push_back(newCluster);
	totBenchmarkEnergy += newCluster.core().energy;
	totCalibClusterEnergy += newCluster.core().energy;
      }
    }
  }
  info() << "Number of re-calibrated clusters      : " << sharedClusters << endmsg;
  if (sharedClusters > 0){
    info() << "Clusters calibrated to EM scale       : " << clustersEM/float(sharedClusters)*100 << " % " << endmsg;
    info() << "Clusters calibrated to hadron scale : " << clustersHad/float(sharedClusters)*100 << " % " << endmsg;
  }
  debug() << "Output Cluster collection size: " << edmClusters->size() << endmsg;

  m_totEnergy->Fill( totClusterEnergy );
  m_totCalibEnergy->Fill( totCalibClusterEnergy );
  m_totBenchmarkEnergy->Fill( totBenchmarkEnergy );
  return StatusCode::SUCCESS;
}

StatusCode CreateCaloClusters::finalize() { 
  float allCalibCluster = m_clusterEnergy->GetEntries();
  m_clusterEnergy->Scale(1/allCalibCluster);
  m_clusterEnergyCalibrated->Scale(1/allCalibCluster);
  m_clusterEnergyBenchmark->Scale(1/allCalibCluster);
  m_energyScale->Scale(1/allCalibCluster);

return GaudiAlgorithm::finalize(); }
