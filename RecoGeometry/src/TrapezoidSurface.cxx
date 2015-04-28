//
//  TrapezoidSurface.cxx
//  
//
//  Created by Julia Hrdinka on 16/02/15.
//
//

#include "RecoGeometry/TrapezoidSurface.h"


Reco::TrapezoidSurface::TrapezoidSurface() :
Surface(),
m_halfX1(0.),
m_halfX2(0.),
m_halfY(0.)
{}

Reco::TrapezoidSurface::TrapezoidSurface(TGeoNode* node, TGeoTrd2* trapezoid) :
Surface(node)
{
    //convention: y'=Z, thicknesss = Y(X,Z)
    m_halfX1 = trapezoid->GetDx1();
    m_halfX2 = trapezoid->GetDx2();
    m_halfY  = trapezoid->GetDz();
}

Reco::TrapezoidSurface::TrapezoidSurface(TGeoTrd2* trapezoid, std::shared_ptr<const Alg::Transform3D> transf) :
Reco::Surface(transf)
{
    //convention: y'=Z, thicknesss = Y(X,Z)
    m_halfX1 = trapezoid->GetDx1();
    m_halfX2 = trapezoid->GetDx2();
    m_halfY  = trapezoid->GetDz();
}

Reco::TrapezoidSurface::TrapezoidSurface(TGeoNode* node, TGeoTrd2* trapezoid, Reco::MaterialMap* materialmap) :
Reco::Surface(node, materialmap)
{
    //convention: y'=Z, thicknesss = Y(X,Z)
    m_halfX1 = trapezoid->GetDx1();
    m_halfX2 = trapezoid->GetDx2();
    m_halfY  = trapezoid->GetDz();
}

Reco::TrapezoidSurface::TrapezoidSurface(TGeoTrd2* trapezoid, Reco::MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf) :
Reco::Surface(materialmap,transf)
{
    //convention: y'=Z, thicknesss = Y(X,Z)
    m_halfX1 = trapezoid->GetDx1();
    m_halfX2 = trapezoid->GetDx2();
    m_halfY  = trapezoid->GetDz();
}

Reco::TrapezoidSurface::TrapezoidSurface(std::shared_ptr<const Alg::Transform3D> transf, double halfX1, double halfX2, double halfY) :
Reco::Surface(transf)
{

    m_halfX2 = halfX2;
    m_halfX1 = halfX1;
    m_halfY  = halfY;
}

Reco::TrapezoidSurface::TrapezoidSurface(const TrapezoidSurface& trapezoidsurface) :
Reco::Surface(trapezoidsurface),
m_halfX1(trapezoidsurface.m_halfX1),
m_halfX2(trapezoidsurface.m_halfX2),
m_halfY(trapezoidsurface.m_halfY)
{}

Reco::TrapezoidSurface::~TrapezoidSurface()
{}

Reco::TrapezoidSurface* Reco::TrapezoidSurface::clone() const
{
    return (new Reco::TrapezoidSurface(*this));
}

Reco::TrapezoidSurface& Reco::TrapezoidSurface::operator=(const TrapezoidSurface& trapezoidsurface)
{
    if (this!=&trapezoidsurface) {
        Reco::Surface::operator=(trapezoidsurface);
        m_halfX1 = trapezoidsurface.m_halfX1;
        m_halfX2 = trapezoidsurface.m_halfX2;
        m_halfY  = trapezoidsurface.m_halfY;
    }
    return (*this);
}

double Reco::TrapezoidSurface::getHalfX1() const
{
    return (m_halfX1);
}

double Reco::TrapezoidSurface::getHalfX2() const
{
    return (m_halfX2);
}

double Reco::TrapezoidSurface::getHalfY() const
{
    return (m_halfY);
}

const Alg::Vector3D& Reco::TrapezoidSurface::normal() const
{
    return (Reco::Surface::normal());
}

const Alg::Vector3D* Reco::TrapezoidSurface::normal(const Alg::Point2D&) const
{
    return (new Alg::Vector3D(normal()));
}

const Reco::Material* Reco::TrapezoidSurface::material(Alg::Point2D& locpos) const
{
    if (materialmap()->binutility()) {
        int biny = materialmap()->binutility()->bin(locpos,0);
        if (materialmap()->binvectorAt(biny)) {
            int binx = materialmap()->binvectorAt(biny)->bin(locpos,1);
            std::pair<int,int> bins = std::make_pair(binx,biny);
            return (materialmap()->material(bins));
        }
    }
    return 0;
}

bool Reco::TrapezoidSurface::isInside(const Alg::Point2D& locpos, double tol1, double tol2) const
{
    double xmin = 0;
    double xmax = 0;
    if (m_halfX2>m_halfX1) {
        xmin = m_halfX1;
        xmax = m_halfX2;
    }
    else {
        xmin = m_halfX2;
        xmax = m_halfX1;
    }
    if (fabs(locpos.X())>(xmax+tol1)) return false;
    if (fabs(locpos.Y())>(m_halfY+tol2)) return false;
    if (fabs(locpos.X())<(xmin-tol1)) return true;
    if (m_halfX1==m_halfX2) return true;

    double sign = ((locpos.X()>0.) ? 1. : -1. );
    double k = sign*(2.*m_halfY)/(m_halfX2-m_halfX1);
    double d = -k*(m_halfX2+m_halfX1)*0.5;
    double x = (locpos.Y()-d)/k;
    if (fabs(locpos.X())<fabs(x+tol1)) return true;
    else return false;
}

void Reco::TrapezoidSurface::localToGlobal(const Alg::Point2D& locpos, const Alg::Vector3D&, Alg::Point3D& glopos) const
{
    Alg::Point3D loc3D (locpos.X(), locpos.Y(), 0.);
    glopos = (transform()*loc3D);
}

bool Reco::TrapezoidSurface::globalToLocal(const Alg::Point3D& glopos, const Alg::Vector3D&, Alg::Point2D& locpos) const
{
    Alg::Point3D loc3D (transform().Inverse()*glopos);
    locpos.SetCoordinates(loc3D.X(),loc3D.Y());
    return(isInside(locpos,s_onSurfaceTolerance,s_onSurfaceTolerance) && (loc3D.Z()*loc3D.Z())<(s_onSurfaceTolerance*s_onSurfaceTolerance));
}

bool Reco::TrapezoidSurface::isSensitive() const
{
    return false;
}
