//
//  ContainerVolume.h
//  
//
//  Created by Julia Hrdinka on 26/01/15.
//
//

#ifndef RECO_CONTAINERVOLUME_H
#define RECO_CONTAINERVOLUME_H

#include "Algebra/AlgPrimitives.h"
#include "TrkGeometryUtils/BinnedArray.h"
#include "RecoGeometry/Volume.h"

namespace Reco {
    
    class ContainerVolume : public virtual Volume {
        
    public:
        
        typedef Trk::BinnedArray<Volume> VolumeArray;
        
        //standard constructor with a BinnedArray of the containing volumes
        ContainerVolume(VolumeArray* volumes);
        //copyconstructor
        ContainerVolume(const ContainerVolume& containervolume);
        //checks if global position glopos is inside bounds
        virtual bool isInside(const Alg::Point3D& glopos, double tol=0.) const = 0;
        //destructor
        virtual ~ContainerVolume();
        //clone Method
        virtual ContainerVolume* clone() const = 0;
        //assignment operatot
        ContainerVolume& operator=(const ContainerVolume& containervolume);
        //set the boundarysurface at n - for definition of n see specific volumes (such as CylinderVolume)
        virtual bool setBoundarySurface(size_t n, std::shared_ptr<const BoundarySurface> boundarysurface) const;
        //get volume at the global position glopos - this volume can itself also be a containervolume
        const Volume* getVolume(const Alg::Point3D& glopos) const;
        
    private:
        //BinnedArray of the containing volumes
        //binned either in z or in r for the ContainerCylinderVolume
        VolumeArray* m_volumes;
    };
}


#endif //RECO_CONTAINERVOLUME_H
