#ifndef MATH_MATH_H
#define MATH_MATH_H

/** @file Math.h Math/Math.h
 * Math.h
 *
 * This file contains needed math type definitions. These are directly taken
 * from ACTS which uses Eigen as an underlying math library.
 *
 *  @author Julia Hrdinka
 *  @date 2017-03
 */

#include "ACTS/Utilities/Definitions.hpp"

namespace fcc {

/// definition of a 2D Vector
typedef Acts::Vector2D Vector2D;
/// definition of a 3D Vector
typedef Acts::Vector3D Vector3D;
/// definition of a matrix of doubles
template <unsigned int rows, unsigned int cols>
using MatrixD = Acts::ActsMatrixD<rows, cols>;

}  // end of namespace fcc

#endif  // MATH_MATH_H
