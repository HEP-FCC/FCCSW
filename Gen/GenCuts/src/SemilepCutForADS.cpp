// local
#include "SemilepCutForADS.h"
#include <iostream>

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"

// from Kernel
#include "Event/ParticleID.h"
#include "GaudiKernel/Vector4DTypes.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// from Generators
#include "Event/HepMCUtils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SemilepCutForADS
//
// 2011-02-18 : Paolo Gandini
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( SemilepCutForADS )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SemilepCutForADS::SemilepCutForADS( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IGenCutTool >( this ) ;
    declareProperty( "ChargedThetaMin" , m_chargedThetaMin = 10 * Gaudi::Units::mrad ) ;
    declareProperty( "ChargedThetaMax" , m_chargedThetaMax = 400 * Gaudi::Units::mrad ) ;
    declareProperty( "NeutralThetaMin" , m_neutralThetaMin = 5 * Gaudi::Units::mrad ) ;
    declareProperty( "NeutralThetaMax" , m_neutralThetaMax = 400 * Gaudi::Units::mrad ) ;
    declareProperty( "MassMin" , m_massMIN = 4600*Gaudi::Units::MeV ) ;

}

//=============================================================================
// Destructor 
//=============================================================================
SemilepCutForADS::~SemilepCutForADS( ) { ; }

//=============================================================================
// Cut Function
//=============================================================================
bool SemilepCutForADS::applyCut( ParticleVector & theParticleVector ,
                                const HepMC::GenEvent * /* theEvent */ ,
                                const LHCb::GenCollision * /* theHardInfo */ )
  const {
  ParticleVector::iterator it ;
  double D0_px(0.) , D0_py(0.) , D0_pz(0.) , D0_pE(0.); 
  int findD0=0;
  double mu_px(0.) , mu_py(0.) , mu_pz(0.) , mu_pE(0.); 
  int findmu=0;
  int findnu=0;
  for ( it = theParticleVector.begin() ; it != theParticleVector.end() ; ++it ) {
    
    debug() << "-----------------------------------"<< std::endl<< endmsg ;
    debug() << "Parent: " << (*it) -> pdg_id() << endmsg ;

    HepMC::GenVertex * EV = (*it) -> end_vertex() ; if ( 0 == EV ) return false;
    if ( ! passCuts( *it ) ) return false;
    
    HepMC::GenVertex::particle_iterator iterDAU ;
    for ( iterDAU = EV -> particles_begin( HepMC::children ) ; iterDAU != EV -> particles_end( HepMC::children ) ; ++iterDAU ) {
      debug() << "   "<< (*iterDAU) -> pdg_id()<< endmsg ;
      //////////////////////
      if ( 421 == abs((*iterDAU) -> pdg_id()) ) {
        D0_px = (*iterDAU) -> momentum().px() ;
        D0_py = (*iterDAU) -> momentum().py() ;
        D0_pz = (*iterDAU) -> momentum().pz() ;
        D0_pE = (*iterDAU) -> momentum().e() ;
        findD0=1;
      }
      if ( 13  == abs((*iterDAU) -> pdg_id()) ) {
        mu_px = (*iterDAU) -> momentum().px() ;
        mu_py = (*iterDAU) -> momentum().py() ;
        mu_pz = (*iterDAU) -> momentum().pz() ;
        mu_pE = (*iterDAU) -> momentum().e() ;
        findmu=1;
      }
      if ( 14  == abs((*iterDAU) -> pdg_id()) ) {
        findnu=1;
      }
      //////////////////////      
    } //end daughters of the B
    
  } //end theParticleVector iterator
  debug() << "-----------------------------------"<< std::endl<< endmsg ;
  double invMASS =  sqrt((D0_pE+mu_pE)*(D0_pE+mu_pE)-(D0_px+mu_px)*(D0_px+mu_px)-(D0_py+mu_py)*(D0_py+mu_py)-(D0_pz+mu_pz)*(D0_pz+mu_pz));
  debug() << " Invariant Mass: "<<invMASS<< std::endl<< endmsg ;
  debug() << "m_massMIN        "<<m_massMIN<< std::endl<< endmsg ;
  debug() << "-----------------------------------"<< std::endl<< endmsg ;

  if(findD0==1&&findmu==1&&findnu==1&& invMASS>m_massMIN) {    return true ;}
  else{    return false;}


}




//=============================================================================
// Functions to test if all daughters are in acceptance
//=============================================================================
bool SemilepCutForADS::passCuts( const HepMC::GenParticle * theSignal ) const {
  HepMC::GenVertex * EV = theSignal -> end_vertex() ;
  if ( 0 == EV ) return true ;

  typedef std::vector< HepMC::GenParticle * > Particles ;
  Particles stables ;
  HepMC::GenVertex::particle_iterator iter ;

  for ( iter = EV -> particles_begin( HepMC::descendants ) ; 
        iter != EV -> particles_end( HepMC::descendants ) ; ++iter ) {
    if ( 0 == (*iter) -> end_vertex() ) stables.push_back( *iter ) ;
  }  

  if ( stables.empty() )
    Exception( "Signal has no stable daughters !" ) ;

  double angle( 0. ) ;
  double firstpz = stables.front() -> momentum().pz() ;

  debug() << "New event" << endmsg ;

  for ( Particles::const_iterator it = stables.begin() ; it != stables.end() ;
        ++it ) {

    debug() << "Check particle " << (*it) -> pdg_id() << " with angle " 
            << (*it) -> momentum().theta() / Gaudi::Units::mrad 
            << " mrad." << endmsg ;
   
    // Remove neutrinos
    if ( ( 12 == abs( (*it) -> pdg_id() ) ) || 
         ( 14 == abs( (*it) -> pdg_id() ) ) || 
         ( 16 == abs( (*it) -> pdg_id() ) ) ) continue ;
 
    // Don't use daughters of Lambda and KS:
    HepMC::GenParticle * theParent ;
    theParent = 
      *( (*it) -> production_vertex() -> particles_in_const_begin() ) ;
    if ( 3122 == abs( theParent -> pdg_id() ) ) continue ;
    if ( 310 == theParent -> pdg_id() ) continue ;

    // Consider only gammas from pi0 and eta
    if ( 22 == (*it) -> pdg_id() ) {
      if ( ( 111 != theParent -> pdg_id() ) &&
           ( 221 != theParent -> pdg_id() ) ) continue ;
    }

    // All particles in same direction
    if ( 0 > ( firstpz * ( (*it) -> momentum().pz() ) ) ) return false ;
      
    angle = (*it) -> momentum().theta() ;
    
    LHCb::ParticleID pid( (*it) -> pdg_id() ) ;
    if ( 0 == pid.threeCharge() ) {
      if ( fabs( sin( angle ) ) > fabs( sin( m_neutralThetaMax ) ) ) 
        return false ;
      if ( fabs( sin( angle ) ) < fabs( sin( m_neutralThetaMin ) ) ) 
        return false ;
    } else {
      if ( fabs( sin( angle ) ) > fabs( sin( m_chargedThetaMax ) ) ) 
        return false ;
      if ( fabs( sin( angle ) ) < fabs( sin( m_chargedThetaMin ) ) ) 
        return false ;
    }
  }

  debug() << "Event passed !" << endmsg ;
  
  return true ;
}
