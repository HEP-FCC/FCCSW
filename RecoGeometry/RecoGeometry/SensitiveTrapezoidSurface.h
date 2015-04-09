//
//  TrapezoidSensitiveSurface.h
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#ifndef RECO_TRAPEZOIDSENSITIVESURFACE_H
#define RECO_TRAPEZOIDSENSITIVESURFACE_H

#include "RecoGeometry/TrapezoidSurface.h"
#include "RecoGeometry/SensitiveSurface.h"

#include "TrkGeometryUtils/BinUtility.h"

namespace Reco {
    
    class SensitiveTrapezoidSurface : public TrapezoidSurface, public SensitiveSurface {
    public:
        
        //constructors from TGeoGeometry for the conversion of the DD4Hep Geometry in the RecoGeoConverterTool
        //constructor with TGeoNode - transform is set automatically
        //binsX and binsY are the number of bins in X and Y for the readout at this surface
        SensitiveTrapezoidSurface(TGeoNode* node, TGeoTrd2* trapezoid, size_t binsX, size_t binsY, long long int volumeID);
        //constructor to set transform manually
        SensitiveTrapezoidSurface(TGeoTrd2* trapezoid, std::shared_ptr<const Alg::Transform3D> transf, size_t binsX, size_t binsY, long long int volumeID);
        //constructors with materialmap
        SensitiveTrapezoidSurface(TGeoNode* node, TGeoTrd2* trapezoid, MaterialMap* materialmap, size_t binsX, size_t binsY, long long int volumeID);
        SensitiveTrapezoidSurface(TGeoTrd2* trapezoid, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, size_t binsX, size_t binsY, long long int volumeID);
        //manuel constructor, to set transform and dimensions manuel
        SensitiveTrapezoidSurface(std::shared_ptr<const Alg::Transform3D> transf, double halfX1, double halfX2, double halfY, size_t binsX, size_t binsY, long long int volumeID);
        //copy constructor
        SensitiveTrapezoidSurface(const SensitiveTrapezoidSurface& trapezoidsurface);
        //destructor
        virtual ~SensitiveTrapezoidSurface();
        //clone method
        virtual SensitiveTrapezoidSurface* clone() const override;
        //assignment operator
        SensitiveTrapezoidSurface& operator=(const SensitiveTrapezoidSurface& senstrap);
        //every position on the surface corresponds to a unique bin of this surface
        //calculates the unique bin in the local 2D grid of the surface corresponding to this local position locpos
        virtual size_t bin(const Alg::Point2D& locpos) const override;

    private:
        
        //BinUtility 2D grid for every surface
        Trk::BinUtility*    m_binutility;
        std::vector<Trk::BinUtility*>*  m_binvector;
    
    };
}

#endif //RECO_TRAPEZOIDSENSITIVESURFACE_H
