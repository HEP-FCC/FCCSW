// $Id: GenCounters.cpp,v 1.5 2007-09-11 17:48:32 robbep Exp $
// Include files

// local
#include "Generators/GenCounters.h"

// Generators
#include "Event/HepMCUtils.h"

// Boost
#include <boost/array.hpp>

// HepMC
#include "HepMC/GenEvent.h"

// Event
#include "Event/HepMCEvent.h"

// LHCb
#include "Event/ParticleID.h"

//=============================================================================
// Function to test if a HepMC::GenParticle is a B hadron at root of decay
//=============================================================================
struct isRootB : std::unary_function< const HepMC::GenParticle * , bool > {

  /// test operator, returns true if it is a root B
  bool operator() ( const HepMC::GenParticle * part ) const {

    // Do not consider documentation and special particles
    if ( part -> status() == LHCb::HepMCEvent::DocumentationParticle ) 
      return false ;
    
    // Check if particle has a b quark
    LHCb::ParticleID thePid( part -> pdg_id() ) ;
    if ( ! thePid.hasBottom() ) return false ;

    // Check if particle has a mother
    if ( 0 == part -> production_vertex() ) return true ;

    // Check all parents of the B 
    HepMC::GenVertex::particles_in_const_iterator parent ;
    const HepMC::GenVertex * thePV = part -> production_vertex() ;
    for ( parent = thePV -> particles_in_const_begin() ;
          parent != thePV -> particles_in_const_end() ; ++parent ) {
      LHCb::ParticleID parentID( (*parent) -> pdg_id() ) ;
      if ( parentID.hasBottom() && (thePid.abspid()==5 || parentID.abspid()!=5)) return false ;
    }

    // If no parent is a B, then it is a root B
    return true ;
  }
};

//=============================================================================
// Function to test if a HepMC::GenParticle is a D hadron at root of decay
//=============================================================================
struct isRootD : std::unary_function< const HepMC::GenParticle * , bool > {

  /// test operator, returns true if it is a root D
  bool operator() ( const HepMC::GenParticle * part ) const {

    // Do not consider documentation and special particles
    if ( part -> status() == LHCb::HepMCEvent::DocumentationParticle ) 
      return false ;

    // Check if particle has a c quark
    LHCb::ParticleID thePid( part -> pdg_id() ) ;
    if ( ! thePid.hasCharm() ) return false ;
    if ( thePid.hasBottom() ) return false ;

    // Check if particle has a mother
    if ( 0 == part -> production_vertex() ) return true ;

    // Check all parents of the D
    HepMC::GenVertex::particles_in_const_iterator parent ;
    const HepMC::GenVertex * thePV = part -> production_vertex() ;
    for ( parent = thePV -> particles_in_const_begin() ;
          parent != thePV -> particles_in_const_end() ; ++parent ) {
      LHCb::ParticleID parentID( (*parent) -> pdg_id() ) ;
      if ( parentID.hasCharm()  && (parentID.abspid()!=4 || thePid.abspid()==4)) return false ;
    }

    // If no parent is a D, then it is a root D
    return true ;
  }
};

//=============================================================================
// Function to test if a HepMC::GenParticle is a B hadron at end of decay tree
//=============================================================================
struct isEndB : std::unary_function< const HepMC::GenParticle * , bool > {

  /// Test operator. Returns true if particle is the last B
  bool operator() ( const HepMC::GenParticle * part ) const {

    // Do not look at special particles
    if ( part -> status() == LHCb::HepMCEvent::DocumentationParticle ) 
      return false ;

    // Test if particle has a b quark
    LHCb::ParticleID thePid( part -> pdg_id() ) ;
    if ( ! thePid.hasBottom() ) return false ;

    // test oscillation
    if ( ! HepMCUtils::IsBAtProduction( part ) ) return false ;

    // Test if the B has daughters (here we are sure it has not oscillated)
    if ( 0 == part -> end_vertex() ) return true ;
    
    // Loop over daughters to check if they are B hadrons
    HepMC::GenVertex::particles_out_const_iterator children ;
    const HepMC::GenVertex * theEV = part -> end_vertex() ;
    for ( children = theEV -> particles_out_const_begin() ;
          children != theEV -> particles_out_const_end() ; ++children ) {
      LHCb::ParticleID childID( (*children) -> pdg_id() ) ;
      if ( childID.hasBottom() ) {
        if ( (*children) -> pdg_id() == - part -> pdg_id() ) return true ;
        return false ;
      }
    }

    // If not, then it is a end B
    return true ;
  }
};

//=============================================================================
// Function to test if a HepMC::GenParticle is a D hadron at end of decay tree
//=============================================================================
struct isEndD : std::unary_function< const HepMC::GenParticle * , bool > {

  /// Test operator. Returns true if it is the last D
  bool operator() ( const HepMC::GenParticle * part ) const {

    // Do not look at special particles
    if ( part -> status() == LHCb::HepMCEvent::DocumentationParticle ) 
      return false ;

    // Check if it has a c quark
    LHCb::ParticleID thePid( part -> pdg_id() ) ;
    if ( ! thePid.hasCharm() ) return false ;

    // Check if it has daughters
    if ( 0 == part -> end_vertex() ) return true ;

    // Loop over the daughters to find a D hadron
    HepMC::GenVertex::particles_out_const_iterator children ;
    const HepMC::GenVertex * theEV = part -> end_vertex() ;
    for ( children = theEV -> particles_out_const_begin() ;
          children != theEV -> particles_out_const_end() ; ++children ) {
      LHCb::ParticleID childID( (*children) -> pdg_id() ) ;
      if ( childID.hasCharm() ) return false ;
    }

    // If not, then it is a End D
    return true ;
  }
};

//-----------------------------------------------------------------------------
// Implementation file for namespace : GenCounters
//
// 2006-02-06 : Patrick Robbe
//-----------------------------------------------------------------------------

//=============================================================================
// Setup names of B hadron counters
//=============================================================================
void GenCounters::setupBHadronCountersNames( BHadronCNames & BC , 
                                             BHadronCNames & antiBC ) {
    BC[ Bd ] = "B0" ;
    BC[ Bu ] = "B+" ;
    BC[ Bs ] = "Bs0" ;
    BC[ Bc ] = "Bc+" ;
    BC[ bBaryon ] = "b-Baryon" ;

    antiBC[ Bd ] = "anti-B0" ;
    antiBC[ Bu ] = "B-" ;
    antiBC[ Bs ] = "anti-Bs0" ;
    antiBC[ Bc ] = "Bc-" ;
    antiBC[ bBaryon ] = "anti-b-Baryon" ;
}

//=============================================================================
// Setup names of D hadron counters
//=============================================================================
void GenCounters::setupDHadronCountersNames( DHadronCNames & DC , 
                                             DHadronCNames & antiDC ) {
    DC[ D0 ] = "D0" ;
    DC[ Dch ] = "D+" ;
    DC[ Ds ] = "Ds+" ;
    DC[ cBaryon ] = "c-Baryon" ;

    antiDC[ D0 ] = "anti-D0" ;
    antiDC[ Dch ] = "D-" ;
    antiDC[ Ds ] = "Ds-" ;
    antiDC[ cBaryon ] = "anti-c-Baryon" ;
}

//=============================================================================
// Setup names of B excited states
//=============================================================================
void GenCounters::setupExcitedCountersNames( ExcitedCNames & B , 
                                             const std::string & root ) {
  B[ _0star ] = root + "(L=0,J=0)";
    B[ _1star ] = root + "* (L=0, J=1)" ;
    B[ _2star ] = root + "** (L=1, J=0,1,2)" ;
}

//=============================================================================
// Count excited states counters
//=============================================================================
void GenCounters::updateExcitedStatesCounters
( const HepMC::GenEvent * theEvent , ExcitedCounter & thebExcitedC ,
  ExcitedCounter & thecExcitedC ) {
  // Signal Vertex
  HepMC::GenVertex * signalV = theEvent -> signal_process_vertex() ;

  // Count B :
  std::vector< HepMC::GenParticle * > rootB ;
  HepMC::copy_if( theEvent -> particles_begin() , theEvent -> particles_end() ,
                  std::back_inserter( rootB ) , isRootB() ) ;

  std::vector< HepMC::GenParticle * >::const_iterator iter ;

  for ( iter = rootB.begin() ; iter != rootB.end() ; ++iter ) {
    if ( 0 != signalV ) {
      if ( ! HepMCUtils::commonTrees( signalV ,
                                      (*iter) -> end_vertex() ) ) 
        continue ;
    }
    LHCb::ParticleID thePid( (*iter) -> pdg_id() ) ;

    if ( thePid.isMeson() ) {
      if ( 0 == thePid.lSpin() ) {
        if ( 1 == thePid.jSpin() ) ++thebExcitedC[ _0star ] ;
        else ++thebExcitedC[ _1star ] ;
      } else ++thebExcitedC[ _2star ] ;
    }
  }

  // Count D :
  std::vector< HepMC::GenParticle * > rootD ;
  HepMC::copy_if( theEvent -> particles_begin() , theEvent -> particles_end() ,
                  std::back_inserter( rootD ) , isRootD() ) ;

  for ( iter = rootD.begin() ; iter != rootD.end() ; ++iter ) {
    if ( 0 != signalV ) {
      if ( ! HepMCUtils::commonTrees( signalV , 
                                      (*iter) -> end_vertex() ) ) 
        continue ;
    }
    
    LHCb::ParticleID thePid( (*iter) -> pdg_id() ) ;
    if ( thePid.isMeson() ) {
      if ( 0 == thePid.lSpin() ) {
        if ( 1 == thePid.jSpin() ) ++thecExcitedC[ _0star ] ;
        else ++thecExcitedC[ _1star ] ;
        } else ++thecExcitedC[ _2star ] ;
    }
  }       
}

//=============================================================================
// Update the counters of number of different hadrons in selected events
//=============================================================================
void GenCounters::updateHadronCounters( const HepMC::GenEvent * theEvent ,
                                        BHadronCounter & thebHadC , 
                                        BHadronCounter & theantibHadC ,
                                        DHadronCounter & thecHadC ,
                                        DHadronCounter & theanticHadC ,
                                        unsigned int & thebbCounter ,
                                        unsigned int & theccCounter ) {
  // Signal vertex
  HepMC::GenVertex * signalV = theEvent -> signal_process_vertex() ;

  // Count B:
  std::vector< HepMC::GenParticle * > endB ;
  HepMC::copy_if( theEvent -> particles_begin() , theEvent -> particles_end() ,
                  std::back_inserter( endB ) , isEndB() ) ;
  std::vector< HepMC::GenParticle * >::const_iterator iter ;
  
  for ( iter = endB.begin() ; iter != endB.end() ; ++iter ) {
    if ( 0 != signalV ) {
      if ( HepMCUtils::commonTrees( signalV , 
                                    (*iter) -> end_vertex() ) )
        continue ;
    }
    
    LHCb::ParticleID thePid( (*iter) -> pdg_id() ) ;
    
    if ( thePid.isMeson() ) {
      if ( thePid.pid() > 0 ) {
        if ( thePid.hasUp() ) ++thebHadC[ Bu ] ;
        else if ( thePid.hasDown() ) ++thebHadC[ Bd ] ;
        else if ( thePid.hasStrange() ) ++thebHadC[ Bs ] ;
        else if ( thePid.hasCharm() ) ++thebHadC[ Bc ] ;
        else ++thebbCounter ;
      } else {
        if ( thePid.hasUp() ) ++theantibHadC[ Bu ] ;
        else if ( thePid.hasDown() ) ++theantibHadC[ Bd ] ;
        else if ( thePid.hasStrange() ) ++theantibHadC[ Bs ] ;
        else if ( thePid.hasCharm() ) ++theantibHadC[ Bc ] ;
        else ++thebbCounter ;
      }
    } else if ( thePid.isBaryon() ) {
      if ( thePid.pid() < 0 ) ++thebHadC[ bBaryon ] ;
      else ++theantibHadC[ bBaryon ] ;
    }
  }
  
  std::vector< HepMC::GenParticle * > endD ;
  HepMC::copy_if( theEvent -> particles_begin() , theEvent -> particles_end() ,
                  std::back_inserter( endD ) , isEndD() ) ;
  
  for ( iter = endD.begin() ; iter != endD.end() ; ++iter ) {
    if ( 0 != signalV ) {
      if ( HepMCUtils::commonTrees( signalV ,
                                    (*iter) -> end_vertex() ) ) 
        continue ;
    }
    
    LHCb::ParticleID thePid( (*iter) -> pdg_id() ) ;
    
    if ( thePid.isMeson() ) {
      if ( thePid.pid() > 0 ) {
        if ( thePid.hasUp() ) ++thecHadC[ D0 ] ;
        else if ( thePid.hasDown() ) ++thecHadC[ Dch ] ;
        else if ( thePid.hasStrange() ) ++thecHadC[ Ds ] ;
        else ++theccCounter ;
      } else {
        if ( thePid.hasUp() ) ++theanticHadC[ D0 ] ;
        else if ( thePid.hasDown() ) ++theanticHadC[ Dch ] ;
        else if ( thePid.hasStrange() ) ++theanticHadC[ Ds ] ;
        else ++theccCounter ;
      }
    } else if ( thePid.isBaryon() ) {
      if ( thePid.pid() > 0 ) ++thecHadC[ cBaryon ] ;
      else ++theanticHadC[ cBaryon ] ;
    }
  } 
}

