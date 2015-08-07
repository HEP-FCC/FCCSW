// $Id: LeptonInAcceptance.cpp,v 1.5 2008-07-09 14:35:36 robbep Exp $
// Include files 
// local
#include "LeptonInAcceptance.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"

// from Event
#include "Event/HepMCEvent.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LeptonInAcceptance
//
// 2005-11-21 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY(LeptonInAcceptance )

// Function to test if particle is a lepton
bool isLepton( const HepMC::GenParticle * p ) {
  return ( ( 13 == abs( p -> pdg_id() ) ) || ( 11 == abs( p -> pdg_id() ) ) ) ;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LeptonInAcceptance::LeptonInAcceptance( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IFullGenEventCutTool >( this ) ;
    declareProperty( "ThetaMax" , m_thetaMax = 400 * Gaudi::Units::mrad ) ;
    declareProperty( "PtMin" , m_ptMin = 4. * Gaudi::Units::GeV ) ;
}
//=============================================================================
// Destructor
//=============================================================================
LeptonInAcceptance::~LeptonInAcceptance( ) { ; }
//=============================================================================
// Selection function
//=============================================================================
bool LeptonInAcceptance::studyFullEvent( LHCb::HepMCEvents * theEvents ,
                                         LHCb::GenCollisions * /* col */ )
  const {
  std::list< HepMC::GenParticle * > leptonList ;

  LHCb::HepMCEvents::iterator iter ;
  for ( iter = theEvents -> begin() ; theEvents -> end() != iter ; ++iter ) {
    HepMC::GenEvent * theHepMCEvent = (*iter) -> pGenEvt() ;
    HepMC::copy_if( theHepMCEvent -> particles_begin() ,
                    theHepMCEvent -> particles_end() , 
                    back_inserter( leptonList )  , isLepton ) ;
  }

  if ( leptonList.empty() ) {
    debug() << "No lepton in this event" << endmsg ;
    return false ;
  }
  
  std::list< HepMC::GenParticle * >::const_iterator it ;
  double px , py , pz , pp , theta , pt ;
  for ( it = leptonList.begin() ; leptonList.end() != it ; ++it ) {
    px = (*it) -> momentum() . px() ;
    py = (*it) -> momentum() . py() ;
    pz = (*it) -> momentum() . pz() ;
    pp = sqrt( px*px + py*py + pz*pz ) ;
    pt = sqrt( px*px + py*py ) ;
    theta = acos( fabs( pz ) / pp ) ;
    if ( ( theta <= m_thetaMax ) && ( pz >= 0. ) && ( pt >= m_ptMin ) ) {
      debug() << 
        format( "Event passed [theta = %.3g] [pz = %.3g] [pt = %.3g]" , 
                theta / Gaudi::Units::mrad , pz / Gaudi::Units::GeV , 
                pt / Gaudi::Units::GeV ) << endmsg ;
      return true ;
    } else 
      debug() << 
        format( "Particle failed cut [theta = %.3g] [pz = %.3g] [pt = %.3g]" ,
                theta / Gaudi::Units::mrad , pz / Gaudi::Units::GeV , 
                pt / Gaudi::Units::GeV ) << endmsg ;
  }
  return false ;
}
