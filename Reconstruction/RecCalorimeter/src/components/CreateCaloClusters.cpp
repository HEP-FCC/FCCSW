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
#include "datamodel/MCParticleCollection.h"

DECLARE_COMPONENT(CreateCaloClusters)

CreateCaloClusters::CreateCaloClusters(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("clusters", m_clusters, "Input clusters (input)");
  declareProperty("genParticles", m_genParticles, "Input gen particles (input)");
  declareProperty("outClusters", m_newClusters, "Output clusters (output)");
  declareProperty("outCells", m_newCells, "Output cells (output)");
  declareProperty("positionsECalTool", m_cellPositionsECalTool,
                  "Handle for tool to retrieve cell positions in ECal");
  declareProperty("positionsHCalTool", m_cellPositionsHCalTool,
                  "Handle for tool to retrieve cell positions in HCal");
  declareProperty("positionsHCalNoSegTool", m_cellPositionsHCalNoSegTool,
                  "Handle for tool to retrieve cell positions in HCal w/o eta-phi segmentation");
 
  declareProperty("calibrate", m_doCalibration, "Clusters are going to be calibrated");
  declareProperty("cryoCorrection", m_doCryoCorrection, "Correction of lost energy between E and HCal");
  declareProperty("ehECal", m_ehECal, "e/h of the ECal");
  declareProperty("ehHCal", m_ehHCal, "e/h of the HCal");
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
  m_totEnergy = new TH1F("totalEnergy", "total energy in all clusters per event",  5000, 0, 5 );
  if (m_histSvc->regHist("/rec/totEnergy", m_totEnergy).isFailure()) {
    error() << "Couldn't register hist of total energy" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_totCalibEnergy = new TH1F("totalCalibEnergy", "total energy in all clusters after calobration per event",  5000, 0, 5 );
  if (m_histSvc->regHist("/rec/totCalibEnergy", m_totCalibEnergy).isFailure()) {
    error() << "Couldn't register hist of total energy after calibration" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_totBenchmarkEnergy = new TH1F("totBenchmarkEnergy", "total energy in all clusters after calobration and correction for lost energy in cryostat per event",  5000, 0, 5 );
  if (m_histSvc->regHist("/rec/totBenchmarkEnergy", m_totBenchmarkEnergy).isFailure()) {
    error() << "Couldn't register hist of total energy after calibration and cryo correction" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_clusterEnergy = new TH1F("clusterEnergy", "energy of cluster",  20100, -100, 20000 );
  if (m_histSvc->regHist("/rec/clusterEnergy", m_clusterEnergy).isFailure()) {
    error() << "Couldn't register hist" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_sharedClusterEnergy = new TH1F("sharedClusterEnergy", "energy in shared clusters per event",  20100, -100, 20000 );
  if (m_histSvc->regHist("/rec/sharedClusterEnergy", m_sharedClusterEnergy).isFailure()) {
    error() << "Couldn't register hist of energy in shared clusters per event" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_clusterEnergyCalibrated = new TH1F("clusterEnergyCalibrated", "energy of calibrated cluster",  20100, -100, 20000 );
  if (m_histSvc->regHist("/rec/clusterEnergyCalibrated", m_clusterEnergyCalibrated).isFailure()) {
    error() << "Couldn't register hist" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_clusterEnergyBenchmark = new TH1F("clusterEnergyBenchmark", "energy of calibrated and energy loss corrected cluster",  20100, -100, 20000 );
  if (m_histSvc->regHist("/rec/clusterEnergyBenchmark", m_clusterEnergyBenchmark).isFailure()) {
    error() << "Couldn't register hist" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_nCluster = new TH1F("nCluster", "number of cluster",  20000, 0, 20000 );
  if (m_histSvc->regHist("/rec/nCluster", m_nCluster).isFailure()) {
    error() << "Couldn't register hist" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_nCluster_1GeV = new TH1F("nCluster_1GeV", "number of cluster with energy > 1GeV",  20000, 0, 20000 );
  if (m_histSvc->regHist("/rec/nCluster_1GeV", m_nCluster_1GeV).isFailure()) {
    error() << "Couldn't register hist" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_nCluster_halfTrueEnergy = new TH1F("nCluster_halfTrueEnergy", "number of cluster with energy > Etrue/2",  20000, 0, 20000 );
  if (m_histSvc->regHist("/rec/nCluster_halfTrueEnergy", m_nCluster_halfTrueEnergy).isFailure()) {
    error() << "Couldn't register hist" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_energyCalibCluster_1GeV = new TH1F("energyCalibCluster_1GeV", "energy of calibrated cluster with energy > 1GeV",  20100, -10, 20000 );
  if (m_histSvc->regHist("/rec/energyCalibCluster_1GeV", m_energyCalibCluster_1GeV).isFailure()) {
    error() << "Couldn't register hist" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_energyCalibCluster_halfTrueEnergy = new TH1F("energyCalibCluster_halfTrueEnergy", "energy of calibrated cluster with energy > Etrue/2",  20100, -100, 20000 );
  if (m_histSvc->regHist("/rec/energyCalibCluster_halfTrueEnergy", m_energyCalibCluster_halfTrueEnergy).isFailure()) {
    error() << "Couldn't register hist" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_fractionEMcluster = new TH1F("fractionEMcluster", "fraction of EM cluster", 11, -0.05, 1.05 );
  if (m_histSvc->regHist("/rec/fractionEMcluster", m_fractionEMcluster).isFailure()) {
    error() << "Couldn't register hist" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_benchmark = new TH1F("benchmark", "added energy due to benchmark correction", 20000, 0, 20000 );
  if (m_histSvc->regHist("/rec/benchmark", m_benchmark).isFailure()) {
    error() << "Couldn't register hist" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_energyScale = new TH1F("energyScale", "energy scale of cluster", 3, 0., 3. );
  if (m_histSvc->regHist("/rec/energyScale", m_energyScale).isFailure()) {
    error() << "Couldn't register hist" << endmsg;
    return StatusCode::FAILURE;
  } 
  m_energyScaleVsClusterEnergy = new TH2F("energyScaleVsClusterEnergy", "energy scale of cluster versus energy of cluster", 3, 0., 3. , 20000, 0, 20000 ); 
  if (m_histSvc->regHist("/rec/energyScaleVsClusterEnergy", m_energyScaleVsClusterEnergy).isFailure()) {
    error() << "Couldn't register 2D hist" << endmsg;
    return StatusCode::FAILURE;
  }

  m_decoderECal = m_geoSvc->lcdd()->readout(m_readoutECal).idSpec().decoder();  
  m_decoderHCal = m_geoSvc->lcdd()->readout(m_readoutHCal).idSpec().decoder();  
 
  info() << "CreateCaloClusters initialized" << endmsg;
  
  return StatusCode::SUCCESS;
}

StatusCode CreateCaloClusters::execute() {
  // Get the input collection with Geant4 hits
  const fcc::CaloClusterCollection* clusters = m_clusters.get();
  debug() << "Input Cluster collection size: " << clusters->size() << endmsg;
  const fcc::MCParticleCollection* particles = m_genParticles.get();
  debug() << "Input genParticle collection size: " << particles->size() << endmsg;

  double Etruth = 0.;

  for (auto& iparticle : *particles){
    Etruth += sqrt(pow(iparticle.core().p4.mass,2)+pow(iparticle.core().p4.px,2)+pow(iparticle.core().p4.py,2)+pow(iparticle.core().p4.pz,2));
  }
  info() << "Truth energy of particle : " << std::floor(Etruth) << endmsg;

  // Output collections
  auto edmClusters = m_newClusters.createAndPut();
  auto edmClusterCells = m_newCells.createAndPut(); // new fcc::CaloHitCollection();

  int sharedClusters = 0;
  int clustersEM = 0;
  int clustersHad = 0;

  int nClusters_1GeV = 0;
  int nClusters_halfTrueEnergy = 0;

  float totClusterEnergy = 0.;
  float totCalibClusterEnergy = 0.;
  float totBenchmarkEnergy = 0.;
  float totBenchmarkCorr = 0.;

  if(m_doCalibration) { 
    for (auto& cluster : *clusters) {
      m_clusterEnergy->Fill(cluster.core().energy);
      // 1. Identify clusters with cells in different sub-systems
      bool cellsInBoth = false;
      std::map<uint,double> energyBoth;
      // sum energies in E and HCal layers for benchmark correction
      double energyLastECal = 0.;
      double energyFirstHCal = 0.;
      double lastBenchmarkTerm = 0.;
      if (cluster.core().energy > 1){
	nClusters_1GeV++;
	m_energyCalibCluster_1GeV->Fill(cluster.core().energy);
      }
      if (cluster.core().energy > Etruth/2.){
	nClusters_halfTrueEnergy++;
	m_energyCalibCluster_halfTrueEnergy->Fill(cluster.core().energy);
      }
      // Loop over cluster cells 
      for (uint it = 0; it < cluster.hits_size(); it++){
	dd4hep::DDSegmentation::CellID cID = cluster.hits(it).core().cellId;
	auto cellEnergy = cluster.hits(it).core().energy;
	uint systemId = m_decoder->get(cID, "system");
	int layerId;
	if (systemId == m_systemIdECal){
	  lastBenchmarkTerm += cellEnergy;
	  layerId = m_decoderECal->get(cID,"layer");
	  if( layerId == m_lastECalLayer ) 
	    energyLastECal += cellEnergy;
	}
	else if  (systemId == m_systemIdHCal){
	  layerId = m_decoderHCal->get(cID,"layer");
	  if ( layerId == m_firstHCalLayer )
	    energyFirstHCal += cellEnergy;
	}
	energyBoth[systemId] += cellEnergy;
      }
      // Define clusters that span over EM and hadronic part
      if (energyBoth[m_systemIdHCal] > 1e-3 &&  energyBoth[m_systemIdECal] > 1e-3){
	cellsInBoth = true;
      }
      // if chosed benchmark reco (doCryoCorrection), all clusters run through 2. step
      if (m_doCryoCorrection)
	cellsInBoth = true;

      // check if cluster energy is equal to sum over cells
      if (static_cast<int>(cluster.core().energy*100.0) != static_cast<int>((energyBoth[m_systemIdECal] + energyBoth[m_systemIdHCal])*100.0))
	warning() << "The cluster energy is not equal to sum over cell energy: " << cluster.core().energy << ", " << (energyBoth[m_systemIdECal] + energyBoth[m_systemIdHCal]) << endmsg;
      
      // 2. Calibrate the cluster if it contains cells in both systems
      if(cellsInBoth) {
	sharedClusters ++;
	m_sharedClusterEnergy->Fill(cluster.core().energy);
	// Calculate the fraction of energy in ECal
	auto energyFraction = energyBoth[m_systemIdECal] / cluster.core().energy;
	debug() << "Energy fraction in ECal : " << energyFraction << endmsg;
	// calibrate ECal cells to hadron scale
	// assuming ECal cells are calibrated to EM scale
	energyBoth[m_systemIdECal] = energyBoth[m_systemIdECal] * m_ehECal;
	bool calibECal = true;
	clustersHad++;
	m_energyScale->Fill(1);
	m_energyScaleVsClusterEnergy->Fill(1.,cluster.core().energy);
	totClusterEnergy += cluster.core().energy;
	
	// Building new calibrated cluster
	fcc::CaloCluster newCluster;
	double posX = 0.;
	double posY = 0.;
	double posZ = 0.;
	double energy = 0.;
	// Add cells to cluster
	for (uint it = 0; it < cluster.hits_size(); it++){
	  fcc::CaloHit newCell;
	  
	  auto cellId = cluster.hits(it).core().cellId;
	  auto cellEnergy = cluster.hits(it).core().energy;
	  
	  newCell.core().cellId = cellId;
	  newCell.core().bits = cluster.hits(it).core().bits;
	  
	  uint systemId = m_decoder->get(cellId, "system");
	  
	  dd4hep::Position posCell;
	  if (systemId == m_systemIdECal){  // ECAL system id
	    posCell = m_cellPositionsECalTool->xyzPosition(cellId);
	    if ( m_doCryoCorrection ) // cluster on hadron scale, apply cryo correction
	      cellEnergy = cellEnergy * m_a1;
	    else if ( calibECal && !m_doCryoCorrection ) // cluster on hadron scale
	      cellEnergy = cellEnergy * m_ehECal;
	  }
	  else if (systemId == m_systemIdHCal){  // HCAL system id
	    if (m_noSegmentationHCal)
	      posCell = m_cellPositionsHCalNoSegTool->xyzPosition(cellId);
	    else
	      posCell = m_cellPositionsHCalTool->xyzPosition(cellId);
	    if ( !calibECal && !m_doCryoCorrection)
	      cellEnergy = cellEnergy * (1/m_ehHCal);
	    else if ( m_doCryoCorrection )
	      cellEnergy = cellEnergy;
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

	newCluster.core().position.x = posX / energy;
	newCluster.core().position.y = posY / energy;
	newCluster.core().position.z = posZ / energy;
        // setting the deltaR determined on EM scale
	newCluster.core().time = cluster.time();

	// Correct for lost energy in cryostat
	if ( m_doCryoCorrection ){
	  debug() << "Energy in last ECal + first HCal layer: " << (energyLastECal+energyFirstHCal) << "GeV " << endmsg;
	  debug() << "Energy in last ECal x first HCal layer: " << (energyLastECal*energyFirstHCal) << "GeV " << endmsg;
	  double corr = m_b1*sqrt(fabs(energyLastECal*m_a1*energyFirstHCal)) + m_c1*pow(lastBenchmarkTerm*m_a1, 2);
	  debug() << "Added energy to cluster  : " << corr << "GeV " << endmsg;
	  energy = energy + corr;
	  
	  totBenchmarkCorr += corr;
	  // Fill histogram with corrected energy
	  m_clusterEnergyBenchmark->Fill(energy);
	  totBenchmarkEnergy += energy;
	}
	newCluster.core().energy = energy;
	edmClusters->push_back(newCluster);
	// close loop over shared cluster
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
	// add the unchanged cluster energies 
	totClusterEnergy += newCluster.core().energy;
	totBenchmarkEnergy += newCluster.core().energy;
	totCalibClusterEnergy += newCluster.core().energy;
      }
      energyBoth.clear();
    } // closing the loop over all input cluster
  }
  info() << "Number of re-calibrated clusters      : " << sharedClusters << endmsg;
  if (sharedClusters > 0){
    info() << "Clusters calibrated to EM scale       : " << clustersEM/float(sharedClusters)*100 << " % " << endmsg;
    info() << "Clusters calibrated to hadron scale : " << clustersHad/float(sharedClusters)*100 << " % " << endmsg;
    m_fractionEMcluster->Fill(clustersEM/float(sharedClusters));
  }
  debug() << "Output Cluster collection size: " << edmClusters->size() << endmsg;

  m_totEnergy->Fill( totClusterEnergy/std::floor(Etruth) );
  m_totCalibEnergy->Fill( totCalibClusterEnergy/std::floor(Etruth) );
  m_totBenchmarkEnergy->Fill( totBenchmarkEnergy/std::floor(Etruth) );
  m_benchmark->Fill(totBenchmarkCorr);

  m_nCluster_1GeV->Fill(nClusters_1GeV);
  m_nCluster_halfTrueEnergy->Fill(nClusters_halfTrueEnergy);

  return StatusCode::SUCCESS;
}

StatusCode CreateCaloClusters::finalize() { 
  float allCluster = m_totEnergy->GetEntries();
  m_clusterEnergy->Scale(1/allCluster);
  m_sharedClusterEnergy->Scale(1/allCluster);
  m_clusterEnergyCalibrated->Scale(1/allCluster);
  m_clusterEnergyBenchmark->Scale(1/allCluster);
  m_energyScale->Scale(1/allCluster);

return GaudiAlgorithm::finalize(); }
