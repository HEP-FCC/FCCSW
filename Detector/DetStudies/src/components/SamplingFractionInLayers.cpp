#include "SamplingFractionInLayers.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// datamodel
#include "datamodel/PositionedCaloHitCollection.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TH1F.h"
#include "TVector2.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DD4hep/Readout.h"

DECLARE_ALGORITHM_FACTORY(SamplingFractionInLayers)

SamplingFractionInLayers::SamplingFractionInLayers(const std::string& aName, ISvcLocator* aSvcLoc)
    : GaudiAlgorithm(aName, aSvcLoc), m_totalEnergy(nullptr), m_totalActiveEnergy(nullptr), m_SF(nullptr) {
  declareProperty("deposits", m_deposits, "Energy deposits in sampling calorimeter (input)");
}
SamplingFractionInLayers::~SamplingFractionInLayers() {}

StatusCode SamplingFractionInLayers::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) return StatusCode::FAILURE;
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
  m_histSvc = service("THistSvc");
  if (!m_histSvc) {
    error() << "Unable to locate Histogram Service" << endmsg;
    return StatusCode::FAILURE;
  }
  int num_layers = m_numLayers;
  for (int i = 0; i < num_layers; i++) {
    m_cellsEnergy.push_back(new TH1F(("cell_total" + std::to_string(i)).c_str(),
                                     ("Total deposited energy in layer " + std::to_string(i)).c_str(), 1000, 0,
                                     1.2 * m_energy));
    if (m_histSvc->regHist("/rec/cell_total" + std::to_string(i), m_cellsEnergy.back()).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
    }
    m_cellsActiveEnergy.push_back(new TH1F(
        ("cell_active" + std::to_string(i)).c_str(),
        ("Deposited energy in active material, in layer " + std::to_string(i)).c_str(), 1000, 0, 1.2 * m_energy));
    if (m_histSvc->regHist("/rec/cell_active" + std::to_string(i), m_cellsActiveEnergy.back()).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
    }
    m_cellsSF.push_back(
        new TH1F(("cell_sf" + std::to_string(i)).c_str(), ("SF for layer " + std::to_string(i)).c_str(), 1000, 0, 1));
    if (m_histSvc->regHist("/rec/cell_sf" + std::to_string(i), m_cellsSF.back()).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  m_totalEnergy = new TH1F("cell_total", "Total deposited energy", 1000, 0, 1.2 * m_energy);
  if (m_histSvc->regHist("/rec/cell_total", m_totalEnergy).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_totalActiveEnergy = new TH1F("cell_totalActive", "Total active deposited energy", 1000, 0, 1.2 * m_energy);
  if (m_histSvc->regHist("/rec/cell_totalActive", m_totalActiveEnergy).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_SF = new TH1F("cell_SF", "Sampling fraction", 1000, 0, 1);
  if (m_histSvc->regHist("/rec/cell_sf", m_SF).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SamplingFractionInLayers::execute() {
  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
  double sumE = 0.;
  std::vector<double> sumEcells;
  double sumEactive = 0.;
  std::vector<double> sumEactiveCells;
  sumEcells.assign(m_cellsEnergy.size(), 0);
  sumEactiveCells.assign(m_cellsEnergy.size(), 0);

  const auto deposits = m_deposits.get();
  for (const auto& hit : *deposits) {
    sumEcells[(*decoder)[m_layerFieldName]] += hit.core().energy;
    // check if energy was deposited in the calorimeter (active/passive material)
    // layers are numbered starting from 1, layer == 0 is cryostat/bath
    if ((*decoder)[m_layerFieldName] > 0) {
      sumE += hit.core().energy;
      decoder->setValue(hit.core().cellId);
      // active material of calorimeter
      if ((*decoder)[m_activeFieldName] == m_activeFieldValue) {
        sumEactive += hit.core().energy;
        sumEactiveCells[(*decoder)[m_layerFieldName]] += hit.core().energy;
      }
    }
  }
  // Fill histograms
  m_totalEnergy->Fill(sumE);
  m_totalActiveEnergy->Fill(sumEactive);
  if (sumE > 0) {
    m_SF->Fill(sumEactive / sumE);
  }
  for (uint i = 0; i < m_cellsEnergy.size(); i++) {
    m_cellsEnergy[i]->Fill(sumEcells[i]);
    m_cellsActiveEnergy[i]->Fill(sumEactiveCells[i]);
    if (i == 0) {
      debug() << "total energy deposited in cryostat and bath = " << sumEcells[i] << endmsg;
    } else {
      debug() << "total energy in layer " << i << " = " << sumEcells[i] << " active = " << sumEactiveCells[i] << endmsg;
    }
    if (sumEcells[i] > 0) {
      m_cellsSF[i]->Fill(sumEactiveCells[i] / sumEcells[i]);
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode SamplingFractionInLayers::finalize() { return GaudiAlgorithm::finalize(); }
