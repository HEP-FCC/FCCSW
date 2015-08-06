//
//  PlaneSurface.cxx
//  
//
//  Created by Julia Hrdinka on 25/11/14.
//
//

#include "RecoGeometry/PlaneSurface.h"

Reco::PlaneSurface::PlaneSurface() :
Reco::Surface(),
m_halfX(0.),
m_halfY(0.),
m_halfThickness(0.)
{}

Reco::PlaneSurface::PlaneSurface(std::shared_ptr<const Alg::Transform3D> transf, double halfX, double halfY, double HalfThickness) :
Reco::Surface (transf),
m_halfX(halfX),
m_halfY(halfY),
m_halfThickness(HalfThickness)
{}

Reco::PlaneSurface::PlaneSurface(TGeoNode* node, TGeoBBox* box) :
Reco::Surface (node)
{
    m_halfX     = box->GetDX();
    m_halfY     = box->GetDY();
    m_halfThickness = box->GetDZ();
}

Reco::PlaneSurface::PlaneSurface(TGeoBBox* box, std::shared_ptr<const Alg::Transform3D> transf) :
Reco::Surface (transf)
{
    m_halfX     = box->GetDX();
    m_halfY     = box->GetDY();
    m_halfThickness = box->GetDZ();;
}

Reco::PlaneSurface::PlaneSurface(TGeoNode* node,  TGeoBBox* box, Reco::MaterialMap* materialmap) :
Reco::Surface (node, materialmap)
{
    m_halfX     = box->GetDX();
    m_halfY     = box->GetDY();
    m_halfThickness = box->GetDZ();
}

Reco::PlaneSurface::PlaneSurface(TGeoBBox* box, Reco::MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf) :
Reco::Surface (materialmap, transf)
{
    m_halfX     = box->GetDX();
    m_halfY     = box->GetDY();
    m_halfThickness = box->GetDZ();
}

Reco::PlaneSurface::PlaneSurface(const PlaneSurface& planesurface) :
Reco::Surface(planesurface),
m_halfX(planesurface.m_halfX),
m_halfY(planesurface.m_halfY),
m_halfThickness(planesurface.m_halfThickness)
{}

Reco::PlaneSurface::~PlaneSurface()
{}

Reco::PlaneSurface* Reco::PlaneSurface::clone() const
{
    return (new Reco::PlaneSurface(*this));
}

Reco::PlaneSurface& Reco::PlaneSurface::operator=(const PlaneSurface& planesurface)
{
    if (this!=&planesurface) {
        Reco::Surface::operator=(planesurface);
        m_halfX = planesurface.m_halfX;
        m_halfY = planesurface.m_halfY;
        m_halfThickness = planesurface.m_halfThickness;
    }
    return (*this);
}

double Reco::PlaneSurface::getHalfX() const
{
    return (m_halfX);
}

double Reco::PlaneSurface::getHalfY() const
{
    return (m_halfY);
}

double Reco::PlaneSurface::halfThickness() const
{
    return (m_halfThickness);
}

/*double Reco::PlaneSurface::halfThickness(const Alg::Point2D& locpos) const
{
    if (material(locpos)) return (material(locpos).thickness);
    return
}*/

const Alg::Vector3D& Reco::PlaneSurface::normal() const
{
    return (Reco::Surface::normal());
}

const Alg::Vector3D* Reco::PlaneSurface::normal(const Alg::Point2D&) const
{
    return (new Alg::Vector3D(normal()));
}

const Reco::Material* Reco::PlaneSurface::material(Alg::Point2D& locpos) const
{
    if (materialmap()) return (materialmap()->material(locpos));
    return 0;
}

bool Reco::PlaneSurface::isInside(const Alg::Point2D& locpos, double tol1, double tol2) const
{
    return ((fabs(locpos.X()) < (m_halfX + tol1)) && (fabs(locpos.Y()) < (m_halfY + tol2)));
}

void Reco::PlaneSurface::localToGlobal(const Alg::Point2D& locpos, const Alg::Vector3D&, Alg::Point3D& glopos) const
{
    Alg::Point3D loc3D (locpos.X(),locpos.Y(),0.);
    glopos = (transform()*loc3D);
}

bool Reco::PlaneSurface::globalToLocal(const Alg::Point3D& glopos, const Alg::Vector3D&, Alg::Point2D& locpos) const
{
    Alg::Point3D loc3D (transform().Inverse()*glopos);
    locpos.SetCoordinates(loc3D.X(),loc3D.Y());
    return(isInside(locpos,s_onSurfaceTolerance,s_onSurfaceTolerance) && (loc3D.Z()*loc3D.Z())<(halfThickness()*halfThickness()));//(s_onSurfaceTolerance*s_onSurfaceTolerance));
}

bool Reco::PlaneSurface::isSensitive() const
{
    return false;
}

double Reco::PlaneSurface::pathlength(const Alg::Point3D& pos, const Alg::Vector3D& dir) const
{
    Alg::Point3D loc3D  = transform().Inverse()*pos;
    Alg::Vector3D locdir= transform().Inverse()*dir;
 //o   std::cout << "------------------------------------------------------------" << std::endl;
 //o   std::cout << "pos: " << pos << " dir " << dir << std::endl;
    double s1 = 0.;
    double s2 = 0.;
    
    Alg::Point3D P1(0.,0.,0.);
    Alg::Point3D P2(0.,0.,0.);
    Alg::Point3D A (0.,0.,0.);
    Alg::Point3D P (0.,0.,0.);
    
    //cut with xy-planes
    s1 = (halfThickness() - loc3D.Z())/locdir.Z();
    s2 = (-halfThickness() - loc3D.Z())/locdir.Z();
    
    P1 = loc3D+s1*locdir;
    P2 = loc3D+s2*locdir;
    P  = P2-P1;
//o    std::cout << "P1: " << P1 << " P2: " << P2 << " P: " << P << std::endl;
    
    if (((fabs(P1.X()) <= m_halfX) && (fabs(P1.Y()) <= m_halfY)) && (fabs(P2.X()) <= m_halfX) && (fabs(P2.Y()) <= m_halfY)) {
//o        std::cout << "P1 & P2 in XY" << std::endl;
//o        std::cout << "P.R: " << fabs(P.R()) << " sqrt: " << fabs(P.R()) << std::endl;
        return (fabs(P.R()));
    }
    else if (((fabs(P1.X()) <= m_halfX) && (fabs(P1.Y()) <= m_halfY)))  {
//o        std::cout << "P1 in XY" << std::endl;
        A = P1;
        //cut with yz-planes
        s1 = (m_halfX - loc3D.X())/locdir.X();
        s2 = (-m_halfX - loc3D.X())/locdir.X();
        
        P1 = loc3D+s1*locdir;
        P2 = loc3D+s2*locdir;
        
        if (((fabs(P1.Y()) <= m_halfY) && (fabs(P1.Z()) <=halfThickness()))) {
//o            std::cout << "P2 in YZ1" << std::endl;
            P = A-P1;
            return (fabs(P.R()));
        }
        
        else if (((fabs(P2.Y()) <= m_halfY) && (fabs(P2.Z()) <=halfThickness()))) {
//o            std::cout << "P2 in YZ2" << std::endl;
            P = A-P2;
            return (fabs(P.R()));
        }
        
        else {
            //cut with xz-planes
            s1 = (m_halfY - loc3D.Y())/locdir.Y();
            s2 = (-m_halfY - loc3D.Y())/locdir.Y();
        
            P1 = loc3D+s1*locdir;
            P2 = loc3D+s2*locdir;
        
            if (((fabs(P1.X()) <= m_halfX) && (fabs(P1.Z()) <=halfThickness()))){
//o                std::cout << "P2 in XZ1" << std::endl;
                P = A-P1;
                return (fabs(P.R()));
            }
        
            else if (((fabs(P2.X()) <= m_halfX) && (fabs(P2.Z()) <=halfThickness()))){
//o                std::cout << "P2 in XZ2" << std::endl;
                P = A-P2;
                return (fabs(P.R()));
            }
            else return 0.;
        }
        
    }
    else if (((fabs(P2.X()) <= m_halfX) && (fabs(P2.Y()) <= m_halfY)))  {
//o        std::cout << "P2 in XY" << std::endl;
        A = P2;
        //cut with yz-planes
        s1 = (m_halfX - loc3D.X())/locdir.X();
        s2 = (-m_halfX - loc3D.X())/locdir.X();
        
        P1 = loc3D+s1*locdir;
        P2 = loc3D+s2*locdir;
        
        if (((fabs(P1.Y()) <= m_halfY) && (fabs(P1.Z()) <=halfThickness()))) {
//o        std::cout << "P1 in YZ1" << std::endl;
            P = A-P1;
            return (fabs(P.R()));
        }
        
        else if (((fabs(P2.Y()) <= m_halfY) && (fabs(P2.Z()) <=halfThickness()))) {
//o        std::cout << "P1 in YZ2" << std::endl;
            P = A-P2;
            return (fabs(P.R()));
        }
        
        else {
            //cut with xz-planes
            s1 = (m_halfY - loc3D.Y())/locdir.Y();
            s2 = (-m_halfY - loc3D.Y())/locdir.Y();
        
            P1 = loc3D+s1*locdir;
            P2 = loc3D+s2*locdir;
        
            if (((fabs(P1.X()) <= m_halfX) && (fabs(P1.Z()) <=halfThickness()))){
//o                std::cout << "P1 in XZ1" << std::endl;
                P = A-P1;
                return (fabs(P.R()));
            }
        
            else if (((fabs(P2.X()) <= m_halfX) && (fabs(P2.Z()) <=halfThickness()))){
//o                std::cout << "P1 in XZ2" << std::endl;
                P = A-P2;
                return (fabs(P.R()));
            }
            else return 0.;
        }
    }
    else {
        
        //cut with yz-planes
        s1 = (m_halfX - loc3D.X())/locdir.X();
        s2 = (-m_halfX - loc3D.X())/locdir.X();
        
        P1 = loc3D+s1*locdir;
        P2 = loc3D+s2*locdir;
        P  = P2-P1;
        
        if (((fabs(P1.Y()) <= m_halfY) && (fabs(P1.Z()) <=halfThickness())) && ((fabs(P2.Y()) <= m_halfY) && (fabs(P2.Z()) <=halfThickness()))){
//o        std::cout << "P1 & P2 in YZ" << std::endl;
            return (fabs(P.R()));
        }
        else if (((fabs(P1.Y()) <= m_halfY) && (fabs(P1.Z()) <=halfThickness()))) {
//o        std::cout << "P1 in YZ" << std::endl;
            A = P1;
            //cut with xz-planes
            s1 = (m_halfY - loc3D.Y())/locdir.Y();
            s2 = (-m_halfY - loc3D.Y())/locdir.Y();
            
            P1 = loc3D+s1*locdir;
            P2 = loc3D+s2*locdir;
            
            if (((fabs(P1.X()) <= m_halfX) && (fabs(P1.Z()) <=halfThickness()))){
//o                std::cout << "P2 in YZ1" << std::endl;
                P = A-P1;
                return (fabs(P.R()));
            }
            
            else if (((fabs(P2.X()) <= m_halfX) && (fabs(P2.Z()) <=halfThickness()))){
//o                std::cout << "P2 in YZ2" << std::endl;
                P = A-P2;
                return (fabs(P.R()));
            }
            
            else return 0.;
        }
        else if (((fabs(P2.Y()) <= m_halfY) && (fabs(P2.Z()) <=halfThickness()))) {
//o            std::cout << "P1 in YZ" << std::endl;
            A = P2;
            //cut with xz-planes
            s1 = (m_halfY - loc3D.Y())/locdir.Y();
            s2 = (-m_halfY - loc3D.Y())/locdir.Y();
            
            P1 = loc3D+s1*locdir;
            P2 = loc3D+s2*locdir;
            
            if (((fabs(P1.X()) <= m_halfX) && (fabs(P1.Z()) <=halfThickness()))){
//o                std::cout << "P2 in XZ1" << std::endl;
                P = A-P1;
                return (fabs(P.R()));
            }
            
            else if (((fabs(P2.X()) <= m_halfX) && (fabs(P2.Z()) <=halfThickness()))){
//o                std::cout << "P1 in XZ2" << std::endl;
                P = A-P2;
                return (fabs(P.R()));
            }
            
            else return 0.;
        }
        
        else {
            //cut with xz-planes
            s1 = (m_halfY - loc3D.Y())/locdir.Y();
            s2 = (-m_halfY - loc3D.Y())/locdir.Y();
            
            P1 = loc3D+s1*locdir;
            P2 = loc3D+s2*locdir;
            P  = P2-P1;
            
            if (((fabs(P1.X()) <= m_halfX) && (fabs(P1.Z()) <=halfThickness())) && ((fabs(P2.X()) <= m_halfX) && (fabs(P2.Z()) <=halfThickness())))
            {
//o                std::cout << "P1 & P2 in XZ" << std::endl;
                return (fabs(P.R()));
            }
            
            else return 0.;
                
            }
    }
    
    return 0.;
}






