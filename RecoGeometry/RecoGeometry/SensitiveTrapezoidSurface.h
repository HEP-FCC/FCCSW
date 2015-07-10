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
        SensitiveTrapezoidSurface(TGeoNode* node, TGeoTrd2* trapezoid, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        //constructor to set transform manually
        SensitiveTrapezoidSurface(TGeoTrd2* trapezoid, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        //constructors with materialmap
        SensitiveTrapezoidSurface(TGeoNode* node, TGeoTrd2* trapezoid, MaterialMap* materialmap, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        SensitiveTrapezoidSurface(TGeoTrd2* trapezoid, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        //manuel constructor, to set transform and dimensions manuel
        SensitiveTrapezoidSurface(std::shared_ptr<const Alg::Transform3D> transf, double halfX1, double halfX2, double halfY, double HalfThickness, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        //copy constructor
        SensitiveTrapezoidSurface(const SensitiveTrapezoidSurface& trapezoidsurface);
        //destructor
        virtual ~SensitiveTrapezoidSurface();
        //clone method
        virtual SensitiveTrapezoidSurface* clone() const override;
        //assignment operator
        SensitiveTrapezoidSurface& operator=(const SensitiveTrapezoidSurface& senstrap);
        //returns if the surface is sensitive (and has a readout)
        virtual bool isSensitive() const override;
        //returns the surface representation of the sensitive surface
        virtual const TrapezoidSurface* surfaceRepresentation() const override;
    };
}

#endif //RECO_TRAPEZOIDSENSITIVESURFACE_H
