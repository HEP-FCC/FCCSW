#include "TrackEfficiencies.h"

#include "GaudiKernel/ITHistSvc.h"

#include "datamodel/MCParticleCollection.h"
#include "datamodel/ParticleCollection.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "TH1F.h"
#include "TH2F.h"

DECLARE_COMPONENT(TrackEfficiencies)

TrackEfficiencies::TrackEfficiencies(const std::string& aName, ISvcLocator* aSvcLoc)
    : GaudiAlgorithm(aName, aSvcLoc) {
  declareProperty("SimParticles", m_simParticles, "Handle for MCTruth Particles (input)");
  declareProperty("RecParticles", m_recParticles, "Handle for Reco Particles (input)");
}
TrackEfficiencies::~TrackEfficiencies() {}

StatusCode TrackEfficiencies::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) return StatusCode::FAILURE;
  m_histSvc = service("THistSvc");
  if (!m_histSvc) {
    error() << "Unable to locate Histogram Service" << endmsg;
    return StatusCode::FAILURE;
  }
  m_pt = new TH2F("histo_eff_vs_pt", "Tracking Efficiency vs Transverse Momentum; seeding effiency; #p_{T}", 50, 0, 100, 2, -0.5, 1.5);
  if (m_histSvc->regHist("/rec/histo_eff_vs_pt", m_pt).isFailure()) {
    error() << "Couldn't register histo_eff_vs_pt histogram" << endmsg;
  }
  m_eta = new TH2F("histo_eff_vs_eta", "Tracking Efficiency vs Pseudorapidity; seeding efficiency; #eta", 50, 0, 6, 2, -0.5, 1.5);
  if (m_histSvc->regHist("/rec/histo_eff_vs_eta", m_eta).isFailure()) {
    error() << "Couldn't register histo_eff_vs_eta histogram" << endmsg;
  }
  m_cosTheta = new TH2F("histo_eff_vs_costheta", "Tracking Efficiency vs cosTheta", 50, 0, 1, 2, -0.5, 1.5);
  if (m_histSvc->regHist("/rec/histo_eff_vs_costheta", m_cosTheta).isFailure()) {
    error() << "Couldn't register histo_eff_vs_costheta histogram" << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode TrackEfficiencies::execute() {
  for (const auto& sim : *(m_simParticles.get())) {
    bool isFound = 0;
    for(const auto& rec: *(m_recParticles.get())) {
      if(rec.core().bits == sim.core().bits) {
        isFound = 1;
        continue;
      }
    }
    const fcc::BareParticle& core = sim.core();
    CLHEP::Hep3Vector mom(core.p4.px, core.p4.py, core.p4.pz);
    m_eta->Fill(mom.eta(), isFound);
    m_pt->Fill(mom.perp(), isFound);
    m_pt->Fill(mom.cosTheta(), isFound);
  }
  return StatusCode::SUCCESS;
}

StatusCode TrackEfficiencies::finalize() { return GaudiAlgorithm::finalize(); }
