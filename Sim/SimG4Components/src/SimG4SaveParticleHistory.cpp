#include "SimG4SaveParticleHistory.h"

// FCCSW
#include "SimG4Common/ParticleInformation.h"
#include "SimG4Common/Units.h"

// Geant4
#include "G4Event.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"

// datamodel
#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"

DECLARE_TOOL_FACTORY(SimG4SaveParticleHistory)

SimG4SaveParticleHistory::SimG4SaveParticleHistory(const std::string& aType, const std::string& aName,
                                                   const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<ISimG4SaveOutputTool>(this);
  declareOutput("mcParticles", m_mcParticles, "sim/secondaries");
  declareOutput("genVertices", m_genVertices, "sim/vertices");
  // needed for AlgTool wit output/input until it appears in Gaudi AlgTool constructor
  declareProperty("DataInputs", inputDataObjects());
  declareProperty("DataOutputs", outputDataObjects());
}

StatusCode SimG4SaveParticleHistory::saveOutput(const G4Event& aEvent) {
  auto edmParticles = m_mcParticles.createAndPut();
  // this we'll need once we address the FIXME below (creating intermediate vertices to keep the history)
  // auto edmVertices = m_genVertices.createAndPut();
  auto nPrimVtcs = aEvent.GetNumberOfPrimaryVertex();
  for (int iVtx = 0; iVtx < nPrimVtcs; ++iVtx) {
    auto g4Vtx = aEvent.GetPrimaryVertex(iVtx);
    auto g4Primary = g4Vtx->GetPrimary();
    if (g4Primary == nullptr) {
      continue;
    }
    // get user information to establish primary particle relation in EDM
    auto userInfo = static_cast<sim::ParticleInformation*>(g4Primary->GetUserInformation());
    const auto& edmPrimary = userInfo->mcParticle();
    auto currDaughter = g4Primary->GetDaughter();
    bool haveDaughters = currDaughter != nullptr;
    std::cout << haveDaughters << std::endl;
    // FIXME this just adds the last particle in the "history" probably need a couple of properties that allow to select
    // what goes into the history and what doesn't
    while (haveDaughters) {
      auto nextDaughter = currDaughter->GetDaughter();
      if (nextDaughter != nullptr) {
        currDaughter = nextDaughter;
      } else {
        haveDaughters = false;
      }
    }
    if (currDaughter != nullptr) {
      auto particle = edmParticles->create();
      particle.p4().px = currDaughter->GetPx() * sim::g42edm::energy;
      particle.p4().py = currDaughter->GetPy() * sim::g42edm::energy;
      particle.p4().pz = currDaughter->GetPz() * sim::g42edm::energy;
      // TODO: set charge, status and position
      particle.pdgId(currDaughter->GetPDGcode());
      // create the link from which we can later construct the history
      particle.startVertex(edmPrimary.endVertex());
    }
  }
  return StatusCode::SUCCESS;
}
