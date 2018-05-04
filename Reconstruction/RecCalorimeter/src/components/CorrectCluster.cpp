#include "CorrectCluster.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"
#include "DetSegmentation/FCCSWGridPhiEta.h"
#include "GaudiKernel/ITHistSvc.h"

// DD4hep
#include "DD4hep/Detector.h"

// our EDM
#include "datamodel/CaloCluster.h"
#include "datamodel/CaloClusterCollection.h"
#include "datamodel/CaloHitCollection.h"

// Root
#include "TFile.h"

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
}

StatusCode CorrectCluster::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  // create control histograms
  m_hEnergyPreAnyCorrections = new TH1F(
      "energyPreAnyCorrections", "Energy of cluster before any correction", 1000, 0.8 * m_energy, 1.2 * m_energy);
  if (m_histSvc->regHist("/rec/energyPreAnyCorrections", m_hEnergyPreAnyCorrections).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_hEnergyPostAllCorrections = new TH1F(
      "energyPostAllCorrections", "Energy of cluster after all corrections", 1000, 0.8 * m_energy, 1.2 * m_energy);
  if (m_histSvc->regHist("/rec/energyPostAllCorrections", m_hEnergyPostAllCorrections).isFailure()) {
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
    m_segmentation[m_systemId[iSys]] = dynamic_cast<dd4hep::DDSegmentation::FCCSWGridPhiEta*>(
        m_geoSvc->lcdd()->readout(m_readoutName[iSys]).segmentation().segmentation());
    if (m_segmentation[m_systemId[iSys]] == nullptr) {
      error() << "There is no phi-eta segmentation." << endmsg;
      return StatusCode::FAILURE;
    }
    m_decoder[m_systemId[iSys]] = m_geoSvc->lcdd()->readout(m_readoutName[iSys]).idSpec().decoder();
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
  return StatusCode::SUCCESS;
}

StatusCode CorrectCluster::execute() {
  // Get the input collection with clusters
  const fcc::CaloClusterCollection* inClusters = m_inClusters.get();
  fcc::CaloClusterCollection* correctedClusters = m_correctedClusters.createAndPut();

  for (const auto& cluster : *inClusters) {
    double energy = 0;
    TVector3 pos(cluster.core().position.x, cluster.core().position.y, cluster.core().position.z);
    double oldEta = pos.Eta();
    for (auto cell = cluster.hits_begin(); cell != cluster.hits_end(); cell++) {
      energy += cell->core().energy;
    }

    // 0. Create new cluster, copy information from input
    fcc::CaloCluster newCluster = correctedClusters->create();
    newCluster.core().energy = cluster.core().energy;
    newCluster.core().position.x = cluster.core().position.x;
    newCluster.core().position.y = cluster.core().position.y;
    newCluster.core().position.z = cluster.core().position.z;
    for (auto cell = cluster.hits_begin(); cell != cluster.hits_end(); cell++) {
      newCluster.addhits(*cell);
    }

    // 1. Correct eta position with log-weighting
    double sumEnFirstLayer = 0;
    uint systemId = m_systemId[0];
    // get current pseudorapidity
    double newEta = 0;
    std::vector<double> sumEnLayer;
    std::vector<double> sumEtaLayer;
    std::vector<double> sumWeightLayer;
    sumEnLayer.assign(m_numLayers, 0);
    sumEtaLayer.assign(m_numLayers, 0);
    sumWeightLayer.assign(m_numLayers, 0);
    // first check the energy deposited in each layer
    for (auto cell = cluster.hits_begin(); cell != cluster.hits_end(); cell++) {
      m_decoder[systemId]->setValue(cell->core().cellId);
      uint layer = (*m_decoder[systemId])[m_layerFieldName] + m_firstLayerId;
      sumEnLayer[layer] += cell->core().energy;
    }
    sumEnFirstLayer = sumEnLayer[0];
    // repeat but calculating eta barycentre in each layer
    for (auto cell = cluster.hits_begin(); cell != cluster.hits_end(); cell++) {
      m_decoder[systemId]->setValue(cell->core().cellId);
      uint layer = (*m_decoder[systemId])[m_layerFieldName] + m_firstLayerId;
      double weightLog = std::max(0., m_etaRecalcLayerWeights[layer] + log(cell->core().energy / sumEnLayer[layer]));
      double eta = m_segmentation[systemId]->eta(cell->core().cellId);
      sumEtaLayer[layer] += (weightLog * eta);
      sumWeightLayer[layer] += weightLog;
    }
    // calculate eta position weighting with energy deposited in layer
    // this energy is a good estimator of 1/sigma^2 of (eta_barycentre-eta_MC) distribution
    for (uint iLayer = 0; iLayer < m_numLayers; iLayer++) {
      if (sumWeightLayer[iLayer] > 1e-10) {
        sumEtaLayer[iLayer] /= sumWeightLayer[iLayer];
        newEta += sumEtaLayer[iLayer] * sumEnLayer[iLayer];
      }
    }
    newEta /= energy;
    // alter Cartesian position of a cluster using new eta position
    double radius = pos.Perp();
    double phi = pos.Phi();
    newCluster.core().position.x = radius * cos(phi);
    newCluster.core().position.y = radius * sin(phi);
    newCluster.core().position.z = radius * sinh(newEta);

    // 2. Correct energy for pileup noise
    uint numCells = newCluster.hits_size();
    double noise = getNoiseConstantPerCluster(newEta, numCells) * m_gauss.shoot();
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
      error() << "cluster eta = " << newEta << " is larger than last defined eta values." << endmsg;
      return StatusCode::FAILURE;
    }
    double presamplerShift = P00 + P01 * cluster.core().energy;
    double presamplerScale = P10 + P11 * sqrt(cluster.core().energy);
    double energyFront = presamplerShift + presamplerScale * sumEnFirstLayer * m_samplingFraction[0];
    m_hUpstreamEnergy->Fill(energyFront);
    newCluster.core().energy += energyFront;

    // Fill histograms
    m_hEnergyPreAnyCorrections->Fill(cluster.core().energy);
    m_hEnergyPostAllCorrections->Fill(newCluster.core().energy);
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
    if (ibin > Nbins) {
      debug() << "eta outside range of the histograms! Cell eta: " << aEta << " Nbins in histogram: " << Nbins
              << endmsg;
      ibin = Nbins;
    }
    param0 = m_histoPileupConst.at(0).GetBinContent(ibin);
    param1 = m_histoPileupConst.at(1).GetBinContent(ibin);
  } else {
    debug() << "No histograms with noise constants!!!!! " << endmsg;
  }
  double pileupNoise = param0 * pow(aNumCells, param1);

  return pileupNoise;
}
