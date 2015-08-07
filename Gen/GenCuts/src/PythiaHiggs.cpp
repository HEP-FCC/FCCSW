// $Id: PythiaHiggs.cpp,v 1.6 2008-07-09 14:44:51 robbep Exp $
// Include files 
// local
#include "PythiaHiggs.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenEvent.h"

// Selection function for Higgs
bool isHiggs( const HepMC::GenParticle * p ) {
  return ( ( 25 == p -> pdg_id() ) && ( 0 == p -> end_vertex() ) ) ;
}

//-----------------------------------------------------------------------------
// Implementation file for class : PythiaHiggs
//
// 2005-11-21 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( PythiaHiggs )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PythiaHiggs::PythiaHiggs( const std::string & type , const std::string & name ,
                          const IInterface * parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IGenCutTool >( this ) ;
    declareProperty( "ThetaMax" , m_thetaMax = 400 * Gaudi::Units::mrad ) ;
}
//=============================================================================
// Destructor
//=============================================================================
PythiaHiggs::~PythiaHiggs( ) { ; }
//=============================================================================
// Accept function
//=============================================================================
bool PythiaHiggs::applyCut( ParticleVector & /* theParticleVector */ ,
                            const HepMC::GenEvent * theEvent ,
                            const LHCb::GenCollision * /* theCollision */ ) 
  const {
  std::list< HepMC::GenParticle * > higgsList ;
  HepMC::copy_if( theEvent -> particles_begin() , theEvent -> particles_end() ,
                  back_inserter( higgsList ) , isHiggs ) ;
  
  if ( higgsList.empty() ) Exception( "No Higgs particle in this event !" ) ;
  
  HepMC::GenParticle * theHiggs = higgsList.front() ;
  HepMC::GenParticle * theHiggsMother( 0 ) ;
  HepMC::GenVertex * theHiggsPV = theHiggs -> production_vertex() ;
  HepMC::GenVertex::particle_iterator iter ;
  
  for ( iter  = theHiggsPV -> particles_begin( HepMC::parents ) ;
        iter != theHiggsPV -> particles_end( HepMC::parents ) ; ++iter ) {
    if ( (*iter) -> pdg_id() == 25 ) {
      theHiggsMother = (*iter) ;
      break ;
    }
  }
  
  if ( 0 == theHiggsMother ) Exception( "No Higgs mother in this event !" ) ;
  
  // Get the 2 b quarks of the hard interaction
  HepMC::GenParticle * thebHard( 0 ) , * thebbarHard( 0 ) ;
  HepMC::GenVertex * theHiggsEV = theHiggsMother -> end_vertex() ;
  for ( iter = theHiggsEV -> particles_begin( HepMC::children ) ;
        iter != theHiggsEV -> particles_end( HepMC::children ) ; ++iter ) {
    if ( (*iter) -> pdg_id() == 5 ) thebHard = (*iter) ;
    else if ( (*iter) -> pdg_id() == -5 ) thebbarHard = (*iter) ;
  }
  
  if ( ( 0 == thebHard ) || ( 0 == thebbarHard ) ) 
    Exception( "No Hard b in this event !" ) ;
  
  // Get the 2 real b quarks
  HepMC::GenParticle * thebFrag( 0 ) , * thebbarFrag( 0 ) ;
  HepMC::GenVertex * thebEV = thebHard -> end_vertex() ;
  HepMC::GenVertex * thebbarEV = thebbarHard -> end_vertex() ;
  for ( iter = thebEV -> particles_begin( HepMC::children ) ;
        iter != thebEV -> particles_end( HepMC::children ) ; ++iter ) {
    if ( (*iter) -> pdg_id() == 5 ) {
      thebFrag = (*iter) ;
      break ;
    }
  }
  
  for ( iter = thebbarEV -> particles_begin( HepMC::children ) ;
        iter != thebbarEV -> particles_end( HepMC::children ) ; ++iter ) {
    if ( (*iter) -> pdg_id() == -5 ) {
      thebbarFrag = (*iter) ;
      break ;
    }
  }
  
  if ( ( 0 == thebbarFrag ) || ( 0 == thebFrag ) ) 
    Exception( "No real b quark in this event !") ;
    
  double pxb = thebFrag -> momentum() . px() * Gaudi::Units::GeV ;
  double pyb = thebFrag -> momentum() . py() * Gaudi::Units::GeV ;
  double pzb = thebFrag -> momentum() . pz() * Gaudi::Units::GeV ;
  double ppb = sqrt( pxb*pxb + pyb*pyb + pzb*pzb ) ;
  double thetab = acos( fabs( pzb ) / ppb ) ;

  double pxbbar = thebbarFrag -> momentum() . px() * Gaudi::Units::GeV ;
  double pybbar = thebbarFrag -> momentum() . py() * Gaudi::Units::GeV ;
  double pzbbar = thebbarFrag -> momentum() . pz() * Gaudi::Units::GeV ;
  double ppbbar = sqrt( pxbbar*pxbbar + pybbar*pybbar + pzbbar*pzbbar ) ;
  double thetabbar = acos( fabs( pzbbar ) / ppbbar ) ;
  
  if ( ( thetab <= m_thetaMax ) && ( pzb >= 0. ) && 
       ( thetabbar <= m_thetaMax ) && ( pzbbar >= 0. ) ) {
    debug() << format( "Event passed [thetab = %.5g] [thetabbar = %.5g]" ,
                       thetab / Gaudi::Units::mrad , 
                       thetabbar / Gaudi::Units::mrad ) 
            << format( " [pzb = %.5g] [pzbbar = %.5g]" , 
                       pzb / Gaudi::Units::GeV , 
                       pzbbar / Gaudi::Units::GeV ) 
            << endmsg ;
    return true ;
  } else {
    debug() << format( "Event rejected [thetab = %.5g] [thetabbar = %.5g]" ,
                       thetab / Gaudi::Units::mrad , 
                       thetabbar / Gaudi::Units::mrad ) 
            << format( " [pzb = %.5g] [pzbbar = %.5g]" , 
                       pzb / Gaudi::Units::GeV , 
                       pzbbar / Gaudi::Units::GeV ) 
            << endmsg ;
    return false ;
  }
  return false ;
}
