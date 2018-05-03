#include "PreparePileup.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// DD4hep
#include "DD4hep/Detector.h"
#include "DD4hep/Readout.h"

// our EDM
#include "datamodel/CaloHit.h"
#include "datamodel/CaloHitCollection.h"

// ROOT
#include "GaudiKernel/ITHistSvc.h"
#include "TH1F.h"
#include "TH2F.h"


DECLARE_ALGORITHM_FACTORY(PreparePileup)

PreparePileup::PreparePileup(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("hits", m_hits, "Hits from which to create cells (input)");
  declareProperty("geometryTool", m_geoTool, "Handle for the geometry tool");
  declareProperty("towerTool", m_towerTool, "Handle for the tower building tool");
}

StatusCode PreparePileup::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  // check if readouts exist
  if (m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout <<" << m_readoutName << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  // retrieve PhiEta segmentation
  m_segmentation = dynamic_cast<dd4hep::DDSegmentation::FCCSWGridPhiEta*>(
      m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if (m_segmentation == nullptr) {
    error() << "There is no phi-eta segmentation." << endmsg;
    return StatusCode::FAILURE;
  }
  // Take readout bitfield decoder from GeoSvc
  m_decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
  // Histogram service
  m_histSvc = service("THistSvc");
  if (!m_histSvc) {
    error() << "Unable to locate Histogram Service" << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_etaSizes.size() != m_phiSizes.size()) {
    error() << "The same number of cluster sizes needs to be defined for eta and phi!" << endmsg;
    return StatusCode::FAILURE;
  }
  // Prepare histograms - 2D histograms per layer, abs(eta) on x-axis, energy in cell on y-axis
  for (uint i = 0; i < m_numLayers; i++) {
    m_energyVsAbsEta.push_back(
             new TH2F((m_histogramName + std::to_string(i)).c_str(),
          ("energy per cell vs fabs cell eta in layer " + std::to_string(i)).c_str(),
          60, 0, 6.0, 5000, -1, m_maxEnergy) );
    if (m_histSvc
  ->regHist("/rec/" + m_histogramName + std::to_string(i), m_energyVsAbsEta.back())
  .isFailure()) {
      error() << "Couldn't register hist" << endmsg;
      return StatusCode::FAILURE;
    }
    m_energyAllEventsVsAbsEta.push_back(
          new TH2F((m_histogramName + std::to_string(i) + "AllEvents").c_str(),
             ("sum of energy per cell in all events vs fabs cell eta in layer " + std::to_string(i)).c_str(),
             60, 0, 6.0, 5000, -1, m_maxEnergy*20) );
    if (m_histSvc
  ->regHist("/rec/" + m_histogramName + std::to_string(i) + "AllEvents", m_energyAllEventsVsAbsEta.back())
  .isFailure()) {
      error() << "Couldn't register hist" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  for (uint iCluster = 0; iCluster < m_etaSizes.size(); iCluster++) {
    m_energyVsAbsEtaClusters.push_back(new TH2F((m_histogramName + "_clusterEta" + std::to_string(m_etaSizes[iCluster]) + "Phi" + std::to_string(m_phiSizes[iCluster])).c_str(),
            ("energy per cluster #Delta#eta#times#Delta#varphi = " + std::to_string(m_etaSizes[iCluster]) + "#times" + std::to_string(m_phiSizes[iCluster]) + " vs fabs centre-cell eta ").c_str(),
            60, 0, 6.0, 5000, -1, m_maxEnergy));
    if (m_histSvc->regHist("/rec/" + m_histogramName + "_clusterEta" + std::to_string(m_etaSizes[iCluster]) + "Phi"+ std::to_string(m_phiSizes[iCluster]), m_energyVsAbsEtaClusters.back()).isFailure()) {
      error() << "Couldn't register hist" << endmsg;
      return StatusCode::FAILURE;
    }
    m_energyAllEventsVsAbsEtaClusters.push_back(new TH2F((m_histogramName + "_clusterEta" + std::to_string(m_etaSizes[iCluster]) + "Phi"
                + std::to_string(m_phiSizes[iCluster]) + "AllEvents").c_str(),
               ("sum of energy per cluster #Delta#eta#times#Delta#varphi = " + std::to_string(m_etaSizes[iCluster]) + "#times" + std::to_string(m_phiSizes[iCluster]) + " in all events vs fabs centre-cell eta ").c_str(),
               60, 0, 6.0, 5000, -1, m_maxEnergy*20) );
    if (m_histSvc
  ->regHist("/rec/" + m_histogramName + "_clusterEta" + std::to_string(m_etaSizes[iCluster]) + "Phi" + std::to_string(m_phiSizes[iCluster]) + "AllEvents", m_energyAllEventsVsAbsEtaClusters.back())
  .isFailure()) {
      error() << "Couldn't register hist" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  // Initialization of geometry tool
  if (!m_geoTool.retrieve()) {
    error() << "Unable to retrieve the geometry tool!!!" << endmsg;
    return StatusCode::FAILURE;
  }
  // Prepare map of all existing cells in calorimeter to add minimum bias signal
  StatusCode sc_prepareCells = m_geoTool->prepareEmptyCells(m_cellsMap);
  if (sc_prepareCells.isFailure()) {
    error() << "Unable to create empty cells!" << endmsg;
    return StatusCode::FAILURE;
  }
  info() << "Number of empty cells: " << m_cellsMap.size() << endmsg;

  //StatusCode sc_prepareCellsTwo = m_geoTool->prepareEmptyCells(m_sumEnergyCellsMap);
  m_sumEnergyCellsMap = m_cellsMap;

  if (!m_towerTool.retrieve()) {
    error() << "Unable to retrieve the tower building tool." << endmsg;
    return StatusCode::FAILURE;
  }
  auto towerMapSize = m_towerTool->towersNumber();
  m_nEtaTower = towerMapSize.eta;
  m_nPhiTower = towerMapSize.phi;
  debug() << "Number of calorimeter towers (eta x phi) : " << m_nEtaTower << " x " << m_nPhiTower << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode PreparePileup::execute() {
  // Get the input collection with Geant4 hits
  const fcc::CaloHitCollection* hits = m_hits.get();
  debug() << "Input Hit collection size: " << hits->size() << endmsg;

  // 0. Clear all cells
  std::for_each(m_cellsMap.begin(), m_cellsMap.end(), [](std::pair<const uint64_t, double>& p) { p.second = 0; });

  // Merge signal events with empty cells
  for (const auto& hit : *hits) {
    m_cellsMap[hit.core().cellId] += hit.core().energy;
    m_sumEnergyCellsMap[hit.core().cellId] += hit.core().energy;
  }
  debug() << "Number of calorimeter cells after merging of hits: " << m_cellsMap.size() << endmsg;

  //Fill 2D histogram per layer
  for (const auto& cell : m_cellsMap) {
    double cellEnergy = cell.second;
    uint64_t cellId = cell.first;
    m_decoder->setValue(cellId);
    uint layerId = (*m_decoder)[m_layerFieldName];
    if (layerId>=m_numLayers) {
      layerId = m_numLayers-1;
      warning() << "Layer id of the cell "<< layerId
                << " is larger than number of layers in the histogram: "
                << m_numLayers
                << ". Filling the last histogram." << endmsg;

    }
    double cellEta = m_segmentation->eta(cellId);
    m_energyVsAbsEta[layerId]->Fill(fabs(cellEta), cellEnergy);
  }
  // create towers
  m_towers.assign(m_nEtaTower, std::vector<float>(m_nPhiTower, 0));
  m_towerTool->buildTowers(m_towers);
  for (uint iCluster = 0; iCluster < m_etaSizes.size(); iCluster++) {
    debug() <<"Size of the reconstruction window (eta,phi) " << m_etaSizes[iCluster] << ", " << m_phiSizes[iCluster] << endmsg;
    // calculate the sum of first m_nEtaWindow bins in eta, for each phi tower
    std::vector<float> sumOverEta(m_nPhiTower, 0);
    for (int iEta = 0; iEta < m_etaSizes[iCluster]; iEta++) {
      std::transform(sumOverEta.begin(), sumOverEta.end(), m_towers[iEta].begin(), sumOverEta.begin(),
                     std::plus<float>());
    }
    int halfEtaWin = floor(m_etaSizes[iCluster] / 2.);
    int halfPhiWin = floor(m_phiSizes[iCluster] / 2.);
    debug() <<"Half-size of the reconstruction window (eta,phi) " << halfEtaWin << ", " << halfPhiWin << endmsg;
    double sumWindow = 0;
    // one slice in eta = window, now only sum over window elements in phi
    for (int iEta = halfEtaWin; iEta < m_nEtaTower - halfEtaWin; iEta++) {
      // sum the first window in phi
      for (int iPhiWindow = -halfPhiWin; iPhiWindow <= halfPhiWin; iPhiWindow++) {
        sumWindow += sumOverEta[phiNeighbour(iPhiWindow)];
      }
      // loop over all the phi slices
      for (int iPhi = 0; iPhi < m_nPhiTower; iPhi++) {
        m_energyVsAbsEtaClusters[iCluster]->Fill(fabs(m_towerTool->eta(iEta)), sumWindow * cosh(fabs(m_towerTool->eta(iEta))));
        // finish processing that window in phi, shift window to the next phi tower
        // substract first phi tower in current window
        sumWindow -= sumOverEta[phiNeighbour(iPhi - halfPhiWin)];
        // add next phi tower to the window
        sumWindow += sumOverEta[phiNeighbour(iPhi + halfPhiWin + 1)];
      }
      sumWindow = 0;
      // finish processing that slice, shift window to next eta tower
      if (iEta < m_nEtaTower - halfEtaWin - 1) {
        // substract first eta slice in current window
        std::transform(sumOverEta.begin(), sumOverEta.end(), m_towers[iEta - halfEtaWin].begin(), sumOverEta.begin(),
                       std::minus<float>());
        // add next eta slice to the window
        std::transform(sumOverEta.begin(), sumOverEta.end(), m_towers[iEta + halfEtaWin + 1].begin(), sumOverEta.begin(),
                       std::plus<float>());
      }
    }
  }

  // create towers

  return StatusCode::SUCCESS;
}

unsigned int PreparePileup::phiNeighbour(int aIPhi) const {
  if (aIPhi < 0) {
    return m_nPhiTower + aIPhi;
  } else if (aIPhi >= m_nPhiTower) {
    return aIPhi % m_nPhiTower;
  }
  return aIPhi;
}


StatusCode PreparePileup::finalize() {

  //Fill 2D histogram per layer (sum of energy in all events per cell)
  for (const auto& cell : m_sumEnergyCellsMap) {
    double cellEnergy = cell.second;
    uint64_t cellId = cell.first;
    m_decoder->setValue(cellId);
    uint layerId = (*m_decoder)[m_layerFieldName];
    if (layerId>=m_numLayers) {
      layerId = m_numLayers-1;
      warning() << "Layer id of the cell "<< layerId
                << " is larger than number of layers in the histogram: "
                << m_numLayers
                << ". Filling the last histogram." << endmsg;

    }
    double cellEta = m_segmentation->eta(cellId);
    m_energyAllEventsVsAbsEta[layerId]->Fill(fabs(cellEta), cellEnergy);
  }

return GaudiAlgorithm::finalize();
}
