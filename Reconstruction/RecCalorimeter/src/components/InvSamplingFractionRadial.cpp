#include "InvSamplingFractionRadial.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// datamodel
#include "datamodel/MCParticleCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"

#include "TH1F.h"
#include "TVector2.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "GaudiKernel/ITHistSvc.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DD4hep/Readout.h"

DECLARE_ALGORITHM_FACTORY(InvSamplingFractionRadial)

InvSamplingFractionRadial::InvSamplingFractionRadial(const std::string& aName, ISvcLocator* aSvcLoc):
GaudiAlgorithm(aName, aSvcLoc), m_totalEnergy(nullptr), m_totalActiveEnergy(nullptr), m_SF(nullptr) {
  declareInput("particles", m_MCparticles);
  declareInput("deposits", m_deposits);
  declareProperty("energy", m_energy);
  declareProperty("readoutName", m_readoutName);
}
InvSamplingFractionRadial::~InvSamplingFractionRadial() {}

StatusCode InvSamplingFractionRadial::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;
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
  m_histSvc = service("THistSvc");
  if (! m_histSvc) {
    error() << "Unable to locate Histogram Service" << endmsg;
    return StatusCode::FAILURE;
  }
  int num_layers = 32;
  for(int i = 0; i < num_layers; i++) {
    m_radialEnergy.push_back( new TH1F(("rad_total"+std::to_string(i)).c_str(), ("Total deposited energy in radial layer "+std::to_string(i)).c_str(), 1000, 0, 1.2 * m_energy) );
    if (m_histSvc->regHist("/rec/rad"+std::to_string(i), m_radialEnergy.back()).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
    }
    m_radialActiveEnergy.push_back( new TH1F(("rad_active"+std::to_string(i)).c_str(), ("Deposited energy in active material, in radial layer "+std::to_string(i)).c_str(), 1000, 0, 1.2 * m_energy) );
    if (m_histSvc->regHist("/rec/rad_active"+std::to_string(i), m_radialActiveEnergy.back()).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
    }
    m_radialSF.push_back( new TH1F(("sf"+std::to_string(i)).c_str(), ("1/SF for radial layer "+std::to_string(i)).c_str(), 1000, 0, 20) );
    if (m_histSvc->regHist("/rec/sf"+std::to_string(i), m_radialSF.back()).isFailure()) {
      error() << "Couldn't register histogram" << endmsg;
      return StatusCode::FAILURE;
    }
  }
  m_totalEnergy = new TH1F("total", "Total deposited energy", 1000, 0, 1.2 * m_energy);
  if (m_histSvc->regHist("/rec/total", m_totalEnergy).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_totalActiveEnergy = new TH1F("totalActive", "Total active deposited energy", 1000, 0, 1.2 * m_energy);
  if (m_histSvc->regHist("/rec/totalActive", m_totalActiveEnergy).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_SF = new TH1F("SF", "1 / Sampling fraction", 1000, 0, 20);
  if (m_histSvc->regHist("/rec/sf", m_SF).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode InvSamplingFractionRadial::execute() {
  const auto particles = m_MCparticles.get();
  if(particles->size() > 1) {
    error() << "MCtruth includes more than 1 particle per event: size " << particles->size() << endmsg;
    return StatusCode::FAILURE;
  }
  const auto& core = particles->begin()->core();
  TVector2 momentumXY(core.p4.px, core.p4.py);
  double phi = momentumXY.Phi_0_2pi(momentumXY.Phi());

  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
  double sumE = 0.;
  std::vector<double> sumEradial;
  double sumEactive = 0.;
  std::vector<double> sumEactiveRadial;
  sumEradial.assign(m_radialEnergy.size(), 0);
  sumEactiveRadial.assign(m_radialEnergy.size(), 0);

  const auto deposits = m_deposits.get();
  for(const auto& hit : *deposits) {
    sumE += hit.core().energy;
    decoder->setValue(hit.core().cellId);
    sumEradial[(*decoder)["r"]] += hit.core().energy;
    if((*decoder)["active"] > 0) {
      sumEactive += hit.core().energy;
      sumEactiveRadial[(*decoder)["r"]] += hit.core().energy;
    }
  }
  //Fill histograms
  m_totalEnergy->Fill(sumE);
  m_totalActiveEnergy->Fill(sumEactive);
  m_SF->Fill(sumE / sumEactive);
  for(uint i = 0; i < m_radialEnergy.size(); i++) {
    m_radialEnergy[i]->Fill(sumEradial[i]);
    m_radialActiveEnergy[i]->Fill(sumEactiveRadial[i]);
    m_radialSF[i]->Fill(sumEradial[i] / sumEactiveRadial[i]);
  }
  return StatusCode::SUCCESS;
}

StatusCode InvSamplingFractionRadial::finalize() {
  return GaudiAlgorithm::finalize();
}
