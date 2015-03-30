//
//  CylinderVolume.cxx
//  
//
//  Created by Julia Hrdinka on 09/01/15.
//
//

#include "RecoGeometry/CylinderVolume.h"
#include "RecoGeometry/BoundaryCylinderSurface.h"
#include "RecoGeometry/BoundaryDiscSurface.h"

Reco::CylinderVolume::CylinderVolume(std::shared_ptr<const Alg::Transform3D> transf, LayerArray* layers, double Rmin, double Rmax, double halfZ) :
Reco::Volume(transf, layers),
m_Rmin(Rmin),
m_Rmax(Rmax),
m_halfZ(halfZ)
{
    //Discs
    Alg::Vector3D lx(0.,0.,0.);
    Alg::Vector3D ly(0.,0.,0.);
    Alg::Vector3D lz(0.,0.,0.);
    Alg::RotationMatrix3D rotation      = transf->Rotation();
    rotation.GetComponents<Alg::Vector3D>(lx,ly,lz);
    Alg::Vector3D         center        = transf->Translation().Vect();
    std::shared_ptr<const Alg::Transform3D> rightTransform(new Alg::Transform3D(rotation,center+lz*halfZ));
    Alg::RotationMatrix3D leftrotation  = rotation*Alg::RotationY(M_PI)*Alg::RotationZ(-M_PI);
    leftrotation.GetComponents<Alg::Vector3D>(lx,ly,lz);
    std::shared_ptr<const Alg::Transform3D> leftTransform(new Alg::Transform3D(leftrotation,center+lz*halfZ));
    
    std::shared_ptr<Reco::BoundaryDiscSurface> posDisc (new Reco::BoundaryDiscSurface(rightTransform,Rmin,Rmax));
    std::shared_ptr<Reco::BoundaryDiscSurface> negDisc (new Reco::BoundaryDiscSurface(leftTransform,Rmin,Rmax));
    
    //outer Cylinder
    std::shared_ptr<Reco::BoundaryCylinderSurface> outerCylinder (new Reco::BoundaryCylinderSurface(std::shared_ptr<const Alg::Transform3D>(transf),Rmax,halfZ));
    
    m_boundarySurfaces.push_back(negDisc);
    m_boundarySurfaces.push_back(posDisc);
    m_boundarySurfaces.push_back(outerCylinder);
    //inner Cylinder
    if (Rmin>0.) {
        std::shared_ptr<Reco::BoundaryCylinderSurface> innerCylinder (new Reco::BoundaryCylinderSurface(std::shared_ptr<const Alg::Transform3D>(transf),Rmin,halfZ));
        m_boundarySurfaces.push_back(innerCylinder);
    }
    //set Coordinates
    m_coordinates[Reco::CylinderVolume::Rmin]   = m_Rmin;
    m_coordinates[Reco::CylinderVolume::Rmax]   = m_Rmax;
    m_coordinates[Reco::CylinderVolume::halfZ]  = m_halfZ;
    //set Type
    m_volumeType = Reco::Volume::CylinderVolume;
    
}

Reco::CylinderVolume::CylinderVolume(std::shared_ptr<const Alg::Transform3D> transf, double Rmin, double Rmax, double halfZ) :
Reco::Volume(transf),
m_Rmin(Rmin),
m_Rmax(Rmax),
m_halfZ(halfZ)
{
    //Discs
    Alg::Vector3D lx(0.,0.,0.);
    Alg::Vector3D ly(0.,0.,0.);
    Alg::Vector3D lz(0.,0.,0.);
    Alg::RotationMatrix3D rotation      = transf->Rotation();
    rotation.GetComponents<Alg::Vector3D>(lx,ly,lz);
    Alg::Vector3D         center        = transf->Translation().Vect();
    std::shared_ptr<const Alg::Transform3D> rightTransform(new Alg::Transform3D(rotation,center+lz*halfZ));
    Alg::RotationMatrix3D leftrotation  = rotation*Alg::RotationY(M_PI)*Alg::RotationZ(-M_PI);
    leftrotation.GetComponents<Alg::Vector3D>(lx,ly,lz);
    std::shared_ptr<const Alg::Transform3D> leftTransform(new Alg::Transform3D(leftrotation,center+lz*halfZ));
    
    std::shared_ptr<Reco::BoundaryDiscSurface> posDisc (new Reco::BoundaryDiscSurface(rightTransform,Rmin,Rmax));
    std::shared_ptr<Reco::BoundaryDiscSurface> negDisc (new Reco::BoundaryDiscSurface(leftTransform,Rmin,Rmax));
    //outer Cylinder
    std::shared_ptr<Reco::BoundaryCylinderSurface> outerCylinder (new Reco::BoundaryCylinderSurface(std::shared_ptr<const Alg::Transform3D>(transf),Rmax,halfZ));
    
    m_boundarySurfaces.push_back(negDisc);
    m_boundarySurfaces.push_back(posDisc);
    m_boundarySurfaces.push_back(outerCylinder);
    
    //inner Cylinder
    if (Rmin>0.) {
        std::shared_ptr<Reco::BoundaryCylinderSurface> innerCylinder (new Reco::BoundaryCylinderSurface(std::shared_ptr<const Alg::Transform3D>(transf),Rmin,halfZ));
        m_boundarySurfaces.push_back(innerCylinder);
    }
    //set Coordinates
    m_coordinates[Reco::CylinderVolume::Rmin]   = m_Rmin;
    m_coordinates[Reco::CylinderVolume::Rmax]   = m_Rmax;
    m_coordinates[Reco::CylinderVolume::halfZ]  = m_halfZ;
    //set Type
    m_volumeType = Reco::Volume::CylinderVolume;
}

Reco::CylinderVolume::CylinderVolume(LayerArray* layers, TGeoNode* node, TGeoConeSeg* tube) :
Reco::Volume(layers,node)
{
    m_Rmin = tube->GetRmin1();
    m_Rmax = tube->GetRmax1();
    m_halfZ= tube->GetDZ();
    
    //Discs
    Alg::Vector3D lx(0.,0.,0.);
    Alg::Vector3D ly(0.,0.,0.);
    Alg::Vector3D lz(0.,0.,0.);
    Alg::RotationMatrix3D rotation      = transform().Rotation();
    rotation.GetComponents<Alg::Vector3D>(lx,ly,lz);
    Alg::Vector3D         center        = transform().Translation().Vect();
    std::shared_ptr<const Alg::Transform3D> rightTransform(new Alg::Transform3D(rotation,center+lz*m_halfZ));
    Alg::RotationMatrix3D leftrotation  = rotation*Alg::RotationY(M_PI)*Alg::RotationZ(-M_PI);
    leftrotation.GetComponents<Alg::Vector3D>(lx,ly,lz);
    std::shared_ptr<const Alg::Transform3D> leftTransform(new Alg::Transform3D(leftrotation,center+lz*m_halfZ));
    std::shared_ptr<Reco::BoundaryDiscSurface> posDisc (new Reco::BoundaryDiscSurface(rightTransform,m_Rmin,m_Rmax));
    std::shared_ptr<Reco::BoundaryDiscSurface> negDisc (new Reco::BoundaryDiscSurface(leftTransform,m_Rmin,m_Rmax));
    //outer Cylinder
    std::shared_ptr<Reco::BoundaryCylinderSurface> outerCylinder (new Reco::BoundaryCylinderSurface(std::make_shared<const Alg::Transform3D>(Alg::Transform3D(transform())),m_Rmax,m_halfZ));
    
    m_boundarySurfaces.push_back(negDisc);
    m_boundarySurfaces.push_back(posDisc);
    m_boundarySurfaces.push_back(outerCylinder);
    //inner Cylinder
    if (m_Rmin>0.) {
        std::shared_ptr<Reco::BoundaryCylinderSurface> innerCylinder (new Reco::BoundaryCylinderSurface(std::make_shared<const Alg::Transform3D>(Alg::Transform3D(transform())),m_Rmin,m_halfZ));
        m_boundarySurfaces.push_back(innerCylinder);
    }
    //setCoordinates
    m_coordinates[Reco::CylinderVolume::Rmin]   = m_Rmin;
    m_coordinates[Reco::CylinderVolume::Rmax]   = m_Rmax;
    m_coordinates[Reco::CylinderVolume::halfZ]  = m_halfZ;
    //set Type
    m_volumeType = Reco::Volume::CylinderVolume;
}

Reco::CylinderVolume::CylinderVolume(TGeoNode* node, TGeoConeSeg* tube) :
Reco::Volume(node)
{
    m_Rmin = tube->GetRmin1();
    m_Rmax = tube->GetRmax1();
    m_halfZ= tube->GetDZ();
    
    //Discs
    Alg::Vector3D lx(0.,0.,0.);
    Alg::Vector3D ly(0.,0.,0.);
    Alg::Vector3D lz(0.,0.,0.);
    Alg::RotationMatrix3D rotation      = transform().Rotation();
    rotation.GetComponents<Alg::Vector3D>(lx,ly,lz);
    Alg::Vector3D         center        = transform().Translation().Vect();
    std::shared_ptr<const Alg::Transform3D> rightTransform(new Alg::Transform3D(rotation,center+lz*m_halfZ));
    Alg::RotationMatrix3D leftrotation  = rotation*Alg::RotationY(M_PI)*Alg::RotationZ(-M_PI);
    leftrotation.GetComponents<Alg::Vector3D>(lx,ly,lz);
    std::shared_ptr<const Alg::Transform3D> leftTransform (new Alg::Transform3D(leftrotation,center+lz*m_halfZ));
    
    std::shared_ptr<Reco::BoundaryDiscSurface> posDisc (new Reco::BoundaryDiscSurface(rightTransform,m_Rmin,m_Rmax));
    std::shared_ptr<Reco::BoundaryDiscSurface> negDisc (new Reco::BoundaryDiscSurface(leftTransform,m_Rmin,m_Rmax));

    //outer Cylinder
    std::shared_ptr<Reco::BoundaryCylinderSurface> outerCylinder (new Reco::BoundaryCylinderSurface(std::make_shared<const Alg::Transform3D>(Alg::Transform3D(transform())),m_Rmax,m_halfZ));
    
    m_boundarySurfaces.push_back(negDisc);
    m_boundarySurfaces.push_back(posDisc);
    m_boundarySurfaces.push_back(outerCylinder);
    
    //inner Cylinder
    if (m_Rmin>0.) {
        std::shared_ptr<Reco::BoundaryCylinderSurface> innerCylinder (new Reco::BoundaryCylinderSurface(std::make_shared<const Alg::Transform3D>(Alg::Transform3D(transform())),m_Rmin,m_halfZ));
        m_boundarySurfaces.push_back(innerCylinder);
    }
    //setCoordinates
    m_coordinates[Reco::CylinderVolume::Rmin]   = m_Rmin;
    m_coordinates[Reco::CylinderVolume::Rmax]   = m_Rmax;
    m_coordinates[Reco::CylinderVolume::halfZ]  = m_halfZ;
    //set Type
    m_volumeType = Reco::Volume::CylinderVolume;
}

Reco::CylinderVolume::CylinderVolume(const Reco::CylinderVolume& cylindervolume) :
Reco::Volume(cylindervolume),
m_Rmin(cylindervolume.m_Rmin),
m_Rmax(cylindervolume.m_Rmax),
m_halfZ(cylindervolume.m_halfZ)
{
    //set Type
    m_volumeType = Reco::Volume::CylinderVolume;
}

Reco::CylinderVolume::~CylinderVolume()
{}

Reco::CylinderVolume* Reco::CylinderVolume::clone() const
{
    return (new Reco::CylinderVolume(*this));
}

Reco::CylinderVolume& Reco::CylinderVolume::operator=(const Reco::CylinderVolume& cylindervolume)
{
    if (this!=&cylindervolume) {
        Reco::Volume::operator= (cylindervolume);
        m_Rmin = cylindervolume.m_Rmin;
        m_Rmax = cylindervolume.m_Rmax;
        m_halfZ= cylindervolume.m_halfZ;
    }
    return (*this);
}

double Reco::CylinderVolume::getRmin() const
{
    return m_Rmin;
}

double Reco::CylinderVolume::getRmax() const
{
    return m_Rmax;
}

double Reco::CylinderVolume::getHalfZ() const
{
    return m_halfZ;
}

const Reco::BoundarySurface* Reco::CylinderVolume::getPosDisc() const
{
    return (m_boundarySurfaces[Reco::CylinderVolume::posDisc].get());
}

const Reco::BoundarySurface* Reco::CylinderVolume::getNegDisc() const
{
    return (m_boundarySurfaces[Reco::CylinderVolume::negDisc].get());
}

const Reco::BoundarySurface* Reco::CylinderVolume::getOuterCylinder() const
{
    return (m_boundarySurfaces[Reco::CylinderVolume::outerCylinder].get());
}

const Reco::BoundarySurface* Reco::CylinderVolume::getInnerCylinder() const
{
    return (m_boundarySurfaces[Reco::CylinderVolume::innerCylinder].get());
}

bool Reco::CylinderVolume::setBoundarySurface(size_t n, std::shared_ptr<const BoundarySurface> boundarysurface) const
{
    return (Reco::Volume::setBoundarySurface(n,boundarysurface));
}

bool Reco::CylinderVolume::isInside(const Alg::Point3D& glopos, double tol) const
{
    Alg::Point3D locpos = Reco::Volume::m_transform ? (transform().Inverse()*glopos) : glopos;
    double r    = sqrt(locpos.X()*locpos.X()+locpos.Y()*locpos.Y());
    return ((r>=(m_Rmin-tol)) && (r<=(m_Rmax+tol)) && (fabs(locpos.Z())<=(m_halfZ+tol)));
}















