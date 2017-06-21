#include "PapasExportParticlesTool.h"
#include "datamodel/GenVertex.h"
#include "datamodel/LorentzVector.h"

DECLARE_TOOL_FACTORY(PapasExportParticlesTool)

PapasExportParticlesTool::PapasExportParticlesTool(const std::string& aType, const std::string& aName,
                                                   const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IPapasTool>(this);
  declareProperty("recparticles", m_particlesHandle);
}

StatusCode PapasExportParticlesTool::initialize() {
  debug() << "PAPAS export options:" << endmsg;
  return GaudiTool::initialize();
}

StatusCode PapasExportParticlesTool::createOutputStructures() {
  m_particles = m_particlesHandle.createAndPut();
  return StatusCode::SUCCESS;
}

StatusCode PapasExportParticlesTool::run(papas::Event& pevent) {
  // convert each papas particle into an MCParticle
  std::string ptype = m_particleSubtype;
  for (const auto& pp : pevent.particles(ptype.c_str()[0])) {
    fcc::Particle ptc = m_particles->create();
    ptc.core().pdgId = pp.second.pdgId();
    auto& p4 = ptc.core().p4;
    p4.px = pp.second.p4().Px();
    p4.py = pp.second.p4().Py();
    p4.pz = pp.second.p4().Pz();
    p4.mass = pp.second.p4().M();
    ptc.core().status = 1;
    ptc.core().charge = pp.second.charge();
  }
  return StatusCode::SUCCESS;
}

StatusCode PapasExportParticlesTool::finalize() { return GaudiTool::finalize(); }
