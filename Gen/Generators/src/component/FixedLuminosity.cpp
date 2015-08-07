// $Id: FixedLuminosity.cpp,v 1.6 2009-04-07 16:11:21 gcorti Exp $
// Include files 

// local
#include "FixedLuminosity.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

// From Event
#include "Event/BeamParameters.h"

// From Generators
#include "Generators/GenCounters.h"
#include "Generators/ICounterLogFile.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FixedLuminosity
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( FixedLuminosity )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FixedLuminosity::FixedLuminosity( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ) ,
    m_xmlLogTool ( 0 ) ,
    m_numberOfZeroInteraction( 0 ) ,
    m_nEvents( 0 ) ,
    m_randSvc( 0 ) {
    declareInterface< IPileUpTool >( this ) ;
    declareProperty( "BeamParameters" , 
                     m_beamParameters = LHCb::BeamParametersLocation::Default ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
FixedLuminosity::~FixedLuminosity( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode FixedLuminosity::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  // Initialize the number generator
  m_randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;

  //  XMl log file
  m_xmlLogTool = tool< ICounterLogFile >( "XmlCounterLogFile" ) ;

  return sc ;
}

//=============================================================================
// Compute the number of pile up to generate according to beam parameters
//=============================================================================
unsigned int FixedLuminosity::numberOfPileUp( ) {
  LHCb::BeamParameters * beam = get< LHCb::BeamParameters >( m_beamParameters ) ;
  if ( 0 == beam ) Exception( "No beam parameters registered" ) ;

  unsigned int result = 0 ;
  while ( 0 == result ) {
    m_nEvents++ ;
    Rndm::Numbers poissonGenerator( m_randSvc , Rndm::Poisson( beam -> nu() ) ) ;
    result = (unsigned int) poissonGenerator() ;
    if ( 0 == result ) m_numberOfZeroInteraction++ ;
  }
  return result ;
}

//=============================================================================
// Print the specific pile up counters
//=============================================================================
void FixedLuminosity::printPileUpCounters( ) {
  using namespace GenCounters ;
  printCounter( m_xmlLogTool , "all events (including empty events)", m_nEvents ) ;
  printCounter( m_xmlLogTool , "events with 0 interaction" , 
                m_numberOfZeroInteraction ) ;
}

//=============================================================================
// Finalize method
//=============================================================================
StatusCode FixedLuminosity::finalize( ) {
  release( m_randSvc ) ;
  return GaudiTool::finalize( ) ;
}
