// $Id: FixedNInteractions.cpp,v 1.5 2009-04-07 16:11:21 gcorti Exp $
// Include files 

// local
#include "FixedNInteractions.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"

// from Event 
#include "Event/GenHeader.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FixedNInteractions
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( FixedNInteractions )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FixedNInteractions::FixedNInteractions( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IPileUpTool >( this ) ;
    declareProperty ( "NInteractions" , m_nInteractions = 1 ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
FixedNInteractions::~FixedNInteractions( ) { }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode FixedNInteractions::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  if ( 1 == m_nInteractions ) 
    info() << "Single Interaction Mode" << endmsg ;
  else info() << "Fixed Number of Interactions per Event = : "
              << m_nInteractions << endmsg ;
  return sc ;
}

//=============================================================================
// Compute the number of pile up to generate according to beam parameters
//=============================================================================
unsigned int FixedNInteractions::numberOfPileUp( ) {
  return m_nInteractions ;
}

//=============================================================================
// Print the specific pile up counters
//=============================================================================
void FixedNInteractions::printPileUpCounters( ) { }

