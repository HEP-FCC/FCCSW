// $Id: BeamSpotSmearVertex.cpp,v 1.12 2010-05-09 17:05:42 gcorti Exp $
// Include files 

// local
#include "BeamSpotSmearVertex.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/PhysicalConstants.h" 

// from Event
#include "Event/HepMCEvent.h"
#include "Event/BeamParameters.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LHCbAcceptance
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( BeamSpotSmearVertex )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BeamSpotSmearVertex::BeamSpotSmearVertex( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IVertexSmearingTool >( this ) ;
    declareProperty( "Xcut" , m_xcut = 4. ) ; // times SigmaX 
    declareProperty( "Ycut" , m_ycut = 4. ) ; // times SigmaY
    declareProperty( "Zcut" , m_zcut = 4. ) ; // times SigmaZ
    declareProperty( "SignOfTimeVsT0", m_timeSignVsT0 = 0 ) ;
    declareProperty( "BeamParameters" , 
                     m_beamParameters = LHCb::BeamParametersLocation::Default ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
BeamSpotSmearVertex::~BeamSpotSmearVertex( ) { ; }

//=============================================================================
// Initialize 
//=============================================================================
StatusCode BeamSpotSmearVertex::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;
  
  IRndmGenSvc * randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;
  sc = m_gaussDist.initialize( randSvc , Rndm::Gauss( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Could not initialize gaussian random number generator" ) ;

    
  info() << "Smearing of interaction point with Gaussian distribution "
         << endmsg;

  release( randSvc ) ;
 
  return sc ;
}

//=============================================================================
// Smearing function
//=============================================================================
StatusCode BeamSpotSmearVertex::smearVertex( LHCb::HepMCEvent * theEvent ) {

  LHCb::BeamParameters * beamp = get< LHCb::BeamParameters >( m_beamParameters ) ;
  if ( 0 == beamp ) Exception( "No beam parameters registered" ) ;

  double dx , dy , dz;
  
  do { dx = m_gaussDist( ) ; } while ( fabs( dx ) > m_xcut ) ;
  dx = dx * beamp -> sigmaX() + beamp -> beamSpot().x() ;
  do { dy = m_gaussDist( ) ; } while ( fabs( dy ) > m_ycut ) ;
  dy = dy * beamp -> sigmaY() + beamp -> beamSpot().y() ;
  do { dz = m_gaussDist( ) ; } while ( fabs( dz ) > m_zcut ) ;
  dz = dz * beamp -> sigmaZ() + beamp -> beamSpot().z() ;

  double meanT = m_timeSignVsT0 * beamp -> beamSpot().z() / Gaudi::Units::c_light ;

  HepMC::FourVector dpos( dx , dy , dz , meanT ) ;
  
  HepMC::GenEvent::vertex_iterator vit ;
  HepMC::GenEvent * pEvt = theEvent -> pGenEvt() ;
  for ( vit = pEvt -> vertices_begin() ; vit != pEvt -> vertices_end() ; 
        ++vit ) {
     HepMC::FourVector pos = (*vit) -> position() ;
    (*vit) -> set_position( HepMC::FourVector( pos.x() + dpos.x() , 
                                               pos.y() + dpos.y() , 
                                               pos.z() + dpos.z() , 
                                               pos.t() + dpos.t() ) ) ;
  }

  return StatusCode::SUCCESS ;      
}

