// ============================================================================
#ifndef LHCBMATH_DIGIT_H
#define LHCBMATH_DIGIT_H 1
// ============================================================================
// Include files
// ============================================================================
// LHCbMath
// ============================================================================
#include "Event/Power.h"
#include <limits>
// ============================================================================
/** @file
 *
 *  The collection of useful utilities for evaluation of
 *  decimal digits for unsigned integral values.
 *  The implementation is based on 'constexpr' and allows the compiler to 
 *  early evaluate given expressions transparently.   
 *
 *  Nth decimal digit for the unsigned integral values:
 *
 *   -  Gaudi::Math::digit
 *
 *  decimal N1->N2 decimal digits for the unsigned integral values:
 *
 *   -  Gaudi::Math::digits
 *      The regular function, the least efficient evaluator
 *
 *  @attention the least significant decimal digit is numbered as #0
 *
 *  @see Gaudi::Math::digit
 *
 *  @see Gaudi::Math::digits
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @author Benedikt HEGNER benedikt.hegner@cern.ch
 *  @date 2015-08-08
 */
// ============================================================================
namespace Gaudi
{
  namespace Math
  {
    // ========================================================================
    /** simple function which evaluate N-th decimal digit for the
     *  integral value
     *
     *  @code
     *
     *   unsigned int value = ... ;
     *   unsigned int N     = ... ;
     *
     *   const int digN = Gaudi::Math::digit ( value , N ) ;
     *
     *  @endcode
     *
     *  @attention the least significat decimal digit is numbered as #0
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @author Benedikt HEGNER benedikt.hegner@cern.ch
     *  @date 2015-08-08
     */
    template <class TYPE>
    constexpr inline TYPE digit ( const TYPE value , const unsigned int N  )
    {
      // ======================================================================
      static_assert (  std::numeric_limits<TYPE>::is_specialized, "" ) ;
      static_assert (  std::numeric_limits<TYPE>::is_integer, ""    ) ;
      static_assert ( !std::numeric_limits<TYPE>::is_signed, ""      ) ;
      // ======================================================================
      return (N > (unsigned int) std::numeric_limits<TYPE>::digits10) ? 0 : (value/Gaudi::Math::pow ( 10 , N )) %10;
    }

    // ========================================================================
    /** simple function which evaluate the range of decimal digits
     *  N1-.N2 (N2 is excluded) for the integral values
     *
     *  @code
     *
     *   unsigned int value = ... ;
     *   unsigned int N1 = ... ;
     *   unsigned int N2 = ... ;
     *
     *   const bool digN1N2 = Gaudi::Math::digits( value , N1 , N2  ) ;
     *
     *  @endcode
     *
     *  @attention the least significat decimal digit is numbered as #0
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @author Benedikt HEGNER benedikt.hegner@cern.ch 
     *  @date 2015-08-08
     */
    template <class TYPE>
    constexpr inline TYPE digits ( const TYPE         value ,
                         const unsigned int N1    ,
                         const unsigned int N2    )
    {
      // ======================================================================
      static_assert (  std::numeric_limits<TYPE>::is_specialized, "" ) ;
      static_assert (  std::numeric_limits<TYPE>::is_integer, ""    ) ;
      static_assert ( !std::numeric_limits<TYPE>::is_signed, ""      ) ;
      // ======================================================================
      return (N2 >  1 + std::numeric_limits<TYPE>::digits10) ? 
	     digits ( value , N1 , 1 + std::numeric_limits<TYPE>::digits10 ) :
             (N1 >= N2 || N1 > (unsigned int) std::numeric_limits<TYPE>::digits10 ) ?
	     0 : 
	     ( (value/Gaudi::Math::pow(10,N1) )%Gaudi::Math::pow(10,N2-N1) );
    }
    // ========================================================================
  } // end of namespace Gaudi::Math
} // end of namespace Gaudi
// ============================================================================
// The END
// ============================================================================
#endif // LHCBMATH_DIGIT_H
// ============================================================================
