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
    : GaudiAlgorithm(aName, aSvcLoc),
      m_histSvc("THistSvc", "SamplingFractionInLayers"),
      m_geoSvc("GeoSvc", "SamplingFractionInLayers"),
      m_totalEnergy(nullptr),
      m_totalActiveEnergy(nullptr),
      m_sf(nullptr) {
  declareProperty("deposits", m_deposits, "Energy deposits in sampling calorimeter (input)");
}
SamplingFractionInLayers::~SamplingFractionInLayers() {}

StatusCode SamplingFractionInLayers::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  // check if readouts exist
  if (m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout <<" << m_readoutName << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  // create histograms
  for (uint i = 0; i < m_numLayers; i++) {
    m_totalEnLayers.push_back(new TH1F(("ecal_totalEnergy_layer" + std::to_string(i)).c_str(),
                                       ("Total deposited energy in layer " + std::to_string(i)).c_str(), 1000, 0,
                                       1.2 * m_energy));
    if (m_histSvc->regHist("/rec/ecal_total_layer" + std::to_string(i), m_totalEnLayers.back()).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
    }
    m_activeEnLayers.push_back(new TH1F(("ecal_activeEnergy_layer" + std::to_string(i)).c_str(),
                                        ("Deposited energy in active material, in layer " + std::to_string(i)).c_str(),
                                        1000, 0, 1.2 * m_energy));
    if (m_histSvc->regHist("/rec/ecal_active_layer" + std::to_string(i), m_activeEnLayers.back()).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
    }
    m_sfLayers.push_back(new TH1F(("ecal_sf_layer" + std::to_string(i)).c_str(),
                                  ("SF for layer " + std::to_string(i)).c_str(), 1000, 0, 1));
    if (m_histSvc->regHist("/rec/ecal_sf_layer" + std::to_string(i), m_sfLayers.back()).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  m_totalEnergy = new TH1F("ecal_totalEnergy", "Total deposited energy", 1000, 0, 1.2 * m_energy);
  if (m_histSvc->regHist("/rec/ecal_total", m_totalEnergy).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_totalActiveEnergy = new TH1F("ecal_active", "Deposited energy in active material", 1000, 0, 1.2 * m_energy);
  if (m_histSvc->regHist("/rec/ecal_active", m_totalActiveEnergy).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_sf = new TH1F("ecal_sf", "Sampling fraction", 1000, 0, 1);
  if (m_histSvc->regHist("/rec/ecal_sf", m_sf).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode SamplingFractionInLayers::execute() {
  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
  double sumE = 0.;
  std::vector<double> sumElayers;
  double sumEactive = 0.;
  std::vector<double> sumEactiveLayers;
  sumElayers.assign(m_numLayers, 0);
  sumEactiveLayers.assign(m_numLayers, 0);

  const auto deposits = m_deposits.get();
  for (const auto& hit : *deposits) {
    decoder->setValue(hit.core().cellId);
    sumElayers[(*decoder)[m_layerFieldName]] += hit.core().energy;
    // check if energy was deposited in the calorimeter (active/passive material)
    if ((*decoder)[m_layerFieldName] >= m_firstLayerId) {
      sumE += hit.core().energy;
      // active material of calorimeter
      if ((*decoder)[m_activeFieldName] == m_activeFieldValue) {
        sumEactive += hit.core().energy;
        sumEactiveLayers[(*decoder)[m_layerFieldName]] += hit.core().energy;
      }
    }
  }
  // Fill histograms
  m_totalEnergy->Fill(sumE);
  m_totalActiveEnergy->Fill(sumEactive);
  if (sumE > 0) {
    m_sf->Fill(sumEactive / sumE);
  }
  for (uint i = 0; i < m_numLayers; i++) {
    m_totalEnLayers[i]->Fill(sumElayers[i]);
    m_activeEnLayers[i]->Fill(sumEactiveLayers[i]);
    if (i < m_firstLayerId) {
      debug() << "total energy deposited outside the calorimeter detector = " << sumElayers[i] << endmsg;
    } else {
      debug() << "total energy in layer " << i << " = " << sumElayers[i] << " active = " << sumEactiveLayers[i]
              << endmsg;
    }
    if (sumElayers[i] > 0) {
      m_sfLayers[i]->Fill(sumEactiveLayers[i] / sumElayers[i]);
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode SamplingFractionInLayers::finalize() { return GaudiAlgorithm::finalize(); }
