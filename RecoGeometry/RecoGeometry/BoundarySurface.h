//
//  BoundarySurface.h
//  
//
//  Created by Julia Hrdinka on 26/01/15.
//
//

#ifndef RECO_BOUNDARYSURFACE_H
#define RECO_BOUNDARYSURFACE_H

#include "TrkGeometryUtils/BinnedArray1D.h"
#include "RecoGeometry/Material.h"
#include "RecoGeometry/Surface.h"

namespace Reco {

    class Volume;
    
    class BoundarySurface  {
        
    public:
    
        typedef Trk::BinnedArray1D<Volume> VolumeArray;
        
        //copy constructor
        BoundarySurface(const BoundarySurface& boundarysurface);
        //desctructor
        virtual ~BoundarySurface();
        //clone method
        virtual BoundarySurface* clone() const = 0;
        //assignment operator
        BoundarySurface& operator=(const BoundarySurface& boundarysurface);
        //get the next volume depending on the starting point on the surface (if global position is not on the surface return nullptr) and the direction
        const Volume* nextVolume(const Alg::Point3D& glopos) const;
        //set the next volume touching the BoundarySurface - spherical direction beginning from center of the detector to outside
        virtual void setNextVolume(std::shared_ptr<const Volume> nextVolume) const;
        //set the previous volume touching the BoundarySurface - spherical direction beginning from center of the detector to outside
        virtual void setPreviousVolume(std::shared_ptr<const Volume> previousVolume) const;
        //set the next volumes touching the BoundarySurface as a 1D binned array - spherical direction beginning from center of the detector to outside
        //binned in z for CylinderSurface
        //binned in r for DiscSurface
        virtual void setNextVolumes(const VolumeArray* nextVolumes) const;
        //set the previous volumes touching the BoundarySurface as a 1D binned array - spherical direction beginning from center of the detector outside
        //binned in z for CylinderSurface
        //binned in r for DiscSurface
        virtual void setPreviousVolumes(const VolumeArray* previousVolumes) const;
        //get the next volume touching the BoundarySurface - spherical direction beginning from center of the detector to outside
        virtual const Volume* getNextVolume() const;
        //get the previous volume touching the BoundarySurface - spherical direction beginning from center of the detector to outside
        virtual const Volume* getPreviousVolume() const;
        //get the next volumes touching the BoundarySurface as a 1D binned array - spherical direction beginning from center of the detector to outside
        //binned in z for CylinderSurface
        //binned in r for DiscSurface
        virtual const VolumeArray* getNextVolumes() const;
        //get the previous volumes touching the BoundarySurface as a 1D binned array - spherical direction beginning from center of the detector to outside
        //binned in z for CylinderSurface
        //binned in r for DiscSurface
        virtual const VolumeArray* getPreviousVolumes() const;
        //returns the surface representation of the BoundarySurface - for straightLineIntersection
        virtual const Surface* surfaceRepresentation() const = 0;
        //set the material of the boundarysurface - constant over the surface
        void setMaterial(Material* material);
        //get the material of the boundarysurface - constant over the surface
        Material* getMaterial() const;
        
    protected:
        //default constructor for inherited classes
        BoundarySurface();

        mutable std::shared_ptr<const Volume>   m_nextVolume;
        mutable std::shared_ptr<const Volume>   m_previousVolume;
        //if there are more next/previous volumes: BinnedArray1D (in z for CylinderBoundarySutface or in r for DiscBoundarySurface) for next/previous volumes
        mutable const VolumeArray*              m_nextVolumes;
        mutable const VolumeArray*              m_previousVolumes;
        //material of the BoundarySurface - possibility to have support structure
        Material*                               m_material;
    };
}
#endif //RECO_BOUNDARYSURFACE_H
