//
//  SurfaceTest.h
//  
//
//  Created by Julia Hrdinka on 18/09/14.
//
//

#ifndef _SurfaceTest_h
#define _SurfaceTest_h

#include <stdio.h>
#include <iostream>
//Trk
/*#include "TrkSurfaces/Surface.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/SurfaceBounds.h"
#include "TrkSurfaces/PlaneSurface.h"
#include "TrkSurfaces/RectangleBounds.h"
#include "TrkSurfaces/TriangleBounds.h"
#include "TrkSurfaces/TrapezoidBounds.h"
//#include "TrkSurfaces/RotatedTrapezoidBounds.h"
#include "TrkSurfaces/DiamondBounds.h"
//#include "TrkSurfaces/EllipseBounds.h"
#include "TrkSurfaces/NoBounds.h"
#include "TrkSurfaces/CylinderSurface.h"
#include "TrkSurfaces/CylinderBounds.h"
#include "TrkSurfaces/DiscSurface.h"
#include "TrkSurfaces/DiscBounds.h"
#include "TrkSurfaces/PerigeeSurface.h"

#include "TrkVolumes/Volume.h"
#include "TrkVolumes/VolumeBounds.h"
#include "TrkVolumes/AbstractVolume.h"
#include "TrkVolumes/CylinderVolumeBounds.h"

#include "TrkParametersBase/ParametersBase.h"
#include "TrkParametersBase/ParametersT.h"
#include "TrkParametersBase/Charged.h"
#include "TrkParametersBase/Neutral.h"*/
//Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/DataObjectHandle.h"
// Alg
#include "Algebra/AlgPrimitives.h"


class SurfaceTest: public GaudiAlgorithm {
    friend class AlgFactory<SurfaceTest> ;
    
public:
    /// Constructor.
    SurfaceTest(const std::string& name, ISvcLocator* svcLoc);
    /// Initialize.
    virtual StatusCode initialize();
    /// Execute.
    virtual StatusCode execute();
    /// Finalize.
    virtual StatusCode finalize();

};



#endif
