//
//  BoundaryCylinderSurface.h
//  
//
//  Created by Julia Hrdinka on 26/01/15.
//
//

#ifndef RECO_BOUNDARYCYLINDERSURFACE_H
#define RECO_BOUNDARYCYLINDERSURFACE_H

#include "RecoGeometry/CylinderSurface.h"
#include "RecoGeometry/BoundarySurface.h"

namespace Reco {

    class BoundaryCylinderSurface : public BoundarySurface, public CylinderSurface {
    
    public:
        //constructors from TGeoGeometry for the conversion of the DD4Hep Geometry in the RecoGeoConverterTool
        //constructor with TGeoNode - transform is set automatically
        BoundaryCylinderSurface(TGeoNode* node, TGeoConeSeg* tube);
        //constructor to set transform manually
        BoundaryCylinderSurface(TGeoConeSeg* tube, std::shared_ptr<const Alg::Transform3D> transf);
        //manuel constructor, to set transform and dimensions manuel
        BoundaryCylinderSurface(std::shared_ptr<const Alg::Transform3D> transf, double radius, double halfZ);
        //copy constructor
        BoundaryCylinderSurface(const BoundaryCylinderSurface& boundarycylindersurface);
        //destructor
        ~BoundaryCylinderSurface();
        //clone method
        virtual BoundaryCylinderSurface* clone() const override;
        //assignment operator
        BoundaryCylinderSurface& operator=(const BoundaryCylinderSurface& boundarycylindersurface);
        //get Material on a local position of the surface
        //for boundarysurface material is constant over the surface
        virtual const Material* material(Alg::Point2D& locpos) const override;
        //returns the surfacerepresentation
        virtual const CylinderSurface* surfaceRepresentation() const override;
        //returns if the surface is sensitive (and has a readout)
        virtual bool isSensitive() const override;
    };
}


#endif //RECO_BOUNDARYCYLINDERSURFACE_H
