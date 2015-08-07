// $Id: RepeatDecay.cpp,v 1.1 2007-04-01 21:28:12 robbep Exp $
// Include files 

// local
#include "RepeatDecay.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// from Generators
#include "Generators/ISampleGenerationTool.h"
#include "Event/HepMCUtils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RepeatDecay
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( RepeatDecay )

//=======================================================
// Standard constructor, initializes variables
//=======================================================
RepeatDecay::RepeatDecay( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent )
  : GaudiTool  ( type, name , parent )  ,
    m_baseTool( 0 ) ,
    m_nRedecay( 0 )  { 
  declareInterface< ISampleGenerationTool >( this ) ;
  declareProperty( "BaseTool" , m_baseToolName = "Inclusive" ) ;
  declareProperty( "NRedecay" , m_nRedecayLimit = 50 ) ;
}

//=======================================================
// Destructor
//=======================================================
RepeatDecay::~RepeatDecay( ) { ; }

//=======================================================
// Initialize method
//=======================================================
StatusCode RepeatDecay::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( ! sc.isSuccess() ) return Error( "Cannot initialize base class !" ) ;

  if ( "" == m_baseToolName ) 
    return Error( "No name given for Base Tool" ) ;

  info() << "Generating repeated events using base events of type "
         << m_baseToolName << endmsg ;

  m_baseTool = tool< ISampleGenerationTool >( m_baseToolName , this ) ;

  m_theMemorizedEvents.clear() ;
  m_theMemorizedCollisions.clear() ;

  return sc ;
}

//===================================================
// Generate Set of Event for Minimum Bias event type
//===================================================
bool RepeatDecay::generate( const unsigned int nPileUp , 
                            LHCb::HepMCEvents * theEvents , 
                            LHCb::GenCollisions * theCollisions ) {
  bool result = false ;

  if ( ( m_theMemorizedEvents.empty() ) || 
       ( m_nRedecay > m_nRedecayLimit ) ) {
    m_theMemorizedEvents.clear() ;
    m_theMemorizedCollisions.clear() ;
    m_nRedecay = 0 ;
    
    result = m_baseTool -> generate( nPileUp , theEvents , theCollisions ) ;  
    if ( result ) {
      // Erase decays before storing the event
      LHCb::HepMCEvents::iterator it ;
      std::vector< HepMC::GenParticle * > eraseList ;
      
      for ( it = theEvents -> begin() ; it != theEvents -> end() ; ++it ) {
        HepMC::GenEvent * ev = (*it) -> pGenEvt() ;
        for ( HepMC::GenEvent::particle_iterator itP = ev -> particles_begin()
                ; itP != ev -> particles_end() ; ++itP ) {
          if ( LHCb::HepMCEvent::DecayedByDecayGenAndProducedByProdGen ==
               (*itP) -> status() )
            eraseList.push_back( (*itP) ) ;
        }
      }
      
      for ( std::vector< HepMC::GenParticle * >::iterator part = 
              eraseList.begin() ;
            part != eraseList.end() ; ++part ) 
        HepMCUtils::RemoveDaughters( (*part) ) ;

      copyEvents( theEvents , &m_theMemorizedEvents ) ;
      copyCollisions( theCollisions , &m_theMemorizedCollisions , theEvents ) ;
    }
  } else {
    ++m_nRedecay ;
    
    copyEvents( &m_theMemorizedEvents , theEvents ) ;
    copyCollisions( &m_theMemorizedCollisions , theCollisions , 
                    &m_theMemorizedEvents ) ;
    result = true ;
  }
  
  return result ;
}

//===================================================
// Print the counters
//===================================================
void RepeatDecay::printCounters( ) const {
  m_baseTool -> printCounters() ;
}

//===================================================
// Copy a HepMCEvent to another
//===================================================
void RepeatDecay::copyEvents( LHCb::HepMCEvents * from , 
                              LHCb::HepMCEvents * to ) {
  // Erase the event where to copy
  to -> clear() ;

  LHCb::HepMCEvents::iterator it ;
  for ( it = from -> begin() ; it != from -> end() ; ++it ) {
    LHCb::HepMCEvent * theHepMCEvent = new LHCb::HepMCEvent( ) ;
    theHepMCEvent -> setGeneratorName( (*it) -> generatorName() ) ;
    (*theHepMCEvent -> pGenEvt()) = (*(*it) -> pGenEvt()) ;
    to -> insert( theHepMCEvent ) ;
  } 
}

//===================================================
// Copy a HepMCEvent to another
//===================================================
void RepeatDecay::copyCollisions( LHCb::GenCollisions * from , 
                                  LHCb::GenCollisions * to ,
                                  LHCb::HepMCEvents * theEvents ) {
  // Erase the event where to copy
  to -> clear() ;

  LHCb::GenCollisions::iterator it ;
  LHCb::HepMCEvents::iterator itEvents = theEvents -> begin() ;
  
  for ( it = from -> begin() ; it != from -> end() ; ++it ) {
    LHCb::GenCollision * theGenCollision = new LHCb::GenCollision( ) ;
    theGenCollision -> setIsSignal( (*it) -> isSignal() ) ;
    theGenCollision -> setProcessType( (*it) -> processType() ) ;
    theGenCollision -> setSHat( (*it) -> sHat() ) ;
    theGenCollision -> setTHat( (*it) -> tHat() ) ;
    theGenCollision -> setUHat( (*it) -> uHat() ) ;
    theGenCollision -> setPtHat( (*it) -> ptHat() ) ;
    theGenCollision -> setX1Bjorken( (*it) -> x1Bjorken() ) ;
    theGenCollision -> setX2Bjorken( (*it) -> x2Bjorken() ) ;
    theGenCollision -> setEvent( (*itEvents) ) ;
    
    to -> insert( theGenCollision ) ;
    ++itEvents ;
  } 
}
