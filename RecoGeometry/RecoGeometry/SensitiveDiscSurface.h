//
//  SensitiveDiscSurface.h
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#ifndef RECO_SENSITIVEDISCSURFACE_H
#define RECO_SENSITIVEDISCSURFACE_H

#include "RecoGeometry/DiscSurface.h"
#include "RecoGeometry/SensitiveSurface.h"
#include "TrkGeometryUtils/ReadoutSegmentation.h"

#include "TrkGeometryUtils/BinUtility.h"

namespace Reco {
    
    class SensitiveDiscSurface : public DiscSurface, public SensitiveSurface {
    
    public:
        //constructor from TGeoGeometry for the conversion of the DD4Hep Geometry in the RecoGeoConverterTool
        //material is set atomatically (node->Material)
        SensitiveDiscSurface(TGeoNode* node, TGeoConeSeg* tube, long long int volumeID, Trk::ReadoutSegmentation* segmentation);
        SensitiveDiscSurface(TGeoConeSeg* tube, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, Trk::ReadoutSegmentation* segmentation);
        //constructor to set Material extra
        SensitiveDiscSurface(TGeoNode* node, TGeoConeSeg* tube, MaterialMap* materialmap, long long int volumeID, Trk::ReadoutSegmentation* segmentation);
        SensitiveDiscSurface(TGeoConeSeg* tube, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, Trk::ReadoutSegmentation* segmentation);
        //manuel constructor with transform matrix and dimensions of the Disc
        SensitiveDiscSurface(std::shared_ptr<const Alg::Transform3D> transf, double Rmin, double Rmax, long long int volumeID, Trk::ReadoutSegmentation* segmentation);
        //copy constructor
        SensitiveDiscSurface(const SensitiveDiscSurface& sensdisc);
        //copy constructor
        virtual ~SensitiveDiscSurface();
        //clone method
        virtual SensitiveDiscSurface* clone() const override;
        //assignment operator
        SensitiveDiscSurface& operator=(const SensitiveDiscSurface& sensdisc);
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
        //BinUtility 2D grid for every surface
        Trk::ReadoutSegmentation*    m_segmentation;
    
    };
}

#endif //RECO_SENSITIVEDISCSURFACE_H
