// $Id: ParticleID.h 151438 2013-02-01 11:19:58Z ibelyaev $ 
// ============================================================================
#ifndef LHCbKernel_ParticleID_H
#define LHCbKernel_ParticleID_H 1
// ============================================================================
// Include files
// ============================================================================
#include <iosfwd>
#include <string>
#include <cmath>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// LHCbMath 
// ============================================================================
#include "Event/Digit.h"
// ============================================================================
namespace LHCb 
{
  // ==========================================================================
  /** @class ParticleID ParticleID.h
   *
   * Holds PDG+LHCb extension particle code, following the <a
   * href="http://pdg.lbl.gov/2006/reviews/montecarlorpp.pdf">PDG particle
   * numbering scheme</a>. Based on HepPDT class 
   *
   * @author Gloria Corti
   * created Mon Dec  1 12:25:08 2008
   */
  class GAUDI_API ParticleID
  {
  public:
    // ========================================================================
    /// PID digits (base 10) are: n nr nl nq1 ne2 nq3 nj
    enum Location {
      nj=1 ,
      nq3  ,
      nq2  ,
      nq1  ,
      nl   ,
      nr   ,
      n    ,
      n8   ,
      n9   ,
      n10
    } ;
    /// Quark PDG codes
    enum Quark { 
      down=1  ,
      up      ,
      strange ,
      charm   ,
      bottom  ,
      top
    } ;
    // ========================================================================
  public:
    // ========================================================================
    /// Constructor with PDG code
    explicit ParticleID(int pid) : m_pid( pid ) {}
    /// Copy Constructor
    ParticleID(const ParticleID& orig) : m_pid ( orig.m_pid ) {}
    /// Default Constructor
    ParticleID() : m_pid(0) {}
    /// Default Destructor
    ~ParticleID() {}
    // ========================================================================
  public:
    // ========================================================================
    /// Fill the ASCII output stream
    std::ostream& fillStream ( std::ostream& s ) const;
    /// string representation 
    std::string   toString () const ;
    /// Equality operator
    bool operator ==( const ParticleID& other ) const
    { return m_pid == other.m_pid ; }
    /// Non-Equality operator
    bool operator !=( const ParticleID& other ) const
    { return m_pid != other.m_pid ; }
    /// Comparison operator
    bool operator < ( const ParticleID& other ) const
    {
      const unsigned int i1 = abspid()       ;
      const unsigned int i2 = other.abspid() ; 
      return 
        i1 < i2 ? true  : 
        i2 < i1 ? false : m_pid < other.m_pid ;
    }
    // ========================================================================
  public:
    // ========================================================================
    /// Retrieve const  PDG code
    int          pid    () const { return m_pid ; }
    /// Absolute value of PDG code
    unsigned int abspid () const { return 0 > m_pid ? -m_pid : m_pid ; }
    /// Update  PDG code
    void setPid ( int value ) { m_pid = value; }
    // ========================================================================
  public:
    // ========================================================================    
    /// Classification of particles
    bool isValid    () const ;
    /// Classification of particles
    bool isMeson    () const ;
    /// Classification of particles
    bool isBaryon   () const ;
    /// Classification of particles
    bool isDiQuark  () const ;
    /// Classification of particles
    bool isHadron   () const 
    { return hasQuarks () && ( isMeson () || isBaryon () ) ; }
    /// Classification of particles
    bool isLepton   () const ;
    /// Classification of particles
    bool isNucleus  () const;
    /// Quark contents
    bool hasUp      () const { return hasQuark ( up      ) ; }
    /// Quark contents
    bool hasDown    () const { return hasQuark ( down    ) ; }
    /// Quark contents
    bool hasStrange () const { return hasQuark ( strange ) ; }
    /// Quark contents
    bool hasCharm   () const { return hasQuark ( charm   ) ; }
    /// Quark contents
    bool hasBottom  () const { return hasQuark ( bottom  ) ; }
    /// Quark contents
    bool hasTop     () const { return hasQuark ( top     ) ; }
    /// Quark contents
    bool hasQuark   ( const Quark& q ) const;
    /// Contains quarks but not a nucleus
    bool hasQuarks  () const ;
    /// Atomic number if a nucleus
    int  Z          () const ;
    /// Nucleon number if a nucleus
    int  A          () const ;
    /// nLambda if this is a nucleus
    int  nLambda    () const ;
    // ========================================================================
  public:
    // ========================================================================
    /// Three times the charge (in positron charge units)
    int threeCharge () const;
    /// Returns 2J+1, where J is the total spin
    int jSpin       () const;
    /// Returns 2S+1, where S is the spin
    int sSpin       () const;
    /// Returns 2L+1, where L is the orbital angular momentum
    int lSpin       () const;
    // ========================================================================
  public: // some semitechnical stuff 
    // ========================================================================
    /** First two digits if it is a 'fundamental ' particle.
     *  Will return digits for quarks, leptons, higgs, etc. 
     *  but 0 for mesons, baryons
     */
    int fundamentalID() const { 
      if( ( 1 == digit_<n10>() ) && ( 0 == digit_<n9>() ) ) { return 0; }
      return 0 == digits_<nq2,nr>() ? abspid()%10000 : 0 ; 
    }
    /// Value of digit in specified location
    unsigned short digit ( const Location& loc) const
    { return Gaudi::Math::digit ( abspid(), loc - 1  ) ; }
    /// Everything behind the 7th digit (e.g. outside the numbering scheme)
    int extraBits () const { return abspid() / 10000000 ; }    
    // ========================================================================
  public: // pure technical stuff 
    // ========================================================================
    /// get the digit at the given (fixed) location 
    template <Location L>
    int digit_() const 
    {
      Gaudi::Math::Digit<unsigned int,L-1> _eval ;
      return _eval ( abspid() ) ;
    } 
    /// get the digits at the given fixed range 
    template <Location L1,Location L2>
    int digits_() const 
    {
      Gaudi::Math::Digits<unsigned int,L1-1,L2-1> _eval ;
      return _eval ( abspid() ) ;
    } 
    // ========================================================================
  public:
    // ========================================================================
    // print the quark to the stream 
    static std::ostream& printQuark    ( const long    q , 
                                         std::ostream& s ) ;
    // convert the quark to the  string
    static std::string   printQuark    ( const long    q ) ;
    // print the location to the stream 
    static std::ostream& printLocation ( const long    l , 
                                         std::ostream& s ) ;
    // convert the location to the  string
    static std::string   printLocation ( const long    l ) ;
    // ========================================================================
  private:
    // ========================================================================
    /// PDG Code 
    int m_pid; // PDG code
    // ========================================================================
  }; // class ParticleID
  // ==========================================================================
  /// the output streamer operator 
  inline std::ostream& operator<< ( std::ostream&              s , 
                                    const LHCb::ParticleID&    o ) 
  { return o.fillStream ( s ) ; }
  /// the nice representation of 'Location' enum
  inline std::ostream& operator<< ( std::ostream&              s , 
                                    LHCb::ParticleID::Location l ) 
  { return LHCb::ParticleID::printLocation ( l , s ) ; }
  /// the nice representation of 'Quark' enum
  inline std::ostream& operator<< ( std::ostream&              s , 
                                    LHCb::ParticleID::Quark    q ) 
  { return LHCb::ParticleID::printQuark ( q , s ) ; }
  // ==========================================================================
} //                                                      end of namespace LHCb
// ============================================================================
#include "GaudiKernel/HashMap.h"
// ============================================================================
namespace GaudiUtils 
{
  // ==========================================================================
  // Hash functions for maps
  template <> struct Hash<LHCb::ParticleID>
  { inline size_t operator() ( const LHCb::ParticleID& s ) const { return (size_t)s.pid(); } };
  template <> struct Hash<const LHCb::ParticleID>
  { inline size_t operator() ( const LHCb::ParticleID& s ) const { return (size_t)s.pid(); } };
  template <> struct Hash<LHCb::ParticleID&>
  { inline size_t operator() ( const LHCb::ParticleID& s ) const { return (size_t)s.pid(); } };
  template <> struct Hash<const LHCb::ParticleID&>
  { inline size_t operator() ( const LHCb::ParticleID& s ) const { return (size_t)s.pid(); } };
  // ==========================================================================
}
// ============================================================================
namespace std
{
  // ==========================================================================
  inline 
  LHCb::ParticleID abs ( const LHCb::ParticleID& p )  
  { return LHCb::ParticleID ( p.abspid() ) ; }
  // ==========================================================================
}
// ============================================================================
#endif ///LHCbKernel_ParticleID_H
// ============================================================================
// The END 
// ============================================================================
