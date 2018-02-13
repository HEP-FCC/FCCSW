#include "SimG4SaveParticleHistory.h"

// FCCSW
#include "SimG4Common/ParticleInformation.h"
#include "SimG4Common/Units.h"

// Geant4
#include "G4Event.hh"
#include "G4EventManager.hh"
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


StatusCode SimG4SaveParticleHistory::saveOutput(const G4Event& aEvent) {
  auto evtinfo = dynamic_cast<sim::EventInformation*>(aEvent.GetUserInformation());
  // take over ownership of particle and vertex collections
  evtinfo->setCollections(m_genVertexColl, m_mcParticleColl);
  info() << "Saved " << m_mcParticleColl->size() << " particles from Geant4 history." << endmsg;
  m_mcParticles.put(m_mcParticleColl);
  m_genVertices.put(m_genVertexColl);

  return StatusCode::SUCCESS;
}
