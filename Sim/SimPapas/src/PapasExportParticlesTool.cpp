#include "PapasExportParticlesTool.h"
// FCCSW
#include "datamodel/GenVertex.h"
#include "datamodel/LorentzVector.h"
//PAPAS
#include "papas/datatypes/HistoryHelper.h"
#include "papas/datatypes/IdCoder.h"


DECLARE_TOOL_FACTORY(PapasExportParticlesTool)

PapasExportParticlesTool::PapasExportParticlesTool(const std::string& aType, const std::string& aName,
                                                   const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<IPapasExportTool>(this);
  declareProperty("recparticles", m_particlesHandle);
  declareProperty("genparticles", m_iGenpHandle);
  declareProperty("particleMCparticleAssociations", m_associationHandle);
}

StatusCode PapasExportParticlesTool::initialize() {
  debug() << "PAPAS export options:" << endmsg;
  return GaudiTool::initialize();
}

StatusCode PapasExportParticlesTool::createOutputStructures() {
  m_particles = m_particlesHandle.createAndPut();
  m_particleMCParticleAssociations = m_associationHandle.createAndPut();
  return StatusCode::SUCCESS;
}

StatusCode PapasExportParticlesTool::run(papas::Event& pevent, std::unordered_map<papas::Identifier, int>& links) {
  // convert each papas particle into an MCParticle
  std::string ptype = m_particleSubtype;
  papas::HistoryHelper history(pevent);
  
  //put the original gen particles into a vector so they can be used to
  //make the association with the reconstructed particle
  const fcc::MCParticleCollection* ptcs = m_iGenpHandle.get();
  std::vector<fcc::ConstMCParticle> vecPtcs;
  for (const auto& p : *ptcs) {
     vecPtcs.push_back(p);
  }
  
  for (const auto& pp : pevent.particles(ptype.c_str()[0])) {
    //make a fcc:Particle corresponding to the reconstructed particle
    fcc::Particle ptc = m_particles->create();
    ptc.core().pdgId = pp.second.pdgId();
    auto& p4 = ptc.core().p4;
    p4.px = pp.second.p4().Px();
    p4.py = pp.second.p4().Py();
    p4.pz = pp.second.p4().Pz();
    p4.mass = pp.second.p4().M();
    ptc.core().status = 1;
    ptc.core().charge = pp.second.charge();
    
    auto simIds = history.linkedIds(pp.second.id(),"ps");
    for (const auto& s: simIds){
      auto search = links.find(s);
      if(search != links.end()) {
        const auto& genParticle= vecPtcs[search->second];
        fcc::ParticleMCParticleAssociation assoc = m_particleMCParticleAssociations->create();
        assoc.sim(genParticle);
        assoc.rec(ptc);
      }
      else {
        warning() << "Associated simulated particle was not found." << endmsg;
      }
    }
    /*//now use papas history to see which papas sim particles are linked to the papas rec particle
    const auto& hist = pevent.history();
    const auto& startnode = hist.at(pp.second.id());
    DAG::BFSRecurseVisitor<papas::PFNode> bfs;
    const auto& nodes = bfs.traverseNodes(startnode, DAG::enumVisitType::UNDIRECTED);
#if WITHSORT
    debug() << "Papas: sorting" << endmsg;
    auto simIds = history.linkedIds(pp.second.id());
#endif
    //std::unordered_set<papas::Identifier> simIds = history.linkedIds(pp.second.id());
    for (const auto& n: nodes) {
      if (papas::IdCoder::typeAndSubtype(n->value())=="ps") {
        //use the links information to find the relevant gen particle
        //and create the association
        auto search = links.find(n->value());
        if(search != links.end()) {
          const auto& genParticle= vecPtcs[search->second];
          fcc::ParticleMCParticleAssociation assoc = m_particleMCParticleAssociations->create();
          assoc.sim(genParticle);
          assoc.rec(ptc);
        }
        else {
          warning() << "Associated simulated particle was not found." << endmsg;
        }
      }
    }*/
    /*auto ids = history.linkedIds(pp.second.id(),"ps", DAG::enumVisitType::UNDIRECTED);
    std::cout << ids.size() <<"ONE";
    
    
    
    }*/
  }
  return StatusCode::SUCCESS;
}

StatusCode PapasExportParticlesTool::finalize() { return GaudiTool::finalize(); }
