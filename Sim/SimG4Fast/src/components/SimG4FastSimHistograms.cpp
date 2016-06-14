#include "SimG4FastSimHistograms.h"

// FCCSW
#include "GaudiKernel/ITHistSvc.h"

// datamodel
#include "datamodel/ParticleCollection.h"
#include "datamodel/ParticleMCParticleAssociationCollection.h"
#include "datamodel/MCParticleCollection.h"

#include "TH1F.h"
#include "CLHEP/Vector/ThreeVector.h"


DECLARE_ALGORITHM_FACTORY(SimG4FastSimHistograms)

SimG4FastSimHistograms::SimG4FastSimHistograms(const std::string& aName, ISvcLocator* aSvcLoc):
GaudiAlgorithm(aName, aSvcLoc),
  m_p(nullptr),
  m_diffP(nullptr),
  m_pdg(nullptr){
  declareInput("particles", m_smParticles);
  declareInput("particlesMCparticles", m_particlesMCparticles);
}
SimG4FastSimHistograms::~SimG4FastSimHistograms() {}

StatusCode SimG4FastSimHistograms::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;
  m_histSvc = service("THistSvc");
  if (! m_histSvc) {
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
  const auto particles = m_smParticles.get();
  const auto associations = m_particlesMCparticles.get();
  for(const auto& part: *particles) {
    const fcc::BareParticle& core = part.Core();
    CLHEP::Hep3Vector mom(core.P4.Px, core.P4.Py, core.P4.Pz);
    m_eta->Fill(mom.eta());
    m_p->Fill(mom.mag());
    m_pdg->Fill(core.Type);
    // TODO fix EDM with relation here (no if needed)
    for(const auto& assoc: *associations) {
      const fcc::BareParticle& coreA = assoc.Rec().Core();
      if(coreA.Type == core.Type &&
        coreA.P4.Px == core.P4.Px &&
        coreA.P4.Py == core.P4.Py &&
        coreA.P4.Pz == core.P4.Pz &&
        coreA.P4.Mass == core.P4.Mass &&
        coreA.Vertex.X == core.Vertex.X &&
        coreA.Vertex.Y == core.Vertex.Y &&
        coreA.Vertex.Z == core.Vertex.Z &&
        coreA.Status == core.Status &&
        coreA.Charge == core.Charge) {
        const fcc::BareParticle& coreMC = assoc.Sim().Core();
        CLHEP::Hep3Vector momMC(coreMC.P4.Px, coreMC.P4.Py, coreMC.P4.Pz);
        m_diffP->Fill((momMC.mag()-mom.mag())/momMC.mag());
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4FastSimHistograms::finalize() {
  return GaudiAlgorithm::finalize();
}
