// $Id: BcNoCut.cpp,v 1.4 2009-10-02 13:08:08 jhe Exp $
// Include files 

// local
#include "BcNoCut.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/SystemOfUnits.h"

// from Kernel
#include "Event/ParticleID.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// from Generators
#include "Event/HepMCUtils.h"
#include "MCInterfaces/IDecayTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BcNoCut
// 
// 2013-06-11 : Jibo He
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( BcNoCut )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BcNoCut::BcNoCut( const std::string& type,
                  const std::string& name,
                  const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_decayTool( 0 ) {
  declareInterface< IGenCutTool >( this ) ;
  declareProperty( "DecayTool" ,       m_decayToolName = "EvtGenDecay") ;
  m_sigBcPID = 541 ;

}

//=============================================================================
// Destructor 
//=============================================================================
BcNoCut::~BcNoCut( ) { ; }

//=============================================================================
// Initialize
//=============================================================================
StatusCode BcNoCut::initialize( ) {

  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiTool

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize and retrieve "
                                      << m_decayToolName << " tool" << endmsg;

  if ( "" != m_decayToolName )
    m_decayTool = tool< IDecayTool >( m_decayToolName ) ;

  m_decayTool -> setSignal( m_sigBcPID ) ;

  return StatusCode::SUCCESS;

}

//=============================================================================
// Finalize
//=============================================================================
StatusCode BcNoCut::finalize( ) {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  if ( 0 != m_decayTool ) release( m_decayTool );

  return GaudiTool::finalize(); 

}

//=============================================================================
// Acceptance function
//=============================================================================
bool BcNoCut::applyCut( ParticleVector & theParticleVector ,
                        const HepMC::GenEvent * theEvent  ,
                        const LHCb::GenCollision */* theHardInfo */ )
  const {
  
  // To see whether the B_c is in the Event or not
  //--------------------------------------------------------------------
  theParticleVector.clear( ) ;
  HepMC::GenEvent::particle_const_iterator it ;
  for ( it = theEvent -> particles_begin() ; 
        it != theEvent -> particles_end() ; ++it )
    if ( abs( (*it) -> pdg_id() ) == m_sigBcPID ) 
      if ( ( LHCb::HepMCEvent::DocumentationParticle != (*it) -> status() ) 
           && ( HepMCUtils::IsBAtProduction( *it ) ) )
        theParticleVector.push_back( *it ) ;
  std::sort( theParticleVector.begin() , theParticleVector.end() , 
             HepMCUtils::compareHepMCParticles ) ;
  
  if ( theParticleVector.empty() ) return false ;

  // To decay the signal particle
  //--------------------------------------------------------------------  
  bool hasFlipped = false ;
  HepMC::GenParticle * theSignal ;
  theSignal = theParticleVector.front() ;  
  m_decayTool -> generateSignalDecay( theSignal , hasFlipped ) ;
  
  return ( ! theParticleVector.empty() ) ;
}
