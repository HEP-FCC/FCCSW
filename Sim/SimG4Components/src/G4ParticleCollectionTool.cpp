
// Include files 

// local
#include "SimG4Common/ParticleCollectionAction.h"

#include "G4ParticleCollectionTool.h"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4RunManager.hh"

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
	declareInput("genParticles",m_genParticles, "allGenParticles");
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

G4VUserPrimaryGeneratorAction* G4ParticleCollectionTool::getParticleGenerator()
{
	info()<<" G4ParticleCollectionTool::getParticleGenerator() "<<endmsg;

        ParticleCollectionAction *sp=new ParticleCollectionAction;
	sp->SetPrimaryCollection(&m_genParticles);

	return sp;
}
