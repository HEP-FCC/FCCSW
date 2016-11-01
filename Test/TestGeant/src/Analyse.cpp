#include "Analyse.h"

#include "datamodel/ParticleCollection.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"
#include "datamodel/ParticleMCParticleAssociationCollection.h"

DECLARE_COMPONENT(Analyse)

Analyse::Analyse(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc) {
   declareInput("particleassociation", m_partassociationhandle);
}

StatusCode Analyse::initialize() {
      return StatusCode::SUCCESS;
}

StatusCode Analyse::execute() {
   // always check if the units are converted properly !!
   const fcc::ParticleMCParticleAssociationCollection* associations = m_partassociationhandle.get();
   info()<<" START ANALYSIS"<<endmsg;
   for(const auto& assoc : *associations) {
      const fcc::BareParticle& mccore = assoc.sim().core();
      const fcc::BareParticle& core = assoc.rec().core();
      if(
         core.p4.px-mccore.p4.px < 0.00001 &&
         core.p4.py-mccore.p4.py < 0.00001 &&
         core.p4.pz-mccore.p4.pz < 0.00001
         ) {
         info()<<"PARTICLES SIM AND REC ARE IDENTICAL !!!!"<<endmsg;
         info()<<"\t"<<core.p4.px<<"\t"<<core.p4.py<<"\t"<<core.p4.pz<<"\t"<<endmsg;
      }
      else  {
         info()<<"CHANGE IN MOMENTUM !!!!"<<endmsg;
         info()<<"original:\t"<<core.p4.px<<"\t"<<core.p4.py<<"\t"<<core.p4.pz<<"\t"<<endmsg;
         info()<<"now   :\t"<<mccore.p4.px<<"\t"<<mccore.p4.py<<"\t"<<mccore.p4.pz<<"\t"<<endmsg;
      }
   }
   info()<<" END ANALYSIS"<<endmsg;
   return StatusCode::SUCCESS;
}

StatusCode Analyse::finalize() {
   return GaudiAlgorithm::finalize();
}
