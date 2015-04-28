//
//  BoundaryDiscSurface.cxx
//  
//
//  Created by Julia Hrdinka on 26/01/15.
//
//

#include "RecoGeometry/BoundaryDiscSurface.h"

Reco::BoundaryDiscSurface::BoundaryDiscSurface(TGeoNode* node, TGeoConeSeg* tube) :
BoundarySurface(),
DiscSurface(node,tube)
{}

Reco::BoundaryDiscSurface::BoundaryDiscSurface(TGeoConeSeg* tube, std::shared_ptr<const Alg::Transform3D> transf) :
BoundarySurface(),
DiscSurface(tube, transf)
{}

Reco::BoundaryDiscSurface::BoundaryDiscSurface(std::shared_ptr<const Alg::Transform3D> transf, double Rmin, double Rmax, double HalfThickness) :
BoundarySurface(),
DiscSurface(transf,Rmin,Rmax, HalfThickness)
{}

Reco::BoundaryDiscSurface::BoundaryDiscSurface(const Reco::BoundaryDiscSurface& boundarydiscsurface) :
BoundarySurface(boundarydiscsurface),
DiscSurface(boundarydiscsurface)
{}


Reco::BoundaryDiscSurface::~BoundaryDiscSurface()
{}

Reco::BoundaryDiscSurface* Reco::BoundaryDiscSurface::clone() const
{
    return (new Reco::BoundaryDiscSurface(*this));
}

Reco::BoundaryDiscSurface& Reco::BoundaryDiscSurface::operator=(const BoundaryDiscSurface& boundarydiscsurface)
{
    if (this!=&boundarydiscsurface) {
        Reco::BoundarySurface::operator=(boundarydiscsurface);
        Reco::Surface::operator=(boundarydiscsurface);
    }
    return (*this);
}

const Reco::Material* Reco::BoundaryDiscSurface::material(Alg::Point2D&) const
{
    return(BoundarySurface::getMaterial());
}

const Reco::DiscSurface* Reco::BoundaryDiscSurface::surfaceRepresentation() const
{
    return (this);
}

bool Reco::BoundaryDiscSurface::isSensitive() const
{
    return false;
}