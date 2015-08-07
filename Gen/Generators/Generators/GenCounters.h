// $Id: GenCounters.h,v 1.5 2006-09-19 21:41:43 gcorti Exp $
#ifndef GENERATORS_GENCOUNTERS_H 
#define GENERATORS_GENCOUNTERS_H 1

// Include files
#include "GaudiKernel/MsgStream.h"
#include "Generators/ICounterLogFile.h"

#include <cmath>
#include <numeric>

namespace boost {
  template< typename T , std::size_t N > class array ;
}

namespace HepMC {
  class GenEvent ;
}

/** @namespace GenCounters
 *  
 *  Utility functions to print counters and compute efficiencies
 * 
 *  @author Patrick Robbe
 *  @date   2005-11-30
 */

namespace GenCounters {
  /// Type for hadron counter
  typedef boost::array< unsigned int , 5 > BHadronCounter ;
  typedef boost::array< unsigned int , 4 > DHadronCounter ;
  typedef boost::array< std::string  , 5 > BHadronCNames  ;
  typedef boost::array< std::string  , 4 > DHadronCNames  ;

  enum bHadronCounterType{ Bd = 0 , ///< counter of B0
                           Bu , ///< counter of B+
                           Bs , ///< counter of Bs0
                           bBaryon , ///< counter of b-baryon
                           Bc , ///< counter of Bc+
  } ;
  
  enum cHadronCounterType{ D0 , ///< counter of D0
                           Dch , ///< counter of D+
                           Ds , ///< counter of Ds
                           cBaryon , ///< counter of c-baryon
  } ;


  /// Type for excited states counters
  typedef boost::array< unsigned int , 3 > ExcitedCounter ;
  typedef boost::array< std::string  , 3 > ExcitedCNames  ;

  enum excitedCounterType { _0star , ///< counter of X (spin 0, ang mom 0)
                            _1star , ///< counter of X* (spin 1, ang mom 0)
                            _2star  ///< counter of X** (ang mom 1)
  } ;

  /** Compute fraction of two numbers A/B
   *  @param[in] A  Numerator 
   *  @param[in] B  Denominator
   *  @return A/B
   */
  inline double fraction( const unsigned int A , const unsigned int B ) {
    return ( (double) A / (double) B ) ;
  }
  
  /** Compute statistical error on fraction
   *  @param[in] A  Numberator
   *  @param[in] B  Denominator
   *  @return Error on A/B
   */
  inline double err_fraction( const unsigned int A , const unsigned int B ) {
    return sqrt( A * ( 1. - ( (double) A / (double) B ) ) ) / ( (double) B ) ;
  }
  
  /** Print efficiency computed with 2 counters.
   *  @param[in] theStream  Print facility to print the counters and efficiency
   *                        after/before.
   *  @param[in] cutName    Description of the cut
   *  @param[in] after      Number of events after the cut
   *  @param[in] before     Number of events before the cut
   */
  inline void printEfficiency( MsgStream & theStream , 
                               const std::string & cutName , 
                               const unsigned int after , 
                               const unsigned int before ) {
    if ( 0 == before ) return ;
    theStream << "Number of events for " << cutName << ", before : " << before
              << ", after : " << after 
              << std::endl ;
    theStream << "Efficiency of the " << cutName << " : " 
              << format( "%.5g +/- %.5g" , fraction( after , before ) , 
                         err_fraction( after , before ) ) << std::endl ;
  }

  /** Print efficiency computed with 2 counters.
   *  @param[in] theLogFile Log file where to print the counters and efficiency
   *                        after/before.
   *  @param[in] cutName    Description of the cut
   *  @param[in] after      Number of events after the cut
   *  @param[in] before     Number of events before the cut
   */
  inline void printEfficiency( ICounterLogFile * theLogFile , 
                               const std::string & cutName , 
                               const unsigned int after , 
                               const unsigned int before ) {
    if ( 0 == before ) return ;
    theLogFile -> addEfficiency( cutName , after , before , 
                                 fraction( after , before ) , 
                                 err_fraction( after , before ) ) ;
  }  

  /** Print a simple counter.
   *  @param[in] theStream  Print facility to print the counter
   *  @param[in] name       Description of the counter
   *  @param[in] value      Value of the counter
   */  
  inline void printCounter( MsgStream & theStream , const std::string & name , 
                            const unsigned int value ) {
    theStream << "Number of " << name << " : " << value << std::endl ; 
  }

  /** Print a simple counter.
   *  @param[in] theLogFile Log file to print the counter
   *  @param[in] name       Description of the cut
   *  @param[in] after      Number of events after the cut
   *  @param[in] before     Number of events before the cut
   */  
  inline void printCounter( ICounterLogFile * theLogFile , const std::string & name , 
                            const unsigned int value ) {
    theLogFile -> addCounter( name , value ) ;
  }
  
  /** Print fraction computed from two counters.
   *  @param[in] theStream  Print facility to print the counters and fraction
   *                        number/total.
   *  @param[in] name       Description of the fraction
   *  @param[in] number     Number of particles contained in the fraction
   *  @param[in] total      Total number of particles.
   */
  inline void printFraction( MsgStream & theStream , 
                             const std::string & name , 
                             const unsigned int number , 
                             const unsigned int total ) {
    theStream << "Number of " << name << " : " << number ;
    theStream << " [fraction : " 
              << format( "%.5g +/- %.5g]" , fraction( number , total ) ,
                         err_fraction( number , total ) ) << std::endl ;  
  }

  /** Print fraction computed from two counters.
   *  @param[in] theLogFile Log file where to print the counters and fraction
   *                        number/total.
   *  @param[in] name       Description of the fraction
   *  @param[in] number     Number of particles contained in the fraction
   *  @param[in] total      Total number of particles.
   */
  inline void printFraction( ICounterLogFile * theLogFile , 
                             const std::string & name , 
                             const unsigned int number , 
                             const unsigned int total ) {
    theLogFile -> addFraction( name , number , fraction( number , total ) , 
                               err_fraction( number , total ) ) ; 
  }

  /** Utility function to add an array to another one
   *  @param[in,out] A   Adds content of B to A
   *  @param[in]     B   Array to add to content of A 
   */
  template< typename T , std::size_t N > 
  inline void AddTo( boost::array< T , N > & A ,
                     const boost::array< T , N > & B ) {
    std::transform( A.begin() , A.end() , B.begin() , A.begin() , 
                    std::plus< unsigned int >( ) ) ;
  }

  /** Utility function to print Fractions from a array of counters
   *  @param[in] 
   */
  template< typename T , std::size_t N >
  inline void printArray( MsgStream & theStream ,
                          boost::array< T , N > A ,
                          boost::array< std::string , N > AName ,
                          const std::string & root ) {
    unsigned int total = std::accumulate( A.begin() , A.end() , 0 ) ;
    for ( unsigned int i = 0 ; i < A.size() ; ++i ) 
      printFraction( theStream , root + " " + AName[ i ] , A[ i ] , total ) ;
  }

  /** Utility function to print Fractions from a array of counters
   *  @param[in] 
   */
  template< typename T , std::size_t N >
  inline void printArray( ICounterLogFile * theLogFile ,
                          boost::array< T , N > A ,
                          boost::array< std::string , N > AName ,
                          const std::string & root ) {
    unsigned int total = std::accumulate( A.begin() , A.end() , 0 ) ;
    for ( unsigned int i = 0 ; i < A.size() ; ++i ) 
      printFraction( theLogFile , root + " " + AName[ i ] , A[ i ] , total ) ;
  }

  /** Utility function to setup names of B hadron counters 
   *  @param[in] BC      Names of B hadron counters
   *  @param[in] antiBC  Names of anti-B hadron counters
   */
  void setupBHadronCountersNames( BHadronCNames & BC , 
                                  BHadronCNames & antiBC ) ;

  /** Utility function to setup names of D hadron counters 
   *  @param[in] DC      Names of D hadron counters
   *  @param[in] antiDC  Names of anti-D hadron counters
   */
  void setupDHadronCountersNames( DHadronCNames & DC , 
                                  DHadronCNames & antiDC ) ;

  /** Utility function to setup names of excited states counters 
   *  @param[in] DC      Names of excited states counters
   *  @param[in] root    Basename 
   */
  void setupExcitedCountersNames( ExcitedCNames & B , 
                                  const std::string & root ) ;

  /** Update counters for excited states.
   *  @param[in]  theEvent     The event where to count.
   *  @param[out] thebExcitedC Counters for B(**) 
   *  @param[out] thecExcitedC Counters for D(**)
   */
  void updateExcitedStatesCounters( const HepMC::GenEvent * theEvent ,
                                    ExcitedCounter & thebExcitedC ,
                                    ExcitedCounter & thecExcitedC ) ;

  /** Update counters for flavour fractions.
   *  @param[in]  theEvent     The event where to count.
   *  @param[out] thebHadC     Counters for b-hadrons
   *  @param[out] theantibHadC Counters for anti-b-hadrons
   *  @param[out] thecHadC     Counters for c-hadrons
   *  @param[out] theanticHadC Counters for anti-c-hadrons
   *  @param[out] thebbCounter Counters for (bb) states
   *  @param[out] theccCounter Counters for (cc) states
   */
  void updateHadronCounters( const HepMC::GenEvent * theEvent , 
                             BHadronCounter & thebHadC , 
                             BHadronCounter & theantibHadC ,
                             DHadronCounter & thecHadC ,
                             DHadronCounter & theanticHadC , 
                             unsigned int & thebbCounter , 
                             unsigned int & theccCounter ) ;
}

#endif // GENERATORS_GENCOUNTERS_H
