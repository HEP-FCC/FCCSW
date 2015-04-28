//
//  BoundaryDiscSurface.h
//  
//
//  Created by Julia Hrdinka on 26/01/15.
//
//

#ifndef RECO_BOUNDARYDISCSURFACE_H
#define RECO_BOUNDARYDISCSURFACE_H

#include "RecoGeometry/DiscSurface.h"
#include "RecoGeometry/BoundarySurface.h"

namespace Reco {
    
    class BoundaryDiscSurface : public BoundarySurface, public DiscSurface {
        
    public:
        //constructors from TGeoGeometry for the conversion of the DD4Hep Geometry in the RecoGeoConverterTool
        //constructor with TGeoNode - transform is set automatically
        BoundaryDiscSurface(TGeoNode* node, TGeoConeSeg* tube);
        //constructor to set transform manually
        BoundaryDiscSurface(TGeoConeSeg* tube, std::shared_ptr<const Alg::Transform3D> transf);
        //manuel constructor, to set transform and dimensions manuel
        BoundaryDiscSurface(std::shared_ptr<const Alg::Transform3D> transf, double Rmin, double Rmax);
        //copy constructor
        BoundaryDiscSurface(const BoundaryDiscSurface& boundarydiscsurface);
        //destructor
        ~BoundaryDiscSurface();
        //clone method
        virtual BoundaryDiscSurface* clone() const override;
        //assignment operator
        BoundaryDiscSurface& operator=(const BoundaryDiscSurface& boundarydiscsurface);
        //get Material on a local position of the surface
        //for a boundarysurface material is constant over the surface
        virtual const Material* material(Alg::Point2D& locpos) const override;
        //returns the surface representation of the boundary surface
        virtual const DiscSurface* surfaceRepresentation() const override;
        //returns if the surface is sensitive (and has a readout)
        virtual bool isSensitive() const override;
    };
}
#endif //RECO_BOUNDARYDISCSURFACE_H
