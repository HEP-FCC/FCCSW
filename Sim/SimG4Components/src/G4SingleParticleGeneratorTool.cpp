
// Include files 

// local
#include "G4SingleParticleGeneratorTool.h"

#include "SimG4Common/SingleParticleGeneratorAction.h"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4RunManager.hh"


//-----------------------------------------------------------------------------
// Implementation file for class : G4SingleParticleGeneratorTool
//
// 2014-10-01 : Andrea Dell'Acqua
//-----------------------------------------------------------------------------

// Declaration of the Tool
DECLARE_COMPONENT( G4SingleParticleGeneratorTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
G4SingleParticleGeneratorTool::G4SingleParticleGeneratorTool( const std::string& type,
                                    const std::string& nam,const IInterface* parent )
  				  : GaudiTool ( type, nam , parent )
{
	declareInterface< IG4ParticleGeneratorTool >( this ) ;
	declareProperty("ParticleName",particleName="geantino");
	declareProperty("eMin",eMin=1*CLHEP::GeV);
	declareProperty("eMax",eMax=1*CLHEP::TeV);
	declareProperty("etaMin",etaMin=-5.);
	declareProperty("etaMax",etaMax=5);
	declareProperty("phiMin",phiMin=0.);
	declareProperty("phiMax",phiMax=2*M_PI);
	declareProperty("VertexX",vX=0);
	declareProperty("VertexY",vY=0);
	declareProperty("VertexZ",vZ=0);
}

//=============================================================================
// Destructor 
//=============================================================================

G4SingleParticleGeneratorTool::~G4SingleParticleGeneratorTool()
{
}

//=============================================================================
// Initialize 
//=============================================================================
StatusCode G4SingleParticleGeneratorTool::initialize( )
{
	info()<<" G4SingleParticleGeneratorTool::initialize( ) "<<endmsg;
	info()<<" Particle = " <<particleName<<endmsg;
	StatusCode sc = GaudiTool::initialize( ) ;
	if ( sc.isFailure() ) return sc ;
	info()<<" done!!! G4SingleParticleGeneratorTool::initialize( ) "<<endmsg;
	return sc;
}


//=============================================================================
// Finalize 
//=============================================================================
StatusCode G4SingleParticleGeneratorTool::finalize( )
{
	info()<<" G4SingleParticleGeneratorTool::finalize( ) "<<endmsg;
	StatusCode sc = GaudiTool::initialize( ) ;
	return sc;
}

G4VUserPrimaryGeneratorAction* G4SingleParticleGeneratorTool::getParticleGenerator() const
{
	info()<<" G4SingleParticleGeneratorTool::getParticleGenerator() "<<endmsg;

        SingleParticleGeneratorAction *sp=new SingleParticleGeneratorAction;
        sp->SetParticleType(particleName);
        sp->SetEnergyRange(eMin,eMax);
        sp->SetEtaRange(etaMin,etaMax);
        sp->SetPhiRange(phiMin,phiMax);
        sp->SetVertexPosition(vX,vY,vZ);

	return sp;
}
