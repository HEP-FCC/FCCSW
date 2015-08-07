// $Id: Special.cpp,v 1.13 2009-09-20 17:53:02 robbep Exp $
// Include files 

// local
#include "Special.h"
// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
// from Kernel
#include "MCInterfaces/IGenCutTool.h"

// from Generators
#include "Generators/IProductionTool.h"
#include "Generators/GenCounters.h"
//TODO #include "Generators/LhaPdf.h"
#include "Generators/ICounterLogFile.h"

// Event 
#include "Event/HepMCEvent.h"
#include "Event/GenCollision.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Special
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( Special )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Special::Special( const std::string & type , const std::string & name ,
                  const IInterface * parent )
  : ExternalGenerator( type, name , parent ) ,
    m_xmlLogTool( 0 ) ,
    m_nEventsBeforeCut    (  0 ) ,
    m_nEventsAfterCut     (  0 ) ,
    m_maxInteractions     ( 30 ) ,
    m_pileUpProductionTool(  0 ) { 
  declareProperty( "PileUpProductionTool" , 
                   m_pileUpProductionToolName = 
                   "PythiaProduction/MinimumBiasPythiaProduction" ) ;
  declareProperty( "ReinitializePileUpGenerator" ,
                   m_reinitialize = true ) ;
}

//=============================================================================
// Destructor
//=============================================================================
Special::~Special( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode Special::initialize( ) {
  info() << "Generating Special events." << endmsg ;
  StatusCode sc = ExternalGenerator::initialize( ) ;

  // Initialize XML Log file
  m_xmlLogTool = tool< ICounterLogFile >( "XmlCounterLogFile" ) ;

  // Switch off LHA print out first
  /* TODO
  if ( msgLevel( MSG::DEBUG ) ) {
    LhaPdf::lhacontrol().setlhaparm( 19 , "DEBUG" ) ;
  }
  else {
    LhaPdf::lhacontrol().setlhaparm( 19 , "SILENT" ) ;
  }
  */
  m_pileUpEventsVector.clear() ;
  m_pileUpCollisionsVector.clear() ;

  return sc ;
}

//=============================================================================
// Finalize method
//=============================================================================
StatusCode Special::finalize( ) {
  std::vector< HepMC::GenEvent * >::iterator iter ;
  for ( iter = m_pileUpEventsVector.begin() ; 
        iter != m_pileUpEventsVector.end() ; ++iter ) 
    delete (*iter) ;
  m_pileUpEventsVector.clear() ;
  
  std::vector< LHCb::GenCollision * >::iterator iterC ;
  for ( iterC = m_pileUpCollisionsVector.begin() ; 
        iterC != m_pileUpCollisionsVector.end() ; ++iterC ) 
    delete (*iterC) ;
  m_pileUpCollisionsVector.clear() ;

  if ( 0 != m_pileUpProductionTool ) release( m_pileUpProductionTool ) ;  

  return ExternalGenerator::finalize() ;
}


//=============================================================================
// Generate Set of Event for Minimum Bias event type
//=============================================================================
bool Special::generate( const unsigned int nPileUp , 
                        LHCb::HepMCEvents * theEvents , 
                        LHCb::GenCollisions * theCollisions ) {
  StatusCode sc ;
  LHCb::GenCollision * theGenCollision( 0 ) ;
  HepMC::GenEvent * theGenEvent( 0 ) ;

  bool result = false ;
  
  // For the moment no pile-up for this type of event
  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    prepareInteraction( theEvents , theCollisions , theGenEvent, 
                        theGenCollision ) ;
    
    // First interaction is always "signal"
    
    if ( 0 == i ) {
      sc = m_productionTool -> generateEvent( theGenEvent , theGenCollision ) ;
      if ( sc.isFailure() ) Exception( "Could not generate event" ) ;
      
      ParticleVector theParticleList ;
      m_nEventsBeforeCut++ ;
      bool passCut = true ;
      if ( 0 != m_cutTool ) 
        passCut = m_cutTool -> applyCut( theParticleList , theGenEvent , 
                                         theGenCollision ) ;
      
      if ( passCut ) {
        m_nEventsAfterCut++ ;
        result = true ;
        theGenCollision -> setIsSignal( true ) ;
      } else return false ;
    } else {
      // if event passed generator level cut
      // look if there are still enough pile-up events
      if ( m_pileUpEventsVector.empty() ) generatePileUp() ;
      
      // retrieve now pile-up events
      HepMC::GenEvent * pileUpEvent = m_pileUpEventsVector.back() ;
      (*theGenEvent) =  ( * pileUpEvent ) ;

      m_pileUpEventsVector.pop_back() ;
      delete pileUpEvent ;

      // and collision
      LHCb::GenCollision * pileUpCollision = m_pileUpCollisionsVector.back() ;
      copyCollision( pileUpCollision , theGenCollision ) ;
      m_pileUpCollisionsVector.pop_back() ;
      delete pileUpCollision ;
    }
  } 
  return result ;
}

//=============================================================================
// Print the counters
//=============================================================================
void Special::printCounters( ) const {
  using namespace GenCounters ;
  printEfficiency( m_xmlLogTool , "generator level cut" , m_nEventsAfterCut , 
                   m_nEventsBeforeCut ) ;
}

//=============================================================================
// Generate PileUp Minimum Bias interactions
//=============================================================================
void Special::generatePileUp() {

  if ( 0 == m_pileUpProductionTool ) {
    if ( "" != m_pileUpProductionToolName ) {
      m_pileUpProductionTool = 
        tool< IProductionTool >( m_pileUpProductionToolName , this ) ;
      m_pileUpProductionTool -> initializeGenerator() ;
    }
  }

  if ( 0 == m_pileUpProductionTool ) {
    fatal() << "No pile up production tool defined !" << endreq ;    
    return ;  
  }

  // initialize the production tool for pile up generation
  if ( m_reinitialize ) 
    m_pileUpProductionTool -> initializeGenerator() ;
  
  // generate given number of events
  for ( unsigned int i = 0 ; i < m_maxInteractions ; ++i ) {
    HepMC::GenEvent * theEvent = new HepMC::GenEvent ;
    LHCb::GenCollision * theCollision = new LHCb::GenCollision ;
    m_pileUpProductionTool -> generateEvent( theEvent , 
                                             theCollision ) ;
    
    m_pileUpEventsVector.push_back( theEvent ) ;
    m_pileUpCollisionsVector.push_back( theCollision ) ;
  }

  // now initialize the production tool for the "signal"
  if ( m_reinitialize )
    m_productionTool -> initializeGenerator() ;
  //TODO : add finalize function to delete remaining events
}

//=============================================================================
// Copy collisions
//=============================================================================
void Special::copyCollision( const LHCb::GenCollision * FROM , 
                             LHCb::GenCollision * TO ) const {
  TO -> setIsSignal( FROM -> isSignal() ) ;
  TO -> setProcessType( FROM -> processType() ) ;
  TO -> setSHat( FROM -> sHat() ) ;
  TO -> setTHat( FROM -> tHat() ) ;
  TO -> setUHat( FROM -> uHat() ) ;
  TO -> setPtHat( FROM -> ptHat() ) ;
  TO -> setX1Bjorken( FROM -> x1Bjorken() ) ;
  TO -> setX2Bjorken( FROM -> x2Bjorken() ) ;
}
