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
#include "RecoGeometry/ReadoutSegmentation.h"
#include <memory>
#include "DD4hep/Detector.h"
#include "RecoGeometry/Surface.h"

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
//        unsigned long bin(const Alg::Point2D& locpos) const;
        //returns a global unique ID for this local position on the surface
        long long int cellID(const Alg::Point2D& locpos) const;
        //returns the corresponding local position to this local bin
//        Alg::Point2D binToLocpos(unsigned long bin) const;
        //returns the globalposition belonging to this cellID
        Alg::Point2D locpos(long long int cellID) const;
        //returns the cell width, ba represents the coordinate
//        float binWidth(const Alg::Point2D& locpos, size_t ba=0) const;
        //returns CellID at this position plus the surrounding CellID's
        const std::set<long long int> neighbours(long long int cellID) const;
        //returns the surface representation of the sensitive surface
        virtual const Surface* surfaceRepresentation() const = 0;
   
    protected:
        
        SensitiveSurface(long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        
        //uniquely identifies the position of the surface in the detector
        long long int m_volumeID;
        std::shared_ptr<const DD4hep::Geometry::Segmentation> m_segmentation;
//        mutable std::ofstream m_IDs;
    };
}

#endif //RECO_SENSITIVESURFACE_H
