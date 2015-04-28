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
#include "TrkGeometryUtils/ReadoutSegmentation.h"

#include "TrkGeometryUtils/BinUtility.h"

namespace Reco {
    
    class SensitiveTrapezoidSurface : public TrapezoidSurface, public SensitiveSurface {
    public:
        
        //constructors from TGeoGeometry for the conversion of the DD4Hep Geometry in the RecoGeoConverterTool
        //constructor with TGeoNode - transform is set automatically
        //binsX and binsY are the number of bins in X and Y for the readout at this surface
        SensitiveTrapezoidSurface(TGeoNode* node, TGeoTrd2* trapezoid, long long int volumeID, Trk::ReadoutSegmentation* segmentation);
        //constructor to set transform manually
        SensitiveTrapezoidSurface(TGeoTrd2* trapezoid, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, Trk::ReadoutSegmentation* segmentation);
        //constructors with materialmap
        SensitiveTrapezoidSurface(TGeoNode* node, TGeoTrd2* trapezoid, MaterialMap* materialmap, long long int volumeID, Trk::ReadoutSegmentation* segmentation);
        SensitiveTrapezoidSurface(TGeoTrd2* trapezoid, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, Trk::ReadoutSegmentation* segmentation);
        //manuel constructor, to set transform and dimensions manuel
        SensitiveTrapezoidSurface(std::shared_ptr<const Alg::Transform3D> transf, double halfX1, double halfX2, double halfY, double HalfThickness, long long int volumeID, Trk::ReadoutSegmentation* segmentation);
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
        virtual unsigned long bin(const Alg::Point2D& locpos) const override;
        //returns the corresponding local position to this local bin
        virtual Alg::Point2D binToLocpos(unsigned long bin) const override;
        //returns if the surface is sensitive (and has a readout)
        virtual bool isSensitive() const override;
        //returns the pitch width, ba represents the coordinate
        virtual float binWidth(const Alg::Point2D& locpos, size_t ba=0) const override;
        //returns bin at this position plus the surrounding bins
        virtual const std::vector<unsigned long> compatibleBins(const Alg::Point2D& locpos) const override;

    private:
        
        Trk::ReadoutSegmentation* m_segmentation;
    };
}

#endif //RECO_TRAPEZOIDSENSITIVESURFACE_H
