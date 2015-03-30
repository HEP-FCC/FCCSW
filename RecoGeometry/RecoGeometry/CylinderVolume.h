//
//  CylinderVolume.h
//  
//
//  Created by Julia Hrdinka on 09/01/15.
//
//

#ifndef RECO_CYLINDERVOLUME_H
#define RECO_CYLINDERVOLUME_H

#include "RecoGeometry/Volume.h"
#include "TGeoCone.h"

namespace Reco {
    
    class CylinderVolume : public virtual Volume {
        
    public:
        
        enum BoundarySurfaceType {
            negDisc       = 0,
            posDisc       = 1,
            outerCylinder = 2,
            innerCylinder = 3,
            length        = 4
        };
        
        //see Volume.h
        enum Coordinates {
            Rmin    = 0,
            Rmax    = 1,
            halfZ   = 2
        };
        //constructor with transformation matrix for a volume with layers
        CylinderVolume(std::shared_ptr<const Alg::Transform3D> transf, LayerArray* layers, double Rmin, double Rmax, double halfZ);
        //constructor with transformation matrix for volume without layers
        CylinderVolume(std::shared_ptr<const Alg::Transform3D> transf, double Rmin, double Rmax, double halfZ);
        //constructors from TGeoGeometry for the conversion of the DD4Hep Geometry in the RecoGeoConverterTool
        //constructor with TGeoNode(transform is set automatically) and TGeoConeSeg(coordinates are set) with layers
        CylinderVolume(LayerArray* layers, TGeoNode* node, TGeoConeSeg* tube);
        //constructor with TGeoNode(transform is set automatically) and TGeoConeSeg(coordinates are set) without layers
        CylinderVolume(TGeoNode* node, TGeoConeSeg* tube);
        //copyconstructor
        CylinderVolume(const CylinderVolume& cylindervolume);
        //destructor
        virtual ~CylinderVolume();
        //clone Method
        virtual CylinderVolume* clone() const override;
        //assignment operator
        CylinderVolume& operator=(const CylinderVolume& cylindervolume);
        //get coordinates
        double getRmin() const;
        double getRmax() const;
        double getHalfZ() const;
        //get the BoundarySurfaces
        const BoundarySurface* getPosDisc() const;
        const BoundarySurface* getNegDisc() const;
        const BoundarySurface* getOuterCylinder() const;
        const BoundarySurface* getInnerCylinder() const;
        //set pointer to boundary surface at n - for definition of n see enum BoundarySurfaceType
        virtual bool setBoundarySurface(size_t n, std::shared_ptr<const BoundarySurface> boundarysurface) const;
        //checks if global position glopos is inside the bounds of the volume with a tolerance tol
        virtual bool isInside(const Alg::Point3D& glopos, double tol) const override;

        
    private:
        //coordinates
        //inner radius (can be zero)
        double          m_Rmin;
        //outer radius
        double          m_Rmax;
        //half length in z
        double          m_halfZ;

    };
}

#endif //RECO_CYLINDERVOLUME_H
