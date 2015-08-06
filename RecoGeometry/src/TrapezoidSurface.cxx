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
m_halfY(0.),
m_halfThickness(0.)
{}

Reco::TrapezoidSurface::TrapezoidSurface(std::shared_ptr<const Alg::Transform3D> transf, double halfX1, double halfX2, double halfY, double halfThickness) :
Reco::Surface(transf),
m_halfX1(halfX1),
m_halfX2(halfX2),
m_halfY(halfY),
m_halfThickness(halfThickness)
{}

Reco::TrapezoidSurface::TrapezoidSurface(TGeoNode* node, TGeoTrd2* trapezoid) :
Surface(node)
{
    Alg::Transform3D newtransf(transform()*Alg::RotationX(0.5*M_PI));
    Alg::Vector3D lx(0.,0.,0.);
    Alg::Vector3D ly(0.,0.,0.);
    Alg::Vector3D lz(0.,0.,0.);
    Alg::RotationMatrix3D rotation = newtransf.Rotation();
    rotation.GetComponents<Alg::Vector3D>(lx,ly,lz);
    rotation.SetComponents<Alg::Vector3D>(lx,ly,-lz);
    std::shared_ptr<const Alg::Transform3D> transform(new Alg::Transform3D(rotation,newtransf.Translation()));
    this->setTransform(transform);
    //convention: y'=Z, thicknesss = Y(X,Z)
    m_halfX1    = trapezoid->GetDx1();
    m_halfX2    = trapezoid->GetDx2();
    m_halfY     = trapezoid->GetDz();
    m_halfThickness = 0.5*(trapezoid->GetDy1()+trapezoid->GetDy2());
}

Reco::TrapezoidSurface::TrapezoidSurface(TGeoTrd2* trapezoid, std::shared_ptr<const Alg::Transform3D> transf) :
Reco::Surface(transf)
{
    Alg::Transform3D newtransf(*transf*Alg::RotationX(0.5*M_PI));
    Alg::Vector3D lx(0.,0.,0.);
    Alg::Vector3D ly(0.,0.,0.);
    Alg::Vector3D lz(0.,0.,0.);
    Alg::RotationMatrix3D rotation = newtransf.Rotation();
    rotation.GetComponents<Alg::Vector3D>(lx,ly,lz);
    rotation.SetComponents<Alg::Vector3D>(lx,ly,-lz);
    std::shared_ptr<const Alg::Transform3D> transform(new Alg::Transform3D(rotation,newtransf.Translation()));
    this->setTransform(transform);
    //convention: y'=Z, thicknesss = Y(X,Z)
    m_halfX1    = trapezoid->GetDx1();
    m_halfX2    = trapezoid->GetDx2();
    m_halfY     = trapezoid->GetDz();
    m_halfThickness = 0.5*(trapezoid->GetDy1()+trapezoid->GetDy2());
}

Reco::TrapezoidSurface::TrapezoidSurface(TGeoNode* node, TGeoTrd2* trapezoid, Reco::MaterialMap* materialmap) :
Reco::Surface(node, materialmap)
{
    Alg::Transform3D newtransf(transform()*Alg::RotationX(0.5*M_PI));
    Alg::Vector3D lx(0.,0.,0.);
    Alg::Vector3D ly(0.,0.,0.);
    Alg::Vector3D lz(0.,0.,0.);
    Alg::RotationMatrix3D rotation = newtransf.Rotation();
    rotation.GetComponents<Alg::Vector3D>(lx,ly,lz);
    rotation.SetComponents<Alg::Vector3D>(lx,ly,-lz);
    std::shared_ptr<const Alg::Transform3D> transform(new Alg::Transform3D(rotation,newtransf.Translation()));
    this->setTransform(transform);

    m_halfX1    = trapezoid->GetDx1();
    m_halfX2    = trapezoid->GetDx2();
    m_halfY     = trapezoid->GetDz();
    m_halfThickness = 0.5*(trapezoid->GetDy1()+trapezoid->GetDy2());
}

Reco::TrapezoidSurface::TrapezoidSurface(TGeoTrd2* trapezoid, Reco::MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf) :
Reco::Surface(materialmap,transf)
{
//    std::cout << "before: " << *transf << std::endl;
    
    Alg::Transform3D newtransf(*transf*Alg::RotationX(0.5*M_PI));
 //   std::cout << "rotation: " << newtransf << std::endl;
    Alg::Vector3D lx(0.,0.,0.);
    Alg::Vector3D ly(0.,0.,0.);
    Alg::Vector3D lz(0.,0.,0.);
    Alg::RotationMatrix3D rotation = newtransf.Rotation();
//    std::cout << "components: " << "lx " << lx << " ly " << ly << " lz " << lz << std::endl;
    rotation.GetComponents<Alg::Vector3D>(lx,ly,lz);
//    std::cout << "components: " << "lx " << lx << " ly " << ly << " lz " << lz << std::endl;
 //   std::cout << "rotation before: " << rotation << std::endl;
    rotation.SetComponents<Alg::Vector3D>(lx,ly,-lz);
 //   std::cout << "rotation after: " << rotation << std::endl;
    std::shared_ptr<const Alg::Transform3D> transform(new Alg::Transform3D(rotation,newtransf.Translation()));
 //   std::cout << "after: " << *transform << std::endl;
    //convention: y'=Z, thicknesss = Y(X,Z)
    this->setTransform(transform);
    m_halfX1    = trapezoid->GetDx1();
    m_halfX2    = trapezoid->GetDx2();
    m_halfY     = trapezoid->GetDz();
    m_halfThickness = 0.5*(trapezoid->GetDy1()+trapezoid->GetDy2());
  //  std::cout << "X1: " << m_halfX1 << " X2: " << m_halfX2 << " Y: " << m_halfY << " thickness: " << m_halfThickness << std::endl;
}

Reco::TrapezoidSurface::TrapezoidSurface(const TrapezoidSurface& trapezoidsurface) :
Reco::Surface(trapezoidsurface),
m_halfX1(trapezoidsurface.m_halfX1),
m_halfX2(trapezoidsurface.m_halfX2),
m_halfY(trapezoidsurface.m_halfY),
m_halfThickness(trapezoidsurface.m_halfThickness)
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
        m_halfX1    = trapezoidsurface.m_halfX1;
        m_halfX2    = trapezoidsurface.m_halfX2;
        m_halfY     = trapezoidsurface.m_halfY;
        m_halfThickness = trapezoidsurface.m_halfThickness;
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

double Reco::TrapezoidSurface::halfThickness() const
{
    return (m_halfThickness);
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
    if (materialmap()) return materialmap()->material(locpos);
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

    double k = (2.*m_halfY)/(m_halfX2-m_halfX1);
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
    return(isInside(locpos,s_onSurfaceTolerance,s_onSurfaceTolerance) && (loc3D.Z()*loc3D.Z())<=(halfThickness()*halfThickness()));//(s_onSurfaceTolerance*s_onSurfaceTolerance));
}

bool Reco::TrapezoidSurface::isSensitive() const
{
    return false;
}

double Reco::TrapezoidSurface::pathlength(const Alg::Point3D&, const Alg::Vector3D& dir) const
{
    Alg::Vector3D direction = transform()*dir;
    return (Reco::Surface::pathlength(direction));
    //o   std::cout << "------------------------------------------------------------" << std::endl;
    //o   std::cout << "pos: " << pos << " dir " << dir << std::endl;
/*    double s1 = 0.;
    double s2 = 0.;
    
    Alg::Point3D P1(0.,0.,0.);
    Alg::Point3D P2(0.,0.,0.);
    Alg::Point3D A (0.,0.,0.);
    Alg::Point3D P (0.,0.,0.);
    
    //cut with xy-planes
    s1 = (thickness() - pos.Z())/dir.Z();
    s2 = (-thickness() - pos.Z())/dir.Z();
    
    P1 = pos+s1*dir;
    P2 = pos+s2*dir;
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
        s1 = (m_halfX - pos.X())/dir.X();
        s2 = (-m_halfX - pos.X())/dir.X();
        
        P1 = pos+s1*dir;
        P2 = pos+s2*dir;
        
        if (((fabs(P1.Y()) <= m_halfY) && (fabs(P1.Z()) <= thickness()))) {
            //o            std::cout << "P2 in YZ1" << std::endl;
            P = A-P1;
            return (fabs(P.R()));
        }
        
        else if (((fabs(P2.Y()) <= m_halfY) && (fabs(P2.Z()) <= thickness()))) {
            //o            std::cout << "P2 in YZ2" << std::endl;
            P = A-P2;
            return (fabs(P.R()));
        }
        
        else {
            //cut with xz-planes
            s1 = (m_halfY - pos.Y())/dir.Y();
            s2 = (-m_halfY - pos.Y())/dir.Y();
            
            P1 = pos+s1*dir;
            P2 = pos+s2*dir;
            
            if (((fabs(P1.X()) <= m_halfX) && (fabs(P1.Z()) <= thickness()))){
                //o                std::cout << "P2 in XZ1" << std::endl;
                P = A-P1;
                return (fabs(P.R()));
            }
            
            else if (((fabs(P2.X()) <= m_halfX) && (fabs(P2.Z()) <= thickness()))){
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
        s1 = (m_halfX - pos.X())/dir.X();
        s2 = (-m_halfX - pos.X())/dir.X();
        
        P1 = pos+s1*dir;
        P2 = pos+s2*dir;
        
        if (((fabs(P1.Y()) <= m_halfY) && (fabs(P1.Z()) <= thickness()))) {
            //o        std::cout << "P1 in YZ1" << std::endl;
            P = A-P1;
            return (fabs(P.R()));
        }
        
        else if (((fabs(P2.Y()) <= m_halfY) && (fabs(P2.Z()) <= thickness()))) {
            //o        std::cout << "P1 in YZ2" << std::endl;
            P = A-P2;
            return (fabs(P.R()));
        }
        
        else {
            //cut with xz-planes
            s1 = (m_halfY - pos.Y())/dir.Y();
            s2 = (-m_halfY - pos.Y())/dir.Y();
            
            P1 = pos+s1*dir;
            P2 = pos+s2*dir;
            
            if (((fabs(P1.X()) <= m_halfX) && (fabs(P1.Z()) <= thickness()))){
                //o                std::cout << "P1 in XZ1" << std::endl;
                P = A-P1;
                return (fabs(P.R()));
            }
            
            else if (((fabs(P2.X()) <= m_halfX) && (fabs(P2.Z()) <= thickness()))){
                //o                std::cout << "P1 in XZ2" << std::endl;
                P = A-P2;
                return (fabs(P.R()));
            }
            else return 0.;
        }
    }
    else {
        
        //cut with yz-planes
        s1 = (m_halfX - pos.X())/dir.X();
        s2 = (-m_halfX - pos.X())/dir.X();
        
        P1 = pos+s1*dir;
        P2 = pos+s2*dir;
        P  = P2-P1;
        
        if (((fabs(P1.Y()) <= m_halfY) && (fabs(P1.Z()) <= thickness())) && ((fabs(P2.Y()) <= m_halfY) && (fabs(P2.Z()) <= thickness()))){
            //o        std::cout << "P1 & P2 in YZ" << std::endl;
            return (fabs(P.R()));
        }
        else if (((fabs(P1.Y()) <= m_halfY) && (fabs(P1.Z()) <= thickness()))) {
            //o        std::cout << "P1 in YZ" << std::endl;
            A = P1;
            //cut with xz-planes
            s1 = (m_halfY - pos.Y())/dir.Y();
            s2 = (-m_halfY - pos.Y())/dir.Y();
            
            P1 = pos+s1*dir;
            P2 = pos+s2*dir;
            
            if (((fabs(P1.X()) <= m_halfX) && (fabs(P1.Z()) <= thickness()))){
                //o                std::cout << "P2 in YZ1" << std::endl;
                P = A-P1;
                return (fabs(P.R()));
            }
            
            else if (((fabs(P2.X()) <= m_halfX) && (fabs(P2.Z()) <= thickness()))){
                //o                std::cout << "P2 in YZ2" << std::endl;
                P = A-P2;
                return (fabs(P.R()));
            }
            
            else return 0.;
        }
        else if (((fabs(P2.Y()) <= m_halfY) && (fabs(P2.Z()) <= thickness()))) {
            //o            std::cout << "P1 in YZ" << std::endl;
            A = P2;
            //cut with xz-planes
            s1 = (m_halfY - pos.Y())/dir.Y();
            s2 = (-m_halfY - pos.Y())/dir.Y();
            
            P1 = pos+s1*dir;
            P2 = pos+s2*dir;
            
            if (((fabs(P1.X()) <= m_halfX) && (fabs(P1.Z()) <= thickness()))){
                //o                std::cout << "P2 in XZ1" << std::endl;
                P = A-P1;
                return (fabs(P.R()));
            }
            
            else if (((fabs(P2.X()) <= m_halfX) && (fabs(P2.Z()) <= thickness()))){
                //o                std::cout << "P1 in XZ2" << std::endl;
                P = A-P2;
                return (fabs(P.R()));
            }
            
            else return 0.;
        }
        
        else {
            //cut with xz-planes
            s1 = (m_halfY - pos.Y())/dir.Y();
            s2 = (-m_halfY - pos.Y())/dir.Y();
            
            P1 = pos+s1*dir;
            P2 = pos+s2*dir;
            P  = P2-P1;
            
            if (((fabs(P1.X()) <= m_halfX) && (fabs(P1.Z()) <= thickness())) && ((fabs(P2.X()) <= m_halfX) && (fabs(P2.Z()) <= thickness())))
            {
                //o                std::cout << "P1 & P2 in XZ" << std::endl;
                return (fabs(P.R()));
            }
            
            else return 0.;
            
        }
    }
    
    return 0.;*/
}




