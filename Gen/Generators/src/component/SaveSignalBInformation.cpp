// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from Event
#include "Event/HepMCEvent.h"

// from Generators
#include "Event/HepMCUtils.h"

// local
#include "SaveSignalBInformation.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SaveSignalBInformation
//
// 2013-06-26 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( SaveSignalBInformation )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SaveSignalBInformation::SaveSignalBInformation( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputHepMCEvent" , m_inputHepMCEvent =
                   LHCb::HepMCEventLocation::Default ) ;
  declareProperty( "OutputHepMCEvent" , m_outputHepMCEvent = 
                   "Gen/BInfo" ) ;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode SaveSignalBInformation::execute() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // loop over input HepMC
  SmartDataPtr< LHCb::HepMCEvents > hepMCptr( eventSvc() , 
                                              m_inputHepMCEvent ) ;
  
  StatusCode sc = StatusCode::SUCCESS ;
  
  if ( 0 == hepMCptr ) {
    info() << "No HepMCEvents at location " << m_inputHepMCEvent 
           << endmsg ;
  } else {
    LHCb::HepMCEvents::iterator it ;
    for ( it = hepMCptr -> begin() ; it != hepMCptr -> end() ; ++it ) {
      // check if signal exists
      if ( 0 != (*it) -> pGenEvt() -> signal_process_vertex() ) {
        sc = extractSignal( (*it) -> pGenEvt() -> signal_process_vertex() ) ;
        break ;
      }
    }
  }

  return sc ;
}

//=============================================================================
// Extract B string and copy to a new location
//=============================================================================
StatusCode SaveSignalBInformation::extractSignal( const HepMC::GenVertex *
                                                  theVertex ) const {
  HepMC::GenParticle * HEPB0 = *( theVertex -> particles_in_const_begin() ) ;
  HepMC::GenParticle * Bstring = 0 ;

  // look for the string associated to the signal
  for ( HepMC::GenVertex::particle_iterator iances = 
          HEPB0->production_vertex()-> particles_begin(HepMC::ancestors);
        iances != HEPB0->production_vertex()-> particles_end(HepMC::ancestors) ;
        ++iances ) {
    int genid = abs( (*iances) -> pdg_id() ) ;
    if ( ( 91 == genid ) || ( 92 == genid ) ) { 
      Bstring = (*iances) ; break ; 
    }
  }

  if ( 0 != Bstring ) {
    // copy the string in a new event
    LHCb::HepMCEvent * mcevt = new LHCb::HepMCEvent( ) ;
    mcevt -> setGeneratorName( "String" ) ;
    
    if ( 0 == Bstring -> production_vertex() ) 
      return Error( "Bstring particle has no production vertex." ) ;
  
    // create a new vertex and a new HepMC Particle for the root particle
    // (a copy of which will be associated to the new HepMC event) 
    
    HepMC::GenVertex * newVertex =
      new HepMC::GenVertex( Bstring -> production_vertex() -> position() ) ;

    HepMC::GenEvent * hepMCevt = mcevt -> pGenEvt() ;
    
    hepMCevt -> add_vertex( newVertex ) ;
    
    HepMC::GenParticle * theNewParticle =
      new HepMC::GenParticle( Bstring -> momentum() , Bstring -> pdg_id() ,
                              Bstring -> status() ) ;
    
    newVertex -> add_particle_out( theNewParticle ) ;
    
    // Associate the new particle to the HepMC event
    StatusCode sc = fillHepMCEvent( theNewParticle , Bstring ) ;
    hepMCevt -> set_signal_process_vertex( theNewParticle -> production_vertex() ) ;
  
    if ( ! sc.isSuccess( ) ) 
      return Error( "Could not fill HepMC event for signal tree" , sc ) ;
                            
    // Check if container already exists
    if ( exist< LHCb::HepMCEvents >( m_outputHepMCEvent ) ) 
      return Error( "B string container already exists !" ) ;
  
    LHCb::HepMCEvents * hepVect = new LHCb::HepMCEvents ;
    hepVect -> insert( mcevt ) ;
    
    // Register new location and store HepMC event
    put( hepVect , m_outputHepMCEvent ) ;
    
    return sc ;
  }
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Fill HepMC event from a HepMC tree
//=============================================================================
StatusCode SaveSignalBInformation::fillHepMCEvent( HepMC::GenParticle * 
                                                   theNewParticle ,
                                                   const HepMC::GenParticle * 
                                                   theOldParticle ) 
  const {
  StatusCode sc = StatusCode::SUCCESS ;
  //
  // Copy theOldParticle to theNewParticle in theEvent
  // theNewParticle already exist and is created outside this function
  HepMC::GenVertex * oVertex = theOldParticle -> end_vertex() ;
  if ( 0 != oVertex ) {
    // Create decay vertex and associate it to theNewParticle
    HepMC::GenVertex * newVertex =
      new HepMC::GenVertex( oVertex -> position() ) ;
    newVertex -> add_particle_in( theNewParticle ) ;
    theNewParticle -> parent_event() -> add_vertex( newVertex ) ;

    // loop over child particle of this vertex after sorting them
    std::list< const HepMC::GenParticle * > outParticles ;
    for ( HepMC::GenVertex::particles_out_const_iterator itP = 
            oVertex -> particles_out_const_begin() ; 
          itP != oVertex -> particles_out_const_end() ; ++itP )
      outParticles.push_back( (*itP ) ) ;

    outParticles.sort( HepMCUtils::compareHepMCParticles ) ;

    std::list< const HepMC::GenParticle * >::const_iterator child ;
    for ( child = outParticles.begin( ) ; child != outParticles.end( ) ; 
          ++child ) {
      
      // Create a new particle for each daughter of theOldParticle
      HepMC::GenParticle * newPart =
        new HepMC::GenParticle ( (*child) -> momentum () ,
                                 (*child) -> pdg_id ()   ,
                                 (*child) -> status ()   ) ;
      newVertex -> add_particle_out( newPart ) ;
      
      const HepMC::GenParticle * theChild = (*child) ;
      // Recursive call : fill the event with the daughters
      sc = fillHepMCEvent( newPart , theChild ) ;
      
      if ( ! sc.isSuccess() ) return sc ;
    }
  }
  return sc ;
}
