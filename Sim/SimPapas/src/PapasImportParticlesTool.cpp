#include "PapasImportParticlesTool.h"
// FCCSW
#include "SimG4Common/Units.h"
#include "datamodel/GenVertex.h"
#include "datamodel/LorentzVector.h"
#include "datamodel/MCParticleCollection.h"
// PAPAS
#include "papas/datatypes/Helix.h"
#include "papas/datatypes/IdCoder.h"
#include "papas/datatypes/Particle.h"
#include "papas/datatypes/Path.h"
#include "papas/detectors/Detector.h"
#include "papas/detectors/Field.h"
#include "papas/graphtools/DefinitionsNodes.h"
#include "papas/simulation/Simulator.h"
#include "papas/utility/PDebug.h"

DECLARE_TOOL_FACTORY(PapasImportParticlesTool)

PapasImportParticlesTool::PapasImportParticlesTool(const std::string& aType, const std::string& aName,
                                                   const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IPapasImportTool>(this);
  declareProperty("genparticles", m_genParticlesHandle);
}

PapasImportParticlesTool::~PapasImportParticlesTool() {}

StatusCode PapasImportParticlesTool::initialize() { return GaudiTool::initialize(); }

StatusCode PapasImportParticlesTool::clear() {
  m_particles.clear();
  return StatusCode::SUCCESS;
}

StatusCode PapasImportParticlesTool::finalize() { return GaudiTool::finalize(); }

StatusCode PapasImportParticlesTool::run(papas::Event& pevent, std::unordered_map<papas::Identifier, int>& links,
                                         std::shared_ptr<papas::Detector> spDetector) {
  const fcc::MCParticleCollection* ptcs = m_genParticlesHandle.get();
  int countp = 0;

  // We need to know the sorted order of fcc MCparticles by decreasing energy to match Python
  // Sort is required when run in PDebug mode. TODO: make this configurable .
  std::vector<fcc::ConstMCParticle> vecPtcs;
  std::list<int> orderPtcs;  // will contain the GenParticle index for the sorted Genparticles
  for (int n = 0; n < (*ptcs).size(); n++) {
    orderPtcs.push_back(n);
  }
  TLorentzVector tlv;
  TLorentzVector tlv2;

  // order the list of the indices so as to correspond to GenParticles in decreasing order of energy
  orderPtcs.sort([&tlv, &tlv2, ptcs](int a, int b) {
    auto p4 = (*ptcs)[a].p4();
    tlv.SetXYZM(p4.px, p4.py, p4.pz, p4.mass);
    p4 = (*ptcs)[b].p4();
    tlv2.SetXYZM(p4.px, p4.py, p4.pz, p4.mass);
    return tlv.E() > tlv2.E();
  });

  // Now construct papas simulated Particles from the genParticles (providing genParticles are stable)
  // making sure to create the Simulted particles in order of decreasing energy
  for (int index : orderPtcs) {
    auto ptc = (*ptcs)[index];
    countp += 1;
    auto p4 = ptc.core().p4;
    tlv.SetXYZM(p4.px, p4.py, p4.pz, p4.mass);
    int pdgid = ptc.core().pdgId;
    TVector3 startVertex = TVector3(0, 0, 0);
    if (ptc.startVertex().isAvailable()) {
      startVertex =
          TVector3(ptc.startVertex().x() * sim::edm2papas::length, ptc.startVertex().y() * sim::edm2papas::length,
                   ptc.startVertex().z() * sim::edm2papas::length);
    }
    if (ptc.core().status == 1) {  // only stable ones
      if (tlv.Pt() > 1e-5 && (abs(pdgid) != 12) && (abs(pdgid) != 14) && (abs(pdgid) != 16)) {
        papas::Particle particle(pdgid, (double)ptc.core().charge, tlv, m_particles.size(), 's', startVertex,
                                 ptc.core().status);
        std::shared_ptr<papas::Path> ppath;
        if (fabs(particle.charge()) < 0.5) {
          ppath = std::make_shared<papas::Path>(papas::Path(particle.p4(), particle.startVertex(), particle.charge()));
        } else {
          ppath = std::make_shared<papas::Helix>(papas::Helix(particle.p4(), particle.startVertex(), particle.charge(),
                                                              spDetector->field()->getMagnitude()));
        }
        particle.setPath(ppath);
        m_particles.emplace(particle.id(), particle);
        papas::PDebug::write("Made {}", particle);
        // Store the link between the GenParticle and the Papas Sim Particle
        links.emplace(particle.id(), index);
      }
    }
  }
  pevent.addCollectionToFolder(m_particles);
  return StatusCode::SUCCESS;
}
