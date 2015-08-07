// $Id: MergedEventsFilter.cpp,v 1.1 2008-05-06 08:27:55 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from LHCb
#include "Event/GenHeader.h"
#include "Event/GenCollision.h"

// from Kernel
#include "MCInterfaces/IFullGenEventCutTool.h"

// local
#include "MergedEventsFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MergedEventsFilter
//
// 2008-04-30 : Gloria CORTI
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MergedEventsFilter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MergedEventsFilter::MergedEventsFilter( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_fullGenEventCutTool  ( 0 )
{
  
    // Tool name to cut on full event
    declareProperty( "FullGenEventCutTool" , 
                     m_fullGenEventCutToolName = "" ) ;
    // Location of the input from the generation
    declareProperty ( "HepMCEventLocation" , m_hepMCEventLocation = 
                      LHCb::HepMCEventLocation::Default ) ;
    declareProperty ( "GenCollisionLocation" , m_genCollisionLocation = 
                      LHCb::GenCollisionLocation::Default ) ;
    
}
//=============================================================================
// Destructor
//=============================================================================
MergedEventsFilter::~MergedEventsFilter() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MergedEventsFilter::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // Retrieve full gen event cut tool
  if ( "" != m_fullGenEventCutToolName ) {
    m_fullGenEventCutTool =
      tool< IFullGenEventCutTool >( m_fullGenEventCutToolName , this ) ;
  }
  else {
    info() << "Filter does not have a cut::: MUST be specified!!!" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MergedEventsFilter::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // Retrieve info from the TES
  LHCb::HepMCEvents* theEvents = 
    get<LHCb::HepMCEvents>( m_hepMCEventLocation );

  LHCb::GenCollisions* theCollisions = 
    get<LHCb::GenCollisions>( m_genCollisionLocation );
  

  // Apply generator level cut on full event
  bool goodEvent = true;
  
  //  ++m_nBeforeFullEvent ;
  goodEvent = m_fullGenEventCutTool->studyFullEvent( theEvents, theCollisions );
  // put counters
  setFilterPassed(goodEvent);
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MergedEventsFilter::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
