// $Id: MinimumBias.cpp,v 1.4 2007-01-12 15:17:38 ranjard Exp $
// Include files 

// local
#include "MinimumBias.h"
// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

#include "Generators/IProductionTool.h"

// Event 
#include "Event/HepMCEvent.h"
#include "Event/GenCollision.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MinimumBias
//
// 2005-08-18 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( MinimumBias )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MinimumBias::MinimumBias( const std::string & type , const std::string & name,
                          const IInterface * parent )
  : ExternalGenerator( type, name , parent ) { }

//=============================================================================
// Destructor
//=============================================================================
MinimumBias::~MinimumBias( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode MinimumBias::initialize( ) {
  info() << "Generating Minimum Bias events." << endmsg ;
  return ExternalGenerator::initialize( ) ;
}

//=============================================================================
// Generate Set of Event for Minimum Bias event type
//=============================================================================
bool MinimumBias::generate( const unsigned int nPileUp , 
                            LHCb::HepMCEvents * theEvents , 
                            LHCb::GenCollisions * theCollisions ) {
  StatusCode sc ;
  LHCb::GenCollision * theGenCollision( 0 ) ;
  HepMC::GenEvent * theGenEvent( 0 ) ;
  
  for ( unsigned int i = 0 ; i < nPileUp ; ++i ) {
    prepareInteraction( theEvents , theCollisions , theGenEvent , 
                        theGenCollision ) ;
    
    sc = m_productionTool -> generateEvent( theGenEvent , theGenCollision ) ;
    if ( sc.isFailure() ) Exception( "Could not generate event" ) ;
  } 
  return true ;
}
