#include "HepMCHistograms.h"

DECLARE_COMPONENT(HepMCHistograms)

HepMCHistograms::HepMCHistograms(const std::string& name, ISvcLocator* svcLoc) :
  GaudiAlgorithm(name, svcLoc),
  m_ths(nullptr),
  m_pt(nullptr),
  m_eta(nullptr),
  m_d0(nullptr),
  m_z0(nullptr) {
  declareInput("hepmc", m_hepmchandle);
}

StatusCode HepMCHistograms::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;
  
  if (service("THistSvc", m_ths).isFailure()) {
    error() << "Couldn't get THistSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  
  m_pt = new TH1F("GenPt", "Generated particles pT", 100, .1, 10);
  if (m_ths->regHist("/rec/GenPt", m_pt).isFailure()) {
    error() << "Couldn't register GenPt" << endmsg;
  }
  
  m_eta = new TH1F("GenEta", "Generated particles Pseudorapidity", 100, -10, 10);
  if (m_ths->regHist("/rec/GenEta", m_eta).isFailure()) {
    error() << "Couldn't register GenEta" << endmsg;
  }
  
  m_d0 = new TH1F("GenD0", "Transversal Impact Parameter", 100, 0, 10);
  if (m_ths->regHist("/rec/GenD0", m_d0).isFailure()) {
    error() << "Couldn't register GenD0" << endmsg;
  }
  
  m_z0 = new TH1F("GenZ0", "Longitudinal Impact Parameter", 100, -30, 30);
  if (m_ths->regHist("/rec/GenZ0", m_z0).isFailure()) {
    error() << "Couldn't register GenZ0" << endmsg;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode HepMCHistograms::execute() {
  auto evt = m_hepmchandle.get();
  
  info() << "Processing event with " << evt->particles_size() << " particles" << endmsg;
  
  for (HepMC::GenEvent::particle_const_iterator it = evt->particles_begin(), end = evt->particles_end();
    it != end;
    ++it) {
    auto particle = *it;
    m_eta->Fill(particle->momentum().eta());
    m_pt->Fill(particle->momentum().perp());
  }

  for (HepMC::GenEvent::vertex_const_iterator it = evt->vertices_begin(), end = evt->vertices_end();
    it != end;
    ++it) {
    auto vertex = *it;
    m_d0->Fill(vertex->position().perp());
    m_z0->Fill(vertex->position().z());
  }
  
  return StatusCode::SUCCESS;
}

StatusCode HepMCHistograms::finalize() {
  if (GaudiAlgorithm::finalize().isFailure())
    return StatusCode::FAILURE;
  
  return StatusCode::SUCCESS;
}
