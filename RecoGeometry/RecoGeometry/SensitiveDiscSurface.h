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

#include "TrkGeometryUtils/BinUtility.h"

namespace Reco {
    
    class SensitiveDiscSurface : public DiscSurface, public SensitiveSurface {
    
    public:
        //constructor from TGeoGeometry for the conversion of the DD4Hep Geometry in the RecoGeoConverterTool
        //material is set atomatically (node->Material)
        SensitiveDiscSurface(TGeoNode* node, TGeoConeSeg* tube, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        SensitiveDiscSurface(TGeoConeSeg* tube, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        //constructor to set Material extra
        SensitiveDiscSurface(TGeoNode* node, TGeoConeSeg* tube, MaterialMap* materialmap, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        SensitiveDiscSurface(TGeoConeSeg* tube, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        //manuel constructor with transform matrix and dimensions of the Disc
        SensitiveDiscSurface(std::shared_ptr<const Alg::Transform3D> transf, double Rmin, double Rmax, double HalfThickness, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation);
        //copy constructor
        SensitiveDiscSurface(const SensitiveDiscSurface& sensdisc);
        //copy constructor
        virtual ~SensitiveDiscSurface();
        //clone method
        virtual SensitiveDiscSurface* clone() const override;
        //assignment operator
        SensitiveDiscSurface& operator=(const SensitiveDiscSurface& sensdisc);
        //returns if the surface is sensitive (and has a readout)
        virtual bool isSensitive() const override;
        //returns the surface representation of the sensitive surface
        virtual const DiscSurface* surfaceRepresentation() const override;
    };
}

#endif //RECO_SENSITIVEDISCSURFACE_H
