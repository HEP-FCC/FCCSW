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
        virtual unsigned long bin(const Alg::Point2D& locpos) const = 0;
        //returns a global unique ID for this local position on the surface
        unsigned long long cellID(const Alg::Point2D& locpos) const;
        //returns the corresponding local position to this local bin
        virtual Alg::Point2D binToLocpos(unsigned long bin) const = 0;
        //returns the globalposition belonging to this cellID
        Alg::Point2D locpos(unsigned long long cellID) const;
        //returns the cell width, ba represents the coordinate
        virtual float binWidth(const Alg::Point2D& locpos, size_t ba=0) const = 0;
        //returns bin at this position plus the surrounding bins
        virtual const std::vector<unsigned long> compatibleBins(const Alg::Point2D& locpos) const = 0;
    protected:
        
        SensitiveSurface(unsigned long surfaceID);
        
        //uniquely identifies the position of the surface in the detector
        unsigned long m_surfaceID;
        
//        mutable std::ofstream m_IDs;
    };
}

#endif //RECO_SENSITIVESURFACE_H
