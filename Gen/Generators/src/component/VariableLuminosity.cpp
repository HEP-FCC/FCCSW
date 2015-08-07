// $Id: VariableLuminosity.cpp,v 1.6 2009-04-07 16:11:21 gcorti Exp $
// Include files 

// local
#include "VariableLuminosity.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"

// From CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// From Event
#include "Event/BeamParameters.h"

// From Generators
#include "Generators/GenCounters.h"
#include "Generators/ICounterLogFile.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VariableLuminosity
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( VariableLuminosity )

using namespace CLHEP;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VariableLuminosity::VariableLuminosity( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent ) ,
    m_xmlLogTool( 0 ) ,
    m_numberOfZeroInteraction( 0 ) ,
    m_nEvents( 0 ),
    m_randSvc( 0 ) {
    declareInterface< IPileUpTool >( this ) ;
    declareProperty( "BeamParameters" , 
                     m_beamParameters = LHCb::BeamParametersLocation::Default ) ;
    declareProperty ( "FillDuration"  , m_fillDuration  = 7.0 * 3600 * s    ) ;
    declareProperty ( "BeamDecayTime" , m_beamDecayTime = 10.0 * 3600 * s   ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
VariableLuminosity::~VariableLuminosity( ) { ; }

//=============================================================================
// Initialize method
//=============================================================================
StatusCode VariableLuminosity::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  // Initialize the number generator
  m_randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;

  // XML file
  m_xmlLogTool = tool< ICounterLogFile >( "XmlCounterLogFile" ) ;
  
  sc = m_flatGenerator.initialize( m_randSvc , Rndm::Flat( 0 , 1 ) ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Could not initialize flat random generator" ) ;

  info() << "Poisson distribution with 'LHCb mean'. " << endmsg ;
  info() << "Fill duration (hours): " << m_fillDuration / 3600 / s << endmsg ;
  info() << "Beam decay time (hours): " << m_beamDecayTime / 3600 / s 
         << endmsg ;

  return sc ;
}

//=============================================================================
// Compute the number of pile up to generate according to beam parameters
//=============================================================================
unsigned int VariableLuminosity::numberOfPileUp( ) {
  LHCb::BeamParameters * beam = get< LHCb::BeamParameters >( m_beamParameters ) ;
  if ( 0 == beam ) Exception( "No beam parameters registered" ) ;

  unsigned int result = 0 ;
  double mean , currentLuminosity;
  while ( 0 == result ) {
    m_nEvents++ ;
    currentLuminosity = beam -> luminosity() * m_fillDuration / m_beamDecayTime / 
      ( 1.0 - exp( -m_fillDuration / m_beamDecayTime ) ) ;

    mean = currentLuminosity * beam -> totalXSec() / beam -> revolutionFrequency() ;
    Rndm::Numbers poissonGenerator( m_randSvc , Rndm::Poisson( mean ) ) ;
    result = (unsigned int) poissonGenerator() ;
    if ( 0 == result ) m_numberOfZeroInteraction++ ;
  }

  return result ;
}

//=============================================================================
// Print the specific pile up counters
//=============================================================================
void VariableLuminosity::printPileUpCounters( ) {
  using namespace GenCounters ;
  printCounter( m_xmlLogTool , "all events (including empty events)" , m_nEvents ) ;
  printCounter( m_xmlLogTool , "events with 0 interaction" , 
                m_numberOfZeroInteraction ) ;
}

//=============================================================================
// Finalize method
//=============================================================================
StatusCode VariableLuminosity::finalize( ) {
  release( m_randSvc ) ;
  return GaudiTool::finalize( ) ;
}
