#include "SimG4FastSimHistograms.h"

// FCCSW
#include "GaudiKernel/ITHistSvc.h"

// datamodel
#include "edm4hep/MCParticleCollection.h"
#include "edm4hep/ReconstructedParticleCollection.h"
#include "edm4hep/MCRecoParticleAssociationCollection.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "TH1F.h"

DECLARE_COMPONENT(SimG4FastSimHistograms)

SimG4FastSimHistograms::SimG4FastSimHistograms(const std::string& aName, ISvcLocator* aSvcLoc)
    : GaudiAlgorithm(aName, aSvcLoc) {
  declareProperty("particlesMCparticles", m_particlesMCparticles,
                  "Handle for the EDM particles and MC particles associations to be read");
}
SimG4FastSimHistograms::~SimG4FastSimHistograms() {}

StatusCode SimG4FastSimHistograms::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) return StatusCode::FAILURE;
  m_histSvc = service("THistSvc");
  if (!m_histSvc) {
    error() << "Unable to locate Histogram Service" << endmsg;
    return StatusCode::FAILURE;
  }
  m_p = new TH1F("SmP", "Smeared particles momentum", 100, 0, 100);
  if (m_histSvc->regHist("/rec/SmP", m_p).isFailure()) {
    error() << "Couldn't register SmP histogram" << endmsg;
  }
  m_diffP = new TH1F("DiffP", "Smeared-MC particles momentum", 100, -0.5, 0.5);
  if (m_histSvc->regHist("/rec/DiffP", m_diffP).isFailure()) {
    error() << "Couldn't register DifP histogram" << endmsg;
  }
  m_eta = new TH1F("SmEta", "Smeared particles pseudorapidity", 100, -10, 10);
  if (m_histSvc->regHist("/rec/SmEta", m_eta).isFailure()) {
    error() << "Couldn't register SmEta histogram" << endmsg;
  }
  m_pdg = new TH1F("SmPdg", "Smeared particles PDG code", 4500, -2250, 2249);
  if (m_histSvc->regHist("/rec/SmPdg", m_pdg).isFailure()) {
    error() << "Couldn't register SmPdg histogram" << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4FastSimHistograms::execute() {
  const auto associations = m_particlesMCparticles.get();
  for (const auto& assoc : *associations) {
    auto mom_edm = assoc.getRec().getMomentum();
    CLHEP::Hep3Vector mom(mom_edm.x, mom_edm.y, mom_edm.z);
    m_eta->Fill(mom.eta());
    m_p->Fill(mom.mag());
    m_pdg->Fill(core.pdgId);
    auto mom_edm_mc = assoc.getSim().getMomentum();
    CLHEP::Hep3Vector momMC(mom_edm_mc.x, mom_edm_mc.y, mom_edm_mc.z);
    m_diffP->Fill((momMC.mag() - mom.mag()) / momMC.mag());
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4FastSimHistograms::finalize() { return GaudiAlgorithm::finalize(); }
