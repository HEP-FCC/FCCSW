//
//  ContainerCylinderVolume.h
//  
//
//  Created by Julia Hrdinka on 27/01/15.
//
//

#ifndef RECO_CONTAINERCYLINDERVOLUME_H
#define RECO_CONTAINERCYLINDERVOLUME_H

#include "RecoGeometry/ContainerVolume.h"
#include "RecoGeometry/CylinderVolume.h"

namespace Reco {
    
    class ContainerCylinderVolume : public ContainerVolume, public CylinderVolume {
        
    public:
        
        //Type for binning the Volumes in Z with the BinUtility
        enum binZType {
            nEnd      = 0,
            nBarrel   = 1,
            pBarrel   = 2,
            pEnd      = 3,
            zLength   = 4
        };
        //Type for binning the Volumes in R with the BinUtility
        enum binRType {
            inner   = 0,
            middle  = 1,
            outer   = 2,
            rLength = 3
        };
        //constructor with a BinnedArray of the containing volumes, the transformation matrix and the dimensions of the volume
        ContainerCylinderVolume(VolumeArray* volumes, std::shared_ptr<const Alg::Transform3D> transf, double Rmin, double Rmax, double halfZ);
        //constructs volume from TGeoNode(transformation matrix) and TGeoConeSeg(dimensions) and a BinnedArray of the containing volumes
        ContainerCylinderVolume(VolumeArray* volumes, TGeoNode* node, TGeoConeSeg* tube);
        //copyconstructor
        ContainerCylinderVolume(const ContainerCylinderVolume& containercylindervolume);
        //destructor
        ~ContainerCylinderVolume();
        //clone method
        virtual ContainerCylinderVolume* clone() const override;
        //assignment operator
        ContainerCylinderVolume& operator=(const ContainerCylinderVolume& containercylindervolume);
        //checks if global position glopos is inside the bounds of the volume with a tolerance tol
        virtual bool isInside(const Alg::Point3D& glopos, double tol=0.) const;
        //checks if global position glopos is inside the bounds of the volume with a tolerance tol
        virtual bool setBoundarySurface(size_t n, std::shared_ptr<const BoundarySurface> boundarysurface) const;
    };
}

#endif //RECO_CONTAINERCYLINDERVOLUME_H
