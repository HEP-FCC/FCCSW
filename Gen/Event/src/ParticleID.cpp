// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <iostream>
#include <sstream>
// ============================================================================
// Local 
// ============================================================================
#include "Event/ParticleID.h"
// ============================================================================
/** @file 
 *  Implementation file for class : ParticleID
 *  @date 19/02/2002 
 *  @author Gloria Corti
 */
// ============================================================================
// Is a valid number according to LHCb scheme
// ============================================================================
bool LHCb::ParticleID::isValid( ) const 
{
  //
  if ( 0 < extraBits() ) 
  {
    if( isNucleus() ) { return true; }
    // Reserved special particles: geantino and intermediate
    return (480000000 == pid()) || (-990000000 == pid());
  }
  // Meson, Baryon or DiQuark signature
  if ( isMeson() || isBaryon() || isDiQuark() ) { return true; }
  // Fundamental particle (quark or lepton)
  if ( 0 < fundamentalID() ) { return true ; }
  //
  return false;
}
// ============================================================================
// Mesons
// ============================================================================
bool LHCb::ParticleID::isMeson( ) const 
{
  if ( hasQuarks() ) 
  {
    const unsigned int aid = abspid();
    if ( 130 == aid || 310 == aid  ) { return true; } // K0S/K0L ? 
    if ( 150 == aid || 510 == aid  ) { return true; } // B0L/B0H ? 
    //
    if ( 0 != digit_<nj>  () && 
         0 != digit_<nq3> () && 
         0 != digit_<nq2> () && 
         0 == digit_<nq1> ()  )
    {
      // check for illegal antiparticles
      return ( 0 > pid() && digit_<nq3>() == digit_<nq2>() ) ? false : true ;
    }
  }
  return false;
}
// ============================================================================
// Baryon
// ============================================================================
bool LHCb::ParticleID::isBaryon( ) const 
{
  return hasQuarks()    && 
    0 != digit_<nj>  () &&
    0 != digit_<nq3> () &&
    0 != digit_<nq2> () &&
    0 != digit_<nq1> ()  ; 
}
// ============================================================================
// DiQuark
// ============================================================================
bool LHCb::ParticleID::isDiQuark( ) const 
{
  if( hasQuarks() ) 
  {
    // diquark signature
    if ( 0 != digit_<nj>  () && 
         0 == digit_<nq3> () &&
         0 != digit_<nq2> () && 
         0 != digit_<nq1> ()  )
    {
      // illegal ? 
      return ( 1 == digit_<nj>() && 
               digit_<nq2>() == digit_<nq1>() ) ? false : true ;
    }
    return false;
  }
  //
  return false;
}
// ============================================================================
// Leptons
// ============================================================================
bool LHCb::ParticleID::isLepton( ) const 
{
  if ( 0 < extraBits() ) { return false; }
  const int fun = fundamentalID() ;  
  return  11 <= fun && fun <= 18 ;
}
// ============================================================================
// Nucleus
// ============================================================================
bool LHCb::ParticleID::isNucleus() const 
{
  const int extra = extraBits () ;
  if ( 0 >= extra ) { return false ; }
  
  // Nuclei with PDG2008 convention (2006 Monte Carlo nuclear code scheme)
  // Ion numbers are +/- 10LZZZAAAI. 
  // AAA is A - baryon/nucleon number
  // ZZZ is Z - total charge/atomic number
  // L is the total number of strange quarks.
  // I is the isomer number, with I=0 corresponding to the ground state.

  // a proton can also be a Hydrogen nucleus
  if( abspid() == 2212 ) { return true; }

  if( ( 1 == digit_<n10>() ) && ( 0 == digit_<n9>() ) ) {
    // charge should always be less than or equal to baryon number
    if( A() >= Z() ) { return true; }
  }

  // geantino
  if ( 48 == extra ) { return false; }
    
  // Old convention
  if ( 45 <= extra && extra <= 49 ) { return true; }
  
  // all else is not a nucleus
  return false;
  
}

// ============================================================================
// Atomic number if this is a nucleus
// ============================================================================
int LHCb::ParticleID::Z( ) const
{
  // a proton can also be an Hydrogen nucleus
  if ( abspid() == 2212 ) { return 1; }

  const int extra = extraBits ();
  if ( ( digit_<n10>() != 1 ) || ( digit_<n9>() != 0 ) ) { 
    // Special LHcb particle and old conventions
    if ( 45 == extra || 46 == extra ) { return 1; } // H2 or H3
    if ( 47 == extra || 49 == extra ) { return 2; } // alph aor He3
    return 0; 
  }
  return (abspid()/10000)%1000;
}

// ============================================================================
// Nucleon number if this is a nucleus
// ============================================================================
int LHCb::ParticleID::A( ) const
{
  // a proton can also be a Hydrogen nucleus
  if ( abspid() == 2212 ) { return 1; }
  if ( ( digit_<n10>() != 1 ) || ( digit_<n9>() != 0 ) ) { 
    // Special LHCb particle and old conventions
    const int extra = extraBits();
    if ( 45 <= extra && extra <= 47 ) { return ( extra - 43 ); } // H2, H3 and He4
    if ( 49 == extra ) { return 3; } // He3
    return 0; 
  }
  return (abspid()/10)%1000;
}


// ============================================================================
// nLambda if this is a nucleus
// ============================================================================
int LHCb::ParticleID::nLambda( ) const
{
    if( ! isNucleus() ) { return 0; }
    // a proton can also be a Hydrogen nucleus
    if( abspid() == 2212 ) { return 0; }
    // special LHCb convention for DC06
    const int extra = extraBits();
    if ( 45 <= extra && extra <= 50 ) { return 0; }
    
    return digit_<n8>();
}

// ============================================================================
// Composed by quarks and not a nucleus
// ============================================================================
bool LHCb::ParticleID::hasQuarks() const 
{
  if ( 0 < extraBits() || 100 >= abspid() ) { return false ; } ;
  const int fun = fundamentalID() ;  
  return  0 >= fun || fun > 100 ;  
}
// ============================================================================
// Quark content
// ============================================================================
bool LHCb::ParticleID::hasQuark ( const Quark& q ) const 
{
  if ( abspid() == (unsigned int)::abs(q) ) { return true ; } // Special case for quarks
  //
  return   hasQuarks() && 
    ( q == digit_<nq3> () ||
      q == digit_<nq2> () || 
      q == digit_<nq1> () ) ;
}
// ============================================================================
// Three times the charge in e+ units
// ============================================================================
int LHCb::ParticleID::threeCharge( ) const 
{
  int charge = 0;
  
  // illegal or uninitialized pid
  if ( 0 == pid () ) { return 0 ; } // REUTRN 
  
  const int extra = extraBits() ;
  
  if ( 0 < extra ) { 
    charge = 0;
    // H2 or H3 with DC06 convention
    if ( 45 == extra || 46 == extra ) { return 3 ; } // H2 or H3 
    // Alpha or He3 with DC06 convetion
    if ( 47 == extra || 49 == extra ) { return 6 ; } // alpha or He3
    // Nuclei with PDG2008 convention (2006 Monte Carlo nuclear code scheme)
    // +/- 10LZZZAAAI
    if ( isNucleus() ) { return 3*Z(); } // ion
    //
    return 0 ;
  }
  
  // KS, KL, BL, BH 
  if( 0 == digit_<nj>() ) { return 0 ; } // K0S/K0L/B0L/B0H ??
  
  // 
  static const int ch100[100] = {
    -1,  2, -1, 2, -1, 2, -1, 2, 0, 0,
    -3,  0, -3, 0, -3, 0, -3, 0, 0, 0,
    0,  0,  0, 3,  0, 0,  0, 0, 0, 0,
    0,  0,  0, 3,  0, 0,  3, 0, 0, 0,
    0, -1,  0, 0,  0, 0,  0, 0, 0, 0,
    0,  6,  3, 6,  0, 0,  0, 0, 0, 0,
    0,  0,  0, 0,  0, 0,  0, 0, 0, 0,
    0,  0,  0, 0,  0, 0,  0, 0, 0, 0,
    0,  0,  0, 0,  0, 0,  0, 0, 0, 0,
    0,  0,  0, 0,  0, 0,  0, 0, 0, 0 };
  
  const int sid = fundamentalID ();
  
  const unsigned int q1 = digit_<nq1>() ;
  const unsigned int q2 = digit_<nq2>() ;
  const unsigned int q3 = digit_<nq3>() ;
  
  // For fundamental particles use table
  if ( sid > 0 && sid <= 100 ) { charge = ch100[sid-1]; }
  // Mesons
  else if ( 0 == q1 ) 
  {
    if ( q2 == 3 || q2 == 5 )  { charge = ch100[q3-1] - ch100[q2-1] ; }
    else                       { charge = ch100[q2-1] - ch100[q3-1] ; }
  }
  // Di-quark
  else if ( 0 == q3 )          { charge = ch100[q2-1] + ch100[q1-1] ; }
  // Baryons
  else { charge = ch100[q3-1] + ch100[q2-1] + ch100[q1-1]; }
  
  
  // Set charge of antiparticle  
  if ( 0 != charge && 0 > pid() ) { charge = -charge ; }
  
  return charge;
}
// ============================================================================
// Returns 2J+1, where J is the total spin
// ============================================================================
int LHCb::ParticleID::jSpin( ) const 
{
  const unsigned int aid = abspid () ;
  if ( 130 == aid || 310 == aid  ||                   // K0S/K0L ? 
       150 == aid || 510 == aid  ) { return 1 ; }     // B0L/B0H ? 
  //
  const int fun = fundamentalID () ;
  // 
  if ( 0 < fun && fun <= 100 ) { return 0 ; }
  if ( 0 < extraBits()       ) { return 0 ; }
  //
  return digit_<nj> () ;
}
// ============================================================================
// Returns 2S+1, where S is the spin
// ============================================================================
int LHCb::ParticleID::sSpin( ) const 
{
  const unsigned int aid = abspid () ;
  if ( 130 == aid || 310 == aid  ||                   // K0S/K0L ? 
       150 == aid || 510 == aid  ) { return 1 ; }     // B0L/B0H ? 
  
  if ( !isMeson() ) { return 0 ; }

  const int jl   =  digit_<nl> () ;
  const int tent =  digit_<n>  () ;
  const int js   =  digit_<nj> () ;
  
  // tentative assignment
  if      ( tent == 9           ) { return 0 ; }
  //
  if      ( jl == 0  && js >= 3 ) { return 1 ; }
  else if ( jl == 0  && js == 1 ) { return 0 ; } 
  else if ( jl == 1  && js >= 3 ) { return 0 ; }
  else if ( jl == 2  && js >= 3 ) { return 1 ; }
  else if ( jl == 1  && js == 1 ) { return 1 ; }
  else if ( jl == 3  && js >= 3 ) { return 1 ; }
  // default set to zero
  return 0;
}
// ============================================================================
// Returns 2L+1, where L is the orbital angular momentum"
// ============================================================================
int LHCb::ParticleID::lSpin( ) const 
{
  
  const unsigned int aid = abspid () ;
  if ( 130 == aid || 310 == aid  ||                   // K0S/K0L ? 
       150 == aid || 510 == aid  ) { return 1 ; }     // B0L/B0H ? 
  
  if( !isMeson() ) { return 0 ; }
  //
  const int jl   =  digit_<nl> () ;
  const int tent =  digit_<n>  () ;
  const int js   =  digit_<nj> () ;
  // tentative assignment
  if( tent == 9 ) { return 0; }
  //
  if      ( jl == 0 && js == 3 ) { return 0 ; }
  else if ( jl == 0 && js == 5 ) { return 1 ; }
  else if ( jl == 0 && js == 7 ) { return 2 ; }
  else if ( jl == 0 && js == 9 ) { return 3 ; } 
  else if ( jl == 0 && js == 1 ) { return 0 ; }
  else if ( jl == 1 && js == 3 ) { return 1 ; }
  else if ( jl == 1 && js == 5 ) { return 2 ; }
  else if ( jl == 1 && js == 7 ) { return 3 ; }
  else if ( jl == 1 && js == 9 ) { return 4 ; }
  else if ( jl == 2 && js == 3 ) { return 1 ; }
  else if ( jl == 2 && js == 5 ) { return 2 ; }
  else if ( jl == 2 && js == 7 ) { return 3 ; }
  else if ( jl == 2 && js == 9 ) { return 4 ; }
  else if ( jl == 1 && js == 1 ) { return 1 ; }
  else if ( jl == 3 && js == 3 ) { return 2 ; }
  else if ( jl == 3 && js == 5 ) { return 3 ; }
  else if ( jl == 3 && js == 7 ) { return 4 ; }
  else if ( jl == 3 && js == 9 ) { return 5 ; }
  // default set to zero
  return 0;
}
// ============================================================================
// Fill the ASCII output stream
// ============================================================================
std::ostream& LHCb::ParticleID::fillStream ( std::ostream& s ) const 
{ return s << "LHCb.ParticleID(" << m_pid << ")" ; }
// ============================================================================
// conversion to the string
// ============================================================================
std::string LHCb::ParticleID::toString() const 
{
  std::ostringstream s ;
  fillStream ( s ) ;
  return s.str() ;
}
// ============================================================================
// print the nice representation of enum
// ============================================================================
std::ostream& LHCb::ParticleID::printLocation 
( const long    l ,
  std::ostream& s ) 
{
  switch (l) 
  {
  case LHCb::ParticleID::nj  : return s << "LHCb.ParticleID.nj"  ;
  case LHCb::ParticleID::nq3 : return s << "LHCb.ParticleID.nq3" ;
  case LHCb::ParticleID::nq2 : return s << "LHCb.ParticleID.nq2" ;
  case LHCb::ParticleID::nq1 : return s << "LHCb.ParticleID.nq1" ;
  case LHCb::ParticleID::nr  : return s << "LHCb.ParticleID.nr"  ;
  case LHCb::ParticleID::nl  : return s << "LHCb.ParticleID.nl"  ;
  case LHCb::ParticleID::n   : return s << "LHCb.ParticleID.n"   ;
  case LHCb::ParticleID::n8  : return s << "LHCb.ParticleID.n8"  ;
  case LHCb::ParticleID::n9  : return s << "LHCb.ParticleID.n9"  ;
  case LHCb::ParticleID::n10 : return s << "LHCb.ParticleID.n10" ;
  default  : ;
  }
  return s << "LHCb.ParticleID.Location(" << l << ")";
}
// ============================================================================
// print the nice representation of enum
// ============================================================================
std::string LHCb::ParticleID::printLocation ( const long l ) 
{
  std::ostringstream s ;
  printLocation ( l , s ) ;
  return s.str() ;
}
// ============================================================================
// the nice representation of 'Quark' enum
// ============================================================================
std::ostream& LHCb::ParticleID::printQuark
( const long    q , 
  std::ostream& s ) 
{
  switch ( q ) 
  {
  case LHCb::ParticleID::down    : return s << "LHCb.ParticleID.down"    ;
  case LHCb::ParticleID::up      : return s << "LHCb.ParticleID.up"      ;
  case LHCb::ParticleID::strange : return s << "LHCb.ParticleID.strange" ;
  case LHCb::ParticleID::charm   : return s << "LHCb.ParticleID.charm"   ;
  case LHCb::ParticleID::bottom  : return s << "LHCb.ParticleID.bottom"  ;
  case LHCb::ParticleID::top     : return s << "LHCb.ParticleID.top"     ;
  default : ;
  }
  return s << "LHCb.ParticleID.Quark(" << q << ")";
}
// ============================================================================
// print the nice representation of enum
// ============================================================================
std::string LHCb::ParticleID::printQuark ( const long q ) 
{
  std::ostringstream s ;
  printQuark ( q , s ) ;
  return s.str() ;
}
// ============================================================================
// the output streamer operator 
// ============================================================================
std::ostream& operator<<( std::ostream& s, const LHCb::ParticleID& o )
{ return o.fillStream ( s ) ; }
// ============================================================================
// The END 
// ============================================================================
