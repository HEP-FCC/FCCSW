//
//  SensitiveSurface.h
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#ifndef RECO_SENSITIVESURFACE_H
#define RECO_SENSITIVESURFACE_H

#include "Algebra/AlgPrimitives.h"

namespace Reco {
    
    /*struct {
    
        VolID : 32
        ModuleID : 32
    }*/
    
    class SensitiveSurface {
        
    public:
        
        //copy constructor
        SensitiveSurface(const SensitiveSurface& senssurf);
        //destructor
        virtual ~SensitiveSurface();
        //clone method
        virtual SensitiveSurface* clone() const = 0;
        //assignment operator
        SensitiveSurface& operator=(const SensitiveSurface& senssurf);
        //every position on the surface corresponds to a unique bin of this sensitive surface
        //calculates the unique bin in the local 2D grid of the surface corresponding to this local position locpos
        virtual size_t bin(const Alg::Point2D& locpos) const = 0;
        //CellidMethode
        unsigned long long cellID(const Alg::Point2D& locpos);
    protected:
        SensitiveSurface(unsigned long surfaceID);
        
        //uniquely identifies the position of the surface in the detector
        unsigned long m_surfaceID;
        
    };
}

#endif //RECO_SENSITIVESURFACE_H
