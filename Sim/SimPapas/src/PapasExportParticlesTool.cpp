#include "PapasExportParticlesTool.h"
// FCCSW
#include "datamodel/GenVertex.h"
#include "datamodel/LorentzVector.h"
// PAPAS
#include "papas/datatypes/HistoryHelper.h"
#include "papas/datatypes/IdCoder.h"

DECLARE_TOOL_FACTORY(PapasExportParticlesTool)

PapasExportParticlesTool::PapasExportParticlesTool(const std::string& aType, const std::string& aName,
                                                   const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IPapasExportTool>(this);
  declareProperty("recparticles", m_recParticlesHandle);
  declareProperty("genparticles", m_genParticlesHandle);
  declareProperty("particleMCparticleAssociations", m_associationHandle);
}

StatusCode PapasExportParticlesTool::initialize() {
  debug() << "PAPAS export options:" << endmsg;
  return GaudiTool::initialize();
}

StatusCode PapasExportParticlesTool::createOutputStructures() {
  m_particles = m_recParticlesHandle.createAndPut();
  m_particleMCParticleAssociations = m_associationHandle.createAndPut();
  return StatusCode::SUCCESS;
}

StatusCode PapasExportParticlesTool::run(papas::Event& pevent, std::unordered_map<papas::Identifier, int>& links) {
  // convert each papas particle into an MCParticle
  std::string ptype = m_particleSubtype;
  papas::HistoryHelper history(pevent);

  // put the original gen particles into a vector so they can be used to
  // make the link association with the reconstructed particle
  const fcc::MCParticleCollection* ptcs = m_genParticlesHandle.get();

  for (const auto& pp : pevent.particles(ptype.c_str()[0])) {
    // make a fcc:Particle corresponding to the reconstructed particle
    fcc::Particle ptc = m_particles->create();
    ptc.core().pdgId = pp.second.pdgId();
    auto& p4 = ptc.core().p4;
    p4.px = pp.second.p4().Px();
    p4.py = pp.second.p4().Py();
    p4.pz = pp.second.p4().Pz();
    p4.mass = pp.second.p4().M();
    ptc.core().status = 1;
    ptc.core().charge = pp.second.charge();

    // Use the links stored during import together with the history information
    // To link the reconstructed particle to the simulated particle
    // and then the simulated particle to the Generated Particle.
    auto simIds = history.linkedIds(pp.second.id(), "ps");  // find linked SimParticles
    for (const auto& s : simIds) {
      auto search = links.find(s);  // find linked GenParticle index
      if (search != links.end()) {
        const auto& genParticle = (*ptcs)[search->second];  // find linked GenParticle
        fcc::ParticleMCParticleAssociation assoc = m_particleMCParticleAssociations->create();
        assoc.sim(genParticle);  // set up association
        assoc.rec(ptc);
      } else {
        warning() << "Associated simulated particle was not found." << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode PapasExportParticlesTool::finalize() { return GaudiTool::finalize(); }
