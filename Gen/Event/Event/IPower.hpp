// $Id: IPower.hpp,v 1.2 2008-08-01 10:16:02 ibelyaev Exp $
#ifndef LHCBMATH_IPOWER_HPP
#define LHCBMATH_IPOWER_HPP 1
// ============================================================================
// Include file
// ============================================================================
// LHcbMath
// ============================================================================
#include "Event/TypeWrapper.h"
// ============================================================================
namespace Gaudi
{
  namespace Math
  {
    // ========================================================================
    /** @struct Gaudi::Math::IPower
     *  Template metafunction for compile-time calculation of even, positive
     *  powers of integer numbers.
     *
     *  @code
     *   // calculate 2**35
     *   const int result = Gaudi::Math::IPower< int, 2,  35>::value;
     *   // calculate 16**3 in long
     *   const long result = Gaudi::Math::IPower< long, 16,  3>::value;
     *
     *  @code
     *
     *  @author Juan PALACIOS juan.palacios@cern.ch
     *  @date 2006-10-26
     */
    template<typename TYPE,  typename TypeWrapper<TYPE>::value_type X, int N>
    struct IPower
    {
      enum { value = X * IPower<TYPE, X, N-1>::value };
    };
    /// the specialization to stop the compile-time recursion
    template<typename TYPE, typename TypeWrapper<TYPE>::value_type X>
    struct IPower<TYPE, X, 1>
    {
      enum { value = X };
    };
    /// the specialization to stop the compile-time recursion
    template<typename TYPE,  typename TypeWrapper<TYPE>::value_type X>
    struct IPower<TYPE, X, 0>
    {
      enum { value = 1 };
    };
    // ========================================================================
  } // Math namespace
  // ==========================================================================
} // Gaudi namespace
// ============================================================================
// The END
// ============================================================================
#endif // LHCBMATH_IPOWER_HPP
// ============================================================================
