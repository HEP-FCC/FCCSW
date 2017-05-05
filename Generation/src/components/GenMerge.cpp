
#include "GenMerge.h"

#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"

#include "GaudiKernel/IEventProcessor.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

DECLARE_COMPONENT(GenMerge)

GenMerge::GenMerge(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {

  declareProperty("signalGenParticles", m_partInSignal, "Generated signal particle collection (input)");
  declareProperty("signalGenVertices", m_vertInSignal, "Generated signal vertex collection (input)");

  declareProperty("pileupGenParticles", m_partInPileUp, "Generated pileup particle collection (input)");
  declareProperty("pileupGenVertices", m_vertInPileUp, "Generated pileup vertex collection (input)");

  declareProperty("allGenParticles", m_partOut, "Merged particle collection (output)");
  declareProperty("allGenVertices", m_vertOut, "Merged vertex collection (output)");
}

StatusCode GenMerge::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc.isSuccess()) return sc;
  return sc;
}

StatusCode GenMerge::execute() {
  auto collVSig = m_vertInSignal.get();
  auto collPSig = m_partInSignal.get();

  auto collVPil = m_vertInPileUp.get();
  auto collPPil = m_partInPileUp.get();

  // ownership given to data service at end of execute
  fcc::MCParticleCollection* collPOut = new fcc::MCParticleCollection();
  fcc::GenVertexCollection* collVOut = new fcc::GenVertexCollection();

  // copy signal vertices
  for (const auto elem : *collVSig) {
    collVOut->push_back(elem.clone());
  }
  // copy pileup vertices
  for (const auto elem : *collVPil) {
    collVOut->push_back(elem.clone());
  }
  // copy signal particles, updating the references
  for (const auto elem : *collPSig) {
    auto newPart = fcc::MCParticle(elem.core());
    // update reference: find new index in merged collection
    if (elem.startVertex().isAvailable()) {
      unsigned int newIdStart = elem.startVertex().getObjectID().index;
      // update startVertex

      newPart.startVertex((*collVOut)[newIdStart]);
    }
    if (elem.endVertex().isAvailable()) {
      // update reference: find new index in merged collection
      unsigned int newIdEnd = elem.endVertex().getObjectID().index;
      // update startVertex
      newPart.endVertex((*collVOut)[newIdEnd]);
    }
    collPOut->push_back(newPart);
  }
  // copy pileup particles, updating the references
  for (const auto elem : *collPPil) {
    auto newPart = fcc::MCParticle(elem.core());

    if (elem.startVertex().isAvailable()) {
      // update reference: find new index in merged collection
      // add offset since the signal particles are copied in new collection first
      unsigned int newIdStart = elem.startVertex().getObjectID().index + collVSig->size();
      // update startVertex
      newPart.startVertex((*collVOut)[newIdStart]);
    }
    if (elem.endVertex().isAvailable()) {
      // update reference: find new index in merged collection
      // add offset since the signal particles are copied in new collection first
      unsigned int newIdEnd = elem.endVertex().getObjectID().index + collVSig->size();
      // update startVertex
      newPart.endVertex((*collVOut)[newIdEnd]);
    }
    collPOut->push_back(newPart);
  }

  m_vertOut.put(collVOut);
  m_partOut.put(collPOut);
  return StatusCode::SUCCESS;
}

StatusCode GenMerge::finalize() { return GaudiAlgorithm::finalize(); }
