#include "SimG4Common/ParticleCollectionAction.h"
#include "SimG4Common/Units.h"
#include "SimG4Common/ParticleInformation.h"

#include "G4Event.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

// datamodel
#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"

ParticleCollectionAction::ParticleCollectionAction()
{
}

void ParticleCollectionAction::SetPrimaryCollection(DataHandle<fcc::MCParticleCollection>* mc )
{
	m_particles=mc;
}

void ParticleCollectionAction::GeneratePrimaries(G4Event* anEvent)
{
  // Adding one particle per one vertex => vertices repeated
        const fcc::MCParticleCollection* m_genParticles=m_particles->get(); 
  	for(const auto& mcparticle : *m_genParticles) {
    		const fcc::ConstGenVertex& v = mcparticle.StartVertex();
    		G4PrimaryVertex* g4_vertex = new G4PrimaryVertex
      			(v.Position().X*sim::edm2g4::length, v.Position().Y*sim::edm2g4::length, v.Position().Z*sim::edm2g4::length, v.Ctau()*sim::edm2g4::length);
    		const fcc::BareParticle& mccore = mcparticle.Core();
    		G4PrimaryParticle* g4_particle = new G4PrimaryParticle
      			(mccore.Type, mccore.P4.Px*sim::edm2g4::energy, mccore.P4.Py*sim::edm2g4::energy, mccore.P4.Pz*sim::edm2g4::energy);
    		g4_particle->SetUserInformation(new sim::ParticleInformation(mcparticle));
    		g4_vertex->SetPrimary(g4_particle);
    		anEvent->AddPrimaryVertex(g4_vertex);
	}
}
