#include "PreparePileup.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// DD4hep
#include "DD4hep/LCDD.h"
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
  m_segmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(
      m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if (m_segmentation == nullptr) {
    error() << "There is no phi-eta segmentation." << endmsg;
    return StatusCode::FAILURE;
  }
  // Take readout bitfield decoder from GeoSvc
  m_decoder =
    std::shared_ptr<DD4hep::DDSegmentation::BitField64>(m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder());
  // Histogram service
  m_histSvc = service("THistSvc");
  if (!m_histSvc) {
    error() << "Unable to locate Histogram Service" << endmsg;
    return StatusCode::FAILURE;
  }
  // Prepare histograms - 2D histograms per layer, abs(eta) on x-axis, energy in cell on y-axis
  // TODO: add similar histogram with energy in a cluster
  for (uint i = 0; i < m_numLayers; i++) {
    m_energyVsAbsEta.push_back(
			    new TH2F(("energyVsAbsEta" + std::to_string(i)).c_str(),
				     ("energy per cell vs fabs cell eta in layer " + std::to_string(i)).c_str(),
				     60, 0, 6.0, 5000, -1, m_maxEnergy) );
    if (m_histSvc
	->regHist("/rec/energyVsAbsEta" + std::to_string(i), m_energyVsAbsEta.back())
	.isFailure()) {
      error() << "Couldn't register hist" << endmsg;
      return StatusCode::FAILURE;
    }
    m_energyAllEventsVsAbsEta.push_back(
					new TH2F(("energyAllEventsVsAbsEta" + std::to_string(i)).c_str(),
						 ("sum of energy per cell in all events vs fabs cell eta in layer " + std::to_string(i)).c_str(),
						 60, 0, 6.0, 5000, -1, m_maxEnergy*20) );
    if (m_histSvc
	->regHist("/rec/energyAllEventsVsAbsEta" + std::to_string(i), m_energyAllEventsVsAbsEta.back())
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
  
  return StatusCode::SUCCESS;
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
