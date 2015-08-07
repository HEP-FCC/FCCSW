// ============================================================================
#ifndef LHCBMATH_POWER_H 
#define LHCBMATH_POWER_H 1

// ============================================================================
namespace Gaudi 
{ 
  // ==========================================================================
  namespace Math 
  {
    // ========================================================================
    /** Simple utility for efficient "pow".
     *  It works only for positive integer powers.
     *
     *  @code 
     *   
     *   const double result = Gaudi::Math::pow ( value , 10 ) ;
     *
     *  @endcode 
     *
     * inspired by 
     *  http://stackoverflow.com/questions/17719674/c11-fast-constexpr-integer-powers
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr (original code)
     *  @author Benedikt HEGNER benedikt.hegner@cern.ch  (constexpr code)
     *  @date 2015-08-08 
     */
    template<typename TYPE>
    constexpr inline TYPE pow ( TYPE __x , unsigned long __y, TYPE result = 1 ){
      return __y < 1 ? result : pow(__x*__x, __y/2, (__y % 2) ? result*__x : result);
    }
    // ========================================================================
  } //                                                    end of namespace Math
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
// The END 
// ============================================================================
#endif // LHCBMATH_POWER_H
// ============================================================================
