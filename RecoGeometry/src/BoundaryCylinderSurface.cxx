//
//  BoundaryCylinderSurface.cxx
//  
//
//  Created by Julia Hrdinka on 26/01/15.
//
//

#include "RecoGeometry/BoundaryCylinderSurface.h"

Reco::BoundaryCylinderSurface::BoundaryCylinderSurface(TGeoNode* node, TGeoConeSeg* tube) :
BoundarySurface(),
CylinderSurface(node,tube)
{}

Reco::BoundaryCylinderSurface::BoundaryCylinderSurface(TGeoConeSeg* tube, std::shared_ptr<const Alg::Transform3D> transf) :
BoundarySurface(),
CylinderSurface(tube,transf)
{}

Reco::BoundaryCylinderSurface::BoundaryCylinderSurface(std::shared_ptr<const Alg::Transform3D> transf, double radius, double halfZ) :
BoundarySurface(),
CylinderSurface(transf,radius,halfZ)
{}

Reco::BoundaryCylinderSurface::BoundaryCylinderSurface(const Reco::BoundaryCylinderSurface& boundarycylindersurface) :
BoundarySurface(boundarycylindersurface),
CylinderSurface(boundarycylindersurface)
{}

Reco::BoundaryCylinderSurface::~BoundaryCylinderSurface()
{}

Reco::BoundaryCylinderSurface* Reco::BoundaryCylinderSurface::clone() const
{
    return (new Reco::BoundaryCylinderSurface(*this));
}

Reco::BoundaryCylinderSurface& Reco::BoundaryCylinderSurface::operator=(const BoundaryCylinderSurface& boundarycylindersurface)
{
    if (this!=&boundarycylindersurface) {
        Reco::BoundarySurface::operator=(boundarycylindersurface);
        Reco::Surface::operator=(boundarycylindersurface);
    }
    return (*this);
}

const Reco::Material* Reco::BoundaryCylinderSurface::material(Alg::Point2D&) const
{
    return(BoundarySurface::getMaterial());
}

const Reco::CylinderSurface* Reco::BoundaryCylinderSurface::surfaceRepresentation() const
{
    return (this);
}
