//
//  AlgPrimitives.h
//  
//
//  Created by Julia Hrdinka on 10/09/14.
//
//

#ifndef _AlgPrimitives_h
#define _AlgPrimitives_h

#include "Math/GenVector/Transform3D.h"
#include "Math/GenVector/Rotation3D.h"
#include "Math/GenVector/DisplacementVector2D.h"
#include "Math/GenVector/DisplacementVector3D.h"
#include "Math/SVector.h"
#include "Math/GenVector/PositionVector2D.h"
#include "Math/GenVector/PositionVector3D.h"
#include "Math/GenVector/AxisAngle.h"
#include "TGeoMatrix.h"
#include "Math/GenVector/RotationX.h"
#include "Math/GenVector/RotationY.h"
#include "Math/GenVector/RotationZ.h"

namespace Alg
{
    typedef ROOT::Math::Transform3D             Transform3D;
    typedef ROOT::Math::Rotation3D              RotationMatrix3D;
    typedef ROOT::Math::RotationX               RotationX;
    typedef ROOT::Math::RotationY               RotationY;
    typedef ROOT::Math::RotationZ               RotationZ;
    
    typedef ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<double>, ROOT::Math::DefaultCoordinateSystemTag>    Vector3D;
    typedef ROOT::Math::DisplacementVector2D<ROOT::Math::Cartesian2D<double>,ROOT::Math::DefaultCoordinateSystemTag>     Vector2D;
    typedef ROOT::Math::PositionVector2D<ROOT::Math::Cartesian2D<double>,ROOT::Math::DefaultCoordinateSystemTag>         Point2D;
    typedef ROOT::Math::PositionVector3D<ROOT::Math::Cartesian3D<double>, ROOT::Math::DefaultCoordinateSystemTag>        Point3D;
    
    typedef TGeoMatrix      GeoMatrix;
    typedef TGeoRotation    Rotation3D;
    
    template <int DIM> using AmgVector = ROOT::Math::SVector<double,DIM>;
    template <int DIM> using AmgSymMatrix = ROOT::Math::MatRepSym<double,DIM>;

}

#endif
