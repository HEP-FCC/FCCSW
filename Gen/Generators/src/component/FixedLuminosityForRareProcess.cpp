// $Id: FixedLuminosityForRareProcess.cpp,v 1.3 2009-04-07 16:11:21 gcorti Exp $
// Include files 

// local
#include "FixedLuminosityForRareProcess.h"

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
// Implementation file for class : FixedLuminosityForRareProcess
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( FixedLuminosityForRareProcess )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FixedLuminosityForRareProcess::FixedLuminosityForRareProcess( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ) ,
    m_xmlLogTool( 0 ) ,
    m_nEvents( 0 ) ,
    m_randSvc( 0 ) {
    declareInterface< IPileUpTool >( this ) ;
    declareProperty( "BeamParameters" , 
                     m_beamParameters = LHCb::BeamParametersLocation::Default ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
FixedLuminosityForRareProcess::~FixedLuminosityForRareProcess( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode FixedLuminosityForRareProcess::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  // Initialize the number generator
  m_randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;

  // XML file for generator statistics
  m_xmlLogTool = tool< ICounterLogFile >( "XmlCounterLogFile" ) ;

  info() << "Poisson distribution with fixed luminosity. " << endmsg ;

  return sc ;
}

//=============================================================================
// Compute the number of pile up to generate according to beam parameters
//=============================================================================
unsigned int FixedLuminosityForRareProcess::numberOfPileUp( ) {
  LHCb::BeamParameters * beam = get< LHCb::BeamParameters >( m_beamParameters ) ;
  if ( 0 == beam ) Exception( "No beam parameters registered" ) ;  

  unsigned int result = 0 ;
  m_nEvents++ ;
  Rndm::Numbers poissonGenerator( m_randSvc , Rndm::Poisson( beam -> nu() ) ) ;
  result = (unsigned int) ( poissonGenerator() + 1.0 ) ;
  return result ;
}

//=============================================================================
// Print the specific pile up counters
//=============================================================================
void FixedLuminosityForRareProcess::printPileUpCounters( ) {
  using namespace GenCounters ;
  printCounter( m_xmlLogTool , "all events (including empty events)", m_nEvents ) ;
}

//=============================================================================
// Finalize method
//=============================================================================
StatusCode FixedLuminosityForRareProcess::finalize( ) {
  release( m_randSvc ) ;
  return GaudiTool::finalize( ) ;
}
