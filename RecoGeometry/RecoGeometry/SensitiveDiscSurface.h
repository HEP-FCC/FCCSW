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
        SensitiveDiscSurface(TGeoNode* node, TGeoConeSeg* tube, size_t binsR, size_t binsPhi, long long int volumeID);
        SensitiveDiscSurface(TGeoConeSeg* tube, std::shared_ptr<const Alg::Transform3D> transf, size_t binsR , size_t binsPhi, long long int volumeID);
        //constructor to set Material extra
        SensitiveDiscSurface(TGeoNode* node, TGeoConeSeg* tube, MaterialMap* materialmap, size_t binsR, size_t binsPhi, long long int volumeID);
        SensitiveDiscSurface(TGeoConeSeg* tube, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, size_t binsR, size_t binsPhi, long long int volumeID);
        //manuel constructor with transform matrix and dimensions of the Disc
        SensitiveDiscSurface(std::shared_ptr<const Alg::Transform3D> transf, double Rmin, double Rmax, size_t binsR, size_t binsPhi, long long int volumeID);
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
        virtual size_t bin(const Alg::Point2D& locpos) const override;
        
    private:
        //BinUtility 2D grid for every surface
        Trk::BinUtility*    m_binutility;
    
    };
}

#endif //RECO_SENSITIVEDISCSURFACE_H
