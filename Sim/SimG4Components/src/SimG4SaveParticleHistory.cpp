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
  declareProperty("mcParticles", m_mcParticles, "Handle to the secondary particles");
  declareProperty("genVertices", m_genVertices, "Handle to the decay vertices");
}

void SimG4SaveParticleHistory::reset(sim::EventInformation* history) {
  m_mcParticleColl = m_mcParticles.createAndPut();
  m_genVertexColl = m_genVertices.createAndPut();
  history->setCollections(m_genVertexColl, m_mcParticleColl);
}

StatusCode SimG4SaveParticleHistory::saveOutput(const G4Event& /*aEvent*/) {
  debug() << "Saved " << m_mcParticleColl->size() << " particles from Geant4 history." << endmsg;

  return StatusCode::SUCCESS;
}
