#include "CorrectCluster.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"
#include "DetSegmentation/FCCSWGridPhiEta.h"
#include "GaudiKernel/ITHistSvc.h"

// DD4hep
#include "DD4hep/Detector.h"
#include "DDSegmentation/MultiSegmentation.h"

// our EDM
#include "datamodel/CaloCluster.h"
#include "datamodel/CaloClusterCollection.h"
#include "datamodel/CaloHitCollection.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"

// Root
#include "TFile.h"
#include "TLorentzVector.h"
#include "TFitResult.h"
#include "TGraphErrors.h"

DECLARE_ALGORITHM_FACTORY(CorrectCluster)

CorrectCluster::CorrectCluster(const std::string& name, ISvcLocator* svcLoc)
    : GaudiAlgorithm(name, svcLoc),
      m_histSvc("THistSvc", "CorrectCluster"),
      m_geoSvc("GeoSvc", "CorrectCluster"),
      m_hEnergyPreAnyCorrections(nullptr),
      m_hEnergyPostAllCorrections(nullptr),
      m_hPileupEnergy(nullptr),
      m_hUpstreamEnergy(nullptr) {
  declareProperty("clusters", m_inClusters, "Input clusters (input)");
  declareProperty("correctedClusters", m_correctedClusters, "Corrected clusters (output)");
  declareProperty("particle", m_particle, "Generated single-particle event (input)");
  declareProperty("vertex", m_vertex, "Generated vertices (input)");
}

StatusCode CorrectCluster::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  int energyStart = 0;
  int energyEnd = 0;
  if( m_energy == 0 ) {
    energyStart = 0;
    energyEnd = 1000;
  } else {
    energyStart = 0;
    energyEnd = 5. * m_energy;
  }

  // create control histograms
  m_hEnergyPreAnyCorrections = new TH1F(
      "energyPreAnyCorrections", "Energy of cluster before any correction", 3000, energyStart, energyEnd);
  if (m_histSvc->regHist("/rec/energyPreAnyCorrections", m_hEnergyPreAnyCorrections).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_hEnergyPostAllCorrections = new TH1F(
      "energyPostAllCorrections", "Energy of cluster after all corrections", 3000, energyStart, energyEnd);
  if (m_histSvc->regHist("/rec/energyPostAllCorrections", m_hEnergyPostAllCorrections).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_hEnergyPostAllCorrectionsAndScaling = new TH1F(
      "energyPostAllCorrectionsAndScaling", "Energy of cluster after all corrections and scaling", 3000, energyStart, energyEnd);
  if (m_histSvc->regHist("/rec/energyPostAllCorrectionsAndScaling", m_hEnergyPostAllCorrectionsAndScaling).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_hEnergyFractionInLayers = new TH1F(
    "energyFractionInLayers", "Fraction of energy deposited in given layer", m_numLayers, 0.5, m_numLayers + 0.5);
  if (m_histSvc->regHist("/rec/energyFractionInLayers", m_hEnergyFractionInLayers).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_hPileupEnergy = new TH1F(
      "pileupCorrectionEnergy", "Energy added to a cluster as a correction for correlated noise", 1000, -10, 10);
  if (m_histSvc->regHist("/rec/pileupCorrectionEnergy", m_hPileupEnergy).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_hUpstreamEnergy = new TH1F(
    "upstreamCorrectionEnergy", "Energy added to a cluster as a correction for upstream material", 1000, -10, 10);
  if (m_histSvc->regHist("/rec/upstreamCorrectionEnergy", m_hUpstreamEnergy).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_hDiffEta = new TH1F("diffEta", "#eta resolution", 10 * ceil(2 * m_etaMax / m_dEta), - m_etaMax / 10., m_etaMax / 10.);
  if (m_histSvc->regHist("/rec/diffEta", m_hDiffEta).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_hDiffEtaResWeight = new TH1F("diffEtaResWeight", "#eta resolution", 10 * ceil(2 * m_etaMax / m_dEta), - m_etaMax / 10., m_etaMax / 10.);
  if (m_histSvc->regHist("/rec/diffEtaResWeight", m_hDiffEtaResWeight).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_hDiffEtaResWeight2point = new TH1F("diffEtaResWeight2point", "#eta resolution", 10 * ceil(2 * m_etaMax / m_dEta), - m_etaMax / 10., m_etaMax / 10.);
  if (m_histSvc->regHist("/rec/diffEtaResWeight2point", m_hDiffEtaResWeight2point).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  for (uint i = 0; i < m_numLayers; i++) {
    m_hDiffEtaLayer.push_back(new TH1F(("diffEtaLayer"+std::to_string(i)).c_str(), ("#eta resolution for layer "+std::to_string(i)).c_str(),
                                       10 * ceil(2 * m_etaMax / m_dEta), - m_etaMax / 10., m_etaMax / 10.));
    if (m_histSvc->regHist("/rec/diffEta_layer" + std::to_string(i), m_hDiffEtaLayer.back()).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  m_hEta = new TH1F("eta", "#eta", 1000, - m_etaMax, m_etaMax);
  if (m_histSvc->regHist("/rec/eta", m_hEta).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_hDiffPhi = new TH1F("diffPhi", "#varphi resolution", 10 * ceil(2 * m_phiMax / m_dPhi), - m_phiMax / 10., m_phiMax / 10.);
  if (m_histSvc->regHist("/rec/diffPhi", m_hDiffPhi).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_hPhi = new TH1F("phi", "#varphi", 1000, - m_phiMax, m_phiMax);
  if (m_histSvc->regHist("/rec/phi", m_hPhi).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_hNumCells = new TH1F("numCells", "number of cells", 2000, -0.5, 1999.5);
  if (m_histSvc->regHist("/rec/numCells", m_hNumCells).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_etaRecalcLayerWeights.size() < m_numLayers) {
    error() << "m_etaRecalcLayerWeights size is smaller than numLayers." << endmsg;
    return StatusCode::FAILURE;
  }
  for (uint iSys = 0; iSys < m_systemId.size(); iSys++) {
    // check if readouts exist
    if (m_geoSvc->lcdd()->readouts().find(m_readoutName[iSys]) == m_geoSvc->lcdd()->readouts().end()) {
      error() << "Readout <<" << m_readoutName[iSys] << ">> does not exist." << endmsg;
      return StatusCode::FAILURE;
    }
    // retrieve PhiEta segmentation
    m_segmentationPhiEta[m_systemId[iSys]] = dynamic_cast<dd4hep::DDSegmentation::FCCSWGridPhiEta*>(
        m_geoSvc->lcdd()->readout(m_readoutName[iSys]).segmentation().segmentation());
    m_segmentationMulti[m_systemId[iSys]] = dynamic_cast<dd4hep::DDSegmentation::MultiSegmentation*>(
        m_geoSvc->lcdd()->readout(m_readoutName[iSys]).segmentation().segmentation());
    if (m_segmentationPhiEta[m_systemId[iSys]] == nullptr && m_segmentationMulti[m_systemId[iSys]] == nullptr) {
      error() << "There is no phi-eta or multi- segmentation." << endmsg;
      return StatusCode::FAILURE;
    }
    m_decoder.insert (std::make_pair(m_systemId[iSys], m_geoSvc->lcdd()->readout(m_readoutName[iSys]).idSpec().decoder()));
  }
  // Initialize random service
  if (service("RndmGenSvc", m_randSvc).isFailure()) {
    error() << "Couldn't get RndmGenSvc!!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  m_gauss.initialize(m_randSvc, Rndm::Gauss(0., 1.));

  // open and check file, read the histograms with noise constants
  if (initNoiseFromFile().isFailure()) {
    error() << "Couldn't open file with noise constants!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  // calculate borders of eta bins:
  if (m_etaValues.size() != m_presamplerShiftP0.size() && m_etaValues.size() != m_presamplerShiftP1.size() &&
      m_etaValues.size() != m_presamplerScaleP0.size() && m_etaValues.size() != m_presamplerScaleP1.size()) {
    error() << "Sizes of parameter vectors for upstream energy correction should be the same" << endmsg;
    return StatusCode::FAILURE;
  }
  // if only one eta, leave border vector empty
  for (uint iEta = 1; iEta < m_etaValues.size(); iEta++) {
    m_etaBorders.push_back(m_etaValues[iEta - 1] + 0.5 * (m_etaValues[iEta] - m_etaValues[iEta - 1]));
  }
  // push values for the last eta bin, width as the last one
  if (m_etaValues.size() > 1) {
    m_etaBorders.push_back(m_etaValues[m_etaValues.size() - 1] +
                           0.5 * (m_etaValues[m_etaValues.size() - 1] - m_etaValues[m_etaValues.size() - 2]));
  } else {
    // high eta to ensure all particles fall below
    m_etaBorders.push_back(100);
  }
  // OPTIMISATION OF CLUSTER SIZE
  // sanity check
  if (!(m_nEtaFinal.size() == m_numLayers && m_nPhiFinal.size() == m_numLayers)) {
    error() << "Size of optimised window should be equal to number of layers: " << endmsg;
    error() << "Size of windows in eta:  " << m_nEtaFinal.size() << "\tsize of windows in phi:  " << m_nPhiFinal.size()
            << "number of layers:  " << m_numLayers << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_nEtaFinal.size() == m_numLayers) {
    for (uint iLayer = 0; iLayer < m_numLayers; iLayer++) {
      m_halfPhiFin.push_back(floor(m_nPhiFinal[iLayer]/2));
      m_halfEtaFin.push_back(floor(m_nEtaFinal[iLayer]/2));
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode CorrectCluster::execute() {
  // Get the input collection with clusters
  const fcc::CaloClusterCollection* inClusters = m_inClusters.get();
  fcc::CaloClusterCollection* correctedClusters = m_correctedClusters.createAndPut();

  // for single particle events compare with truth particles
  TVector3 momentum;
  double phiVertex = 0;
  double etaVertex = 0;
  double thetaVertex = 0;
  double zVertex = 0;
  const auto particle = m_particle.get();
  const auto vertex = m_vertex.get();
  if (particle->size() == 1 && vertex->size() == 1) {
    for(const auto& part : *particle) {
      momentum = TVector3(part.core().p4.px, part.core().p4.py, part.core().p4.pz);
      etaVertex = momentum.Eta();
      phiVertex = momentum.Phi();
      zVertex =  vertex->begin()->position().z;
      thetaVertex = 2 * atan( exp( - etaVertex ) );
    verbose() << " vertex eta " << etaVertex << "   phi = " << phiVertex << " theta = " << thetaVertex << " z = " << zVertex << endmsg;
   }
  }

  // TODO change that so all systems can be used
  uint systemId = m_systemId[0];
  const dd4hep::DDSegmentation::FCCSWGridPhiEta* segmentation = nullptr;
  if (m_segmentationPhiEta[systemId] != nullptr) {
    segmentation = m_segmentationPhiEta[systemId];
  }

  std::vector<TLorentzVector> clustersMassInv;
  std::vector<TLorentzVector> clustersMassInvScaled;
  for (const auto& cluster : *inClusters) {
    double oldEnergy = 0;
    TVector3 pos(cluster.core().position.x, cluster.core().position.y, cluster.core().position.z);
    double oldEta = pos.Eta();
    double oldPhi = pos.Phi();
    for (auto cell = cluster.hits_begin(); cell != cluster.hits_end(); cell++) {
      oldEnergy += cell->core().energy;
    }
    verbose() << " OLD ENERGY = " << oldEnergy << " from " << cluster.hits_size() << " cells" << endmsg;
    verbose() << " OLD CLUSTER ENERGY = " << cluster.core().energy << endmsg;

    // Do everything only using the first defined calorimeter (default: Ecal barrel)
    double oldEtaId = -1;
    double oldPhiId = -1;
    if (m_segmentationPhiEta[systemId] != nullptr) {
      oldEtaId = int(floor((oldEta + 0.5 * segmentation->gridSizeEta() - segmentation->offsetEta()) / segmentation->gridSizeEta()));
      oldPhiId = int(floor((oldPhi + 0.5 * segmentation->gridSizePhi() - segmentation->offsetPhi()) / segmentation->gridSizePhi()));
    }

    // 0. Create new cluster, copy information from input
    fcc::CaloCluster newCluster = correctedClusters->create();
    double energy = 0;
    newCluster.core().position.x = cluster.core().position.x;
    newCluster.core().position.y = cluster.core().position.y;
    newCluster.core().position.z = cluster.core().position.z;
    for (auto cell = cluster.hits_begin(); cell != cluster.hits_end(); cell++) {
      if (m_segmentationMulti[systemId] != nullptr) {
        segmentation = dynamic_cast<const dd4hep::DDSegmentation::FCCSWGridPhiEta*>(&m_segmentationMulti[systemId]->subsegmentation(cell->core().cellId));
        oldEtaId = int(floor((oldEta + 0.5 * segmentation->gridSizeEta() - segmentation->offsetEta()) / segmentation->gridSizeEta()));
        oldPhiId = int(floor((oldPhi + 0.5 * segmentation->gridSizePhi() - segmentation->offsetPhi()) / segmentation->gridSizePhi()));
      }
      if (m_decoder[systemId]->get(cell->core().cellId, "system") == systemId) {
        uint layerId = m_decoder[systemId]->get(cell->core().cellId, "layer");
        if(m_nPhiFinal[layerId] > 0 && m_nEtaFinal[layerId] > 0) {
          uint etaId = m_decoder[systemId]->get(cell->core().cellId, "eta");
          uint phiId = m_decoder[systemId]->get(cell->core().cellId, "phi");
          if ( etaId >= (oldEtaId - m_halfEtaFin[layerId]) &&  etaId <= (oldEtaId + m_halfEtaFin[layerId]) &&
               phiId >= phiNeighbour((oldPhiId - m_halfPhiFin[layerId]), segmentation->phiBins()) &&  phiId <= phiNeighbour((oldPhiId + m_halfPhiFin[layerId]), segmentation->phiBins()) ) {
            if (m_ellipseFinalCluster) {
              if ( pow( (etaId - oldEtaId) / (m_nEtaFinal[layerId] / 2.), 2) + pow( (phiId - oldPhiId) / (m_nPhiFinal[layerId] / 2.), 2) < 1) {
                newCluster.addhits(*cell);
                energy += cell->core().energy;
              }
            } else {
              newCluster.addhits(*cell);
              energy += cell->core().energy;
            }
          }
        }
      }
    }
    newCluster.core().energy = energy;

    // 1. Correct eta position with log-weighting
    double sumEnFirstLayer = 0;
    // get current pseudorapidity
    std::vector<double> sumEnLayer;
    std::vector<double> sumEnLayerSorted;
    std::vector<double> sumEtaLayer;
    std::vector<double> sumWeightLayer;
    sumEnLayer.assign(m_numLayers, 0);
    sumEnLayerSorted.assign(m_numLayers, 0);
    sumEtaLayer.assign(m_numLayers, 0);
    sumWeightLayer.assign(m_numLayers, 0);
    // first check the energy deposited in each layer
    for (auto cell = newCluster.hits_begin(); cell != newCluster.hits_end(); cell++) {
      dd4hep::DDSegmentation::CellID cID = cell->core().cellId;
      uint layer = m_decoder[systemId]->get(cID, m_layerFieldName) + m_firstLayerId;
      sumEnLayer[layer] += cell->core().energy;
    }
    // sort energy to check value of 2nd highest, 3rd highest etc
    for (uint iLayer = 0; iLayer < m_numLayers; iLayer++) {
      sumEnLayerSorted[iLayer] = sumEnLayer[iLayer];
    }
    std::sort(sumEnLayerSorted.begin(),sumEnLayerSorted.end(),std::greater<double>());
    sumEnFirstLayer = sumEnLayer[0];
    // repeat but calculating eta barycentre in each layer
    for (auto cell = newCluster.hits_begin(); cell != newCluster.hits_end(); cell++) {
      if (m_segmentationMulti[systemId] != nullptr) {
        segmentation = dynamic_cast<const dd4hep::DDSegmentation::FCCSWGridPhiEta*>(&m_segmentationMulti[systemId]->subsegmentation(cell->core().cellId));
      }
      dd4hep::DDSegmentation::CellID cID = cell->core().cellId;
      uint layer = m_decoder[systemId]->get(cID, m_layerFieldName) + m_firstLayerId;
      double weightLog = std::max(0., m_etaRecalcLayerWeights[layer] + log(cell->core().energy / sumEnLayer[layer]));
      double eta = segmentation->eta(cell->core().cellId);
      sumEtaLayer[layer] += (weightLog * eta);
      sumWeightLayer[layer] += weightLog;
    }
    // calculate eta position weighting with energy deposited in layer
    // this energy is a good estimator of 1/sigma^2 of (eta_barycentre-eta_MC) distribution
    double layerWeight = 0;
    double sumLayerWeight = 0;
    double sumLayerWeight2point = 0;
    double newEta = 0;
    double newEtaErrorRes = 0;
    double newEtaErrorRes2point = 0;
    for (uint iLayer = 0; iLayer < m_numLayers; iLayer++) {
      if (sumWeightLayer[iLayer] > 1e-10) {
        sumEtaLayer[iLayer] /= sumWeightLayer[iLayer];
        newEta += sumEtaLayer[iLayer] * sumEnLayer[iLayer];
        layerWeight = 1. / (pow(m_etaLayerResolutionSampling[iLayer], 2) / energy +  pow(m_etaLayerResolutionConst[iLayer], 2));
        sumLayerWeight += layerWeight;
        newEtaErrorRes += sumEtaLayer[iLayer] * layerWeight;
        if (iLayer == 1 || iLayer == 2) {
          newEtaErrorRes2point += sumEtaLayer[iLayer] * layerWeight;
          sumLayerWeight2point += layerWeight;
        }
      }
    }
    newEta /= energy;
    newEtaErrorRes /= sumLayerWeight;
    newEtaErrorRes2point /= sumLayerWeight2point;
    // alter Cartesian position of a cluster using new eta position
    double radius = pos.Perp();
    double phi = pos.Phi();
    newCluster.core().position.x = radius * cos(phi);
    newCluster.core().position.y = radius * sin(phi);
    newCluster.core().position.z = radius * sinh(newEta);

    // 2. Correct energy for pileup noise
    uint numCells = newCluster.hits_size();
    double noise = 0;
    if (m_constPileupNoise == 0) {
      noise = getNoiseConstantPerCluster(newEta, numCells) * m_gauss.shoot() * std::sqrt(static_cast<int>(m_mu));
      verbose() << " NUM CELLS = " << numCells << "   cluster noise const = " << getNoiseConstantPerCluster(newEta, numCells)
                << " scaled to PU " << m_mu<< "  = " << getNoiseConstantPerCluster(newEta, numCells)* std::sqrt(static_cast<int>(m_mu)) << endmsg;
    } else {
      noise = m_constPileupNoise * m_gauss.shoot() * std::sqrt(static_cast<int>(m_mu));
    }
    newCluster.core().energy += noise;
    m_hPileupEnergy->Fill(noise);

    // 3. Correct for energy upstream
    // correct for presampler based on energy in the first layer layer:
    // check eta of the cluster and get correction parameters:
    double P00 = 0, P01 = 0, P10 = 0, P11 = 0;
    for (uint iEta = 0; iEta < m_etaBorders.size(); iEta++) {
      if (fabs(newEta) < m_etaBorders[iEta]) {
        P00 = m_presamplerShiftP0[iEta];
        P01 = m_presamplerShiftP1[iEta];
        P10 = m_presamplerScaleP0[iEta];
        P11 = m_presamplerScaleP1[iEta];
        break;
      }
    }
    // if eta is larger than the last available eta values, take the last known parameters
    if (fabs(newEta) > m_etaBorders.back()) {
      warning() << "cluster eta = " << newEta << " is larger than last defined eta values." << endmsg;
      //return StatusCode::FAILURE;
    }
    double presamplerShift = P00 + P01 * cluster.core().energy;
    double presamplerScale = P10 + P11 * sqrt(cluster.core().energy);
    double energyFront = presamplerShift + presamplerScale * sumEnFirstLayer * m_samplingFraction[0];
    m_hUpstreamEnergy->Fill(energyFront);
    newCluster.core().energy += energyFront;

    // Fill histograms
    m_hEnergyPreAnyCorrections->Fill(oldEnergy);
    m_hEnergyPostAllCorrections->Fill(newCluster.core().energy);
    m_hEnergyPostAllCorrectionsAndScaling->Fill(newCluster.core().energy / m_response);

    // Position resolution
    m_hEta->Fill(newEta);
    m_hPhi->Fill(phi);
    verbose() << " energy " << energy << "   numCells = " << numCells << " old energy = " << oldEnergy <<
      " newEta " << newEta << "   phi = " << phi << " theta = " << 2 * atan( exp( - newEta ) ) << endmsg;
    m_hNumCells->Fill(numCells);
    // Fill histograms for single particle events
    if (particle->size() == 1) {
      m_hDiffEta->Fill(newEta - etaVertex);
      m_hDiffEtaResWeight->Fill(newEtaErrorRes - etaVertex);
      m_hDiffEtaResWeight2point->Fill(newEtaErrorRes2point - etaVertex);
      for(uint iLayer = 0; iLayer < m_numLayers; iLayer++) {
        m_hDiffEtaLayer[iLayer]->Fill(sumEtaLayer[iLayer] - etaVertex);
        if (energy > 0)
          m_hEnergyFractionInLayers->Fill(iLayer+1, sumEnLayer[iLayer] / energy);
      }
      m_hDiffPhi->Fill(phi - phiVertex);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode CorrectCluster::finalize() { return GaudiAlgorithm::finalize(); }

StatusCode CorrectCluster::initNoiseFromFile() {
  // check if file exists
  if (m_noiseFileName.empty()) {
    error() << "Name of the file with noise values not set" << endmsg;
    return StatusCode::FAILURE;
  }
  TFile file(m_noiseFileName.value().c_str(), "READ");
  if (file.IsZombie()) {
    error() << "Couldn't open the file with noise constants" << endmsg;
    return StatusCode::FAILURE;
  } else {
    info() << "Opening the file with noise constants: " << m_noiseFileName << endmsg;
  }

  std::string pileupParamHistoName;
  // Read the histograms with parameters for the pileup noise from the file
  for (unsigned i = 0; i < 2; i++) {
    pileupParamHistoName = m_pileupHistoName + std::to_string(i);
    debug() << "Getting histogram with a name " << pileupParamHistoName << endmsg;
    m_histoPileupConst.push_back(*dynamic_cast<TH1F*>(file.Get(pileupParamHistoName.c_str())));
    if (m_histoPileupConst.at(i).GetNbinsX() < 1) {
      error() << "Histogram  " << pileupParamHistoName
              << " has 0 bins! check the file with noise and the name of the histogram!" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // Check if we have same number of histograms (all layers) for pileup and electronics noise
  if (m_histoPileupConst.size() == 0) {
    error() << "No histograms with noise found!!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

double CorrectCluster::getNoiseConstantPerCluster(double aEta, uint aNumCells) {
  double param0 = 0.;
  double param1 = 0.;

  // All histograms have same binning, all bins with same size
  // Using the histogram of the first parameter to get the bin size
  unsigned index = 0;
  if (m_histoPileupConst.size() != 0) {
    int Nbins = m_histoPileupConst.at(index).GetNbinsX();
    double deltaEtaBin =
        (m_histoPileupConst.at(index).GetBinLowEdge(Nbins) + m_histoPileupConst.at(index).GetBinWidth(Nbins) -
         m_histoPileupConst.at(index).GetBinLowEdge(1)) /
        Nbins;
    double etaFirtsBin = m_histoPileupConst.at(index).GetBinLowEdge(1);
    // find the eta bin for the cell
    int ibin = floor((fabs(aEta) - etaFirtsBin) / deltaEtaBin) + 1;
    verbose() << "Current eta = " << aEta << " bin = " << ibin << endmsg;
    if (ibin > Nbins) {
      debug() << "eta outside range of the histograms! Cell eta: " << aEta << " Nbins in histogram: " << Nbins
              << endmsg;
      ibin = Nbins;
    }
    param0 = m_histoPileupConst.at(0).GetBinContent(ibin);
    param1 = m_histoPileupConst.at(1).GetBinContent(ibin);
    verbose() << "p0 = " << param0 << " param1 = " << param1 << endmsg;
  } else {
    debug() << "No histograms with noise constants!!!!! " << endmsg;
  }
  double pileupNoise = param0 * pow(aNumCells * (m_dEta / 0.01), param1);

  return pileupNoise;
}

unsigned int CorrectCluster::phiNeighbour(int aIPhi, int aMaxPhi) const {
  if (aIPhi < 0) {
    return aMaxPhi + aIPhi;
  } else if (aIPhi >= aMaxPhi) {
    return aIPhi % aMaxPhi;
  }
  return aIPhi;
}
