#include "Analyse.h"

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
   const ParticleMCAssociationCollection* associations = m_partassociationhandle.get();
   info()<<" START ANALYSIS"<<endmsg;
   for(const auto& assoc : *associations) {
      const MCParticleHandle mcpart = assoc.read().Sim;
      const ParticleHandle part = assoc.read().Rec;
      const BareParticle& mccore = mcpart.read().Core;
      const BareParticle& core = part.read().Core;
      if(
         core.P4.Px-mccore.P4.Px < 0.00001 &&
         core.P4.Py-mccore.P4.Py < 0.00001 &&
         core.P4.Pz-mccore.P4.Pz < 0.00001
         ) {
         info()<<"PARTICLES SIM AND REC ARE IDENTICAL !!!!"<<endmsg;
         info()<<"\t"<<core.P4.Px<<"\t"<<core.P4.Py<<"\t"<<core.P4.Pz<<"\t"<<endmsg;
      }
      else  {
         info()<<"CHANGE IN MOMENTUM !!!!"<<endmsg;
         info()<<"original:\t"<<core.P4.Px<<"\t"<<core.P4.Py<<"\t"<<core.P4.Pz<<"\t"<<endmsg;
         info()<<"now   :\t"<<mccore.P4.Px<<"\t"<<mccore.P4.Py<<"\t"<<mccore.P4.Pz<<"\t"<<endmsg;
      }
   }
   info()<<" END ANALYSIS"<<endmsg;
   return StatusCode::SUCCESS;
}

StatusCode Analyse::finalize() {
   return GaudiAlgorithm::finalize();
}
