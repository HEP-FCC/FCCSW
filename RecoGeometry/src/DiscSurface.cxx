//
//  DiscSurface.cxx
//  
//
//  Created by Julia Hrdinka on 02/12/14.
//
//

#include "RecoGeometry/DiscSurface.h"
#include "Algebra/RealQuadraticEquation.h"

Reco::DiscSurface::DiscSurface() :
Reco::Surface(),
m_Rmin(0.),
m_Rmax(0.),
m_thickness(0.)
{}

Reco::DiscSurface::DiscSurface(std::shared_ptr<const Alg::Transform3D> transf, double Rmin, double Rmax, double HalfThickness) :
Reco::Surface(transf),
m_Rmin(Rmin),
m_Rmax(Rmax),
m_thickness(HalfThickness)
{}

Reco::DiscSurface::DiscSurface(TGeoNode* node, TGeoConeSeg* tube) :
Reco::Surface(node)
{
    m_Rmin      = tube->GetRmin1();
    m_Rmax      = tube->GetRmax1();
    m_thickness = tube->GetDZ();
}

Reco::DiscSurface::DiscSurface(TGeoConeSeg* tube, std::shared_ptr<const Alg::Transform3D> transf) :
Reco::Surface(transf)
{
    m_Rmin      = tube->GetRmin1();
    m_Rmax      = tube->GetRmax1();
    m_thickness = tube->GetDZ();
}

Reco::DiscSurface::DiscSurface(TGeoNode* node, TGeoConeSeg* tube, Reco::MaterialMap* materialmap) :
Reco::Surface(node, materialmap)
{
    m_Rmin      = tube->GetRmin1();
    m_Rmax      = tube->GetRmax1();
    m_thickness = tube->GetDZ();
}

Reco::DiscSurface::DiscSurface(TGeoConeSeg* tube, Reco::MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf) :
Reco::Surface(materialmap, transf)
{
    m_Rmin      = tube->GetRmin1();
    m_Rmax      = tube->GetRmax1();
    m_thickness = tube->GetDZ();
}

Reco::DiscSurface::DiscSurface(const Reco::DiscSurface& discsurface) :
Reco::Surface(discsurface),
m_Rmin(discsurface.m_Rmin),
m_Rmax(discsurface.m_Rmax),
m_thickness(discsurface.m_thickness)
{}

Reco::DiscSurface::~DiscSurface()
{}

Reco::DiscSurface* Reco::DiscSurface::clone() const
{
    return (new Reco::DiscSurface(*this));
}

Reco::DiscSurface& Reco::DiscSurface::operator=(const DiscSurface& discsurface)
{
    if (this!=&discsurface) {
        Reco::Surface::operator=(discsurface);
        m_Rmin      = discsurface.m_Rmin;
        m_Rmax      = discsurface.m_Rmax;
        m_thickness = discsurface.m_thickness;
    }
    return (*this);
}

double Reco::DiscSurface::getRmin() const
{
    return (m_Rmin);
}

double Reco::DiscSurface::getRmax() const
{
    return (m_Rmax);
}

double Reco::DiscSurface::thickness() const
{
    return (m_thickness);
}

const Alg::Vector3D& Reco::DiscSurface::normal() const
{
    return (Reco::Surface::normal());
}

const Alg::Vector3D* Reco::DiscSurface::normal(const Alg::Point2D&) const
{
    return (new Alg::Vector3D(normal()));
}

const Reco::Material* Reco::DiscSurface::material(Alg::Point2D& locpos) const
{
    if (materialmap()->binutility()) {
        int binx = materialmap()->binutility()->bin(locpos,0);
        int biny = materialmap()->binutility()->bin(locpos,1);
        std::pair<int,int> bins = std::make_pair(binx,biny);
        return (materialmap()->material(bins));
    }
    return 0;
}

bool Reco::DiscSurface::isInside(const Alg::Point2D& locpos, double tol1, double tol2) const
{
    return ((fabs(locpos.Y()) <= (2.*M_PI + tol2)) && (locpos.X() >= (m_Rmin - tol1)) && (locpos.X() <= (m_Rmax + tol1)));
}

void Reco::DiscSurface::localToGlobal(const Alg::Point2D& locpos, const Alg::Vector3D&, Alg::Point3D& glopos) const
{
    double x        = locpos.X()*cos(locpos.Y());
    double y        = locpos.X()*sin(locpos.Y());
    Alg::Point3D loc3D(x,y,0.);
    glopos          = transform()*loc3D;
}

bool Reco::DiscSurface::globalToLocal(const Alg::Point3D& glopos, const Alg::Vector3D&, Alg::Point2D& locpos) const
{
    Alg::Point3D loc3D(transform().Inverse()*glopos);
    double r        = sqrt(loc3D.Perp2());
    double phi      = ((fabs(loc3D.X()) < s_zeroLimit) &&  (fabs(loc3D.Y()) < s_zeroLimit)) ? 0 : atan2(loc3D.Y(),loc3D.X());
    locpos.SetCoordinates(r,phi);
    return (isInside(locpos,s_onSurfaceTolerance,s_onSurfaceTolerance) && (loc3D.Z()*loc3D.Z())<(s_onSurfaceTolerance*s_onSurfaceTolerance));
}

bool Reco::DiscSurface::isSensitive() const
{
    return false;
}