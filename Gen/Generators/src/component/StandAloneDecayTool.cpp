// $Id: StandAloneDecayTool.cpp,v 1.4 2008-07-24 22:06:07 robbep Exp $
// Include files 
#include "StandAloneDecayTool.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "Event/IParticlePropertySvc.h"
#include "Event/ParticleProperty.h"

// from Generators
#include "MCInterfaces/IDecayTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : StandAloneDecayTool
//
// 2006-04-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the tool factory

DECLARE_TOOL_FACTORY( StandAloneDecayTool )

//=============================================================================
// const constructor, initializes variables
//=============================================================================
StandAloneDecayTool::StandAloneDecayTool( const std::string& type ,
                                          const std::string& name ,
                                          const IInterface * parent )
  : Signal ( type , name , parent ) , m_signalMass( 0. ) {
    declareProperty ( "Inclusive" , m_inclusive = false ) ;
  } 
//=============================================================================
// Destructor
//=============================================================================
StandAloneDecayTool::~StandAloneDecayTool() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode StandAloneDecayTool::initialize() {
  StatusCode sc = Signal::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  LHCb::IParticlePropertySvc * ppSvc = 
    svc< LHCb::IParticlePropertySvc >( "ParticlePropertySvc" ) ;
  const LHCb::ParticleProperty * prop = ppSvc -> find( LHCb::ParticleID( *m_pids.begin() ) ) ;
  m_signalMass = prop -> mass() ;

  release( ppSvc ) ;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
bool StandAloneDecayTool::generate( const unsigned int nPileUp , 
                                    LHCb::HepMCEvents * theEvents ,
                                    LHCb::GenCollisions * theCollisions ) {
  // prepare event
  LHCb::GenCollision * theGenCollision( 0 ) ;
  HepMC::GenEvent * theGenEvent( 0 ) ;

  // generate the requested number of "pile-up" events
  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    prepareInteraction( theEvents , theCollisions , theGenEvent , 
                        theGenCollision ) ;
    
    // Particle to decay
    HepMC::GenParticle * theParticle = new HepMC::GenParticle( ) ;
    theParticle -> 
      set_momentum( HepMC::FourVector( 0. , 0. , 0., m_signalMass ) ) ;

    // Decay the particle at (0,0,0,0)
    HepMC::GenVertex * theVertex = 
      new HepMC::GenVertex( HepMC::FourVector( 0., 0., 0., 0. ) ) ;
    theGenEvent -> add_vertex( theVertex ) ;
    theVertex -> add_particle_out( theParticle ) ;
    
    bool flip( false ) ;
    
    int thePID = *m_pids.begin() ;
    if ( m_cpMixture ) {
      // decide the PID to generate
      double flavour = m_flatGenerator() ;
      m_decayTool -> enableFlip() ;
      
      if ( flavour < 0.5 ) 
        theParticle -> set_pdg_id( +abs( thePID ) ) ;
      else
        theParticle -> set_pdg_id( -abs( thePID ) ) ;
    } else {
      m_decayTool -> disableFlip() ;
      theParticle -> set_pdg_id( thePID ) ;
    }

    if ( ! m_inclusive ) 
      m_decayTool -> generateSignalDecay( theParticle , flip ) ;
    else 
      m_decayTool -> generateDecay( theParticle ) ;
    
    theParticle -> set_status( LHCb::HepMCEvent::SignalInLabFrame ) ;
  
    theGenEvent -> 
      set_signal_process_vertex( theParticle -> end_vertex() ) ;
    theGenCollision -> setIsSignal( true ) ;
  }
  
  return true ;
}

//=============================================================================

