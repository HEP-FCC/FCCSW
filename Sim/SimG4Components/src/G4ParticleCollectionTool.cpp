
// Include files 

// local
#include "SimG4Common/Units.h"
#include "SimG4Common/ParticleInformation.h"
#include "G4ParticleCollectionTool.h"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4RunManager.hh"

// datamodel
#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"

//-----------------------------------------------------------------------------
// Implementation file for class : G4ParticleCollectionTool
//
// 2016-01-11 : Andrea Dell'Acqua
//-----------------------------------------------------------------------------

// Declaration of the Tool
DECLARE_COMPONENT( G4ParticleCollectionTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
G4ParticleCollectionTool::G4ParticleCollectionTool( const std::string& type,
                                    const std::string& nam,const IInterface* parent )
  				  : GaudiTool ( type, nam , parent )
{
	declareInput("genParticles",m_genParticles, "allSimParticles");
}

//=============================================================================
// Destructor 
//=============================================================================

G4ParticleCollectionTool::~G4ParticleCollectionTool()
{
}

//=============================================================================
// Initialize 
//=============================================================================
StatusCode G4ParticleCollectionTool::initialize( )
{
	info()<<" G4ParticleCollectionTool::initialize( ) "<<endmsg;
	StatusCode sc = GaudiTool::initialize( ) ;
	if ( sc.isFailure() ) return sc ;
	info()<<" done!!! G4ParticleCollectionTool::initialize( ) "<<endmsg;
	return sc;
}

G4VUserPrimaryGeneratorAction* G4ParticleCollectionTool::getParticleGenerator() const
{
	info()<<" G4ParticleCollectionTool::getParticleGenerator() "<<endmsg;

        ParticleCollectionAction *sp=new ParticleCollectionAction;

	return sp;
}

ParticleCollectionAction::ParticleCollectionAction()
{
	// std::cout<<"this is ParticleCollectionAction::ParticleCollectionAction()"<<std::endl;
}

void ParticleCollectionAction::SetPrimaryCollection(DataHandle<fcc::MCParticleCollection> mc )
{
	m_particles=const_cast<fcc::MCParticleCollection*>(mc.get());
}

void ParticleCollectionAction::GeneratePrimaries(G4Event* anEvent)
{
  // Adding one particle per one vertex => vertices repeated
  	for(const auto& mcparticle : *m_particles) {
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
