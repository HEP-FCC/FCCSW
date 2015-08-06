//
//  BoundarySurface.h
//  
//
//  Created by Julia Hrdinka on 26/01/15.
//
//

#include "RecoGeometry/BoundarySurface.h"

Reco::BoundarySurface::BoundarySurface() :
m_nextVolume(0),
m_previousVolume(0),
m_nextVolumes(0),
m_previousVolumes(0),
m_material(0)
{}

Reco::BoundarySurface::BoundarySurface(const BoundarySurface& boundarysurface) :
m_nextVolume(0),
m_previousVolume(0),
m_nextVolumes(0),
m_previousVolumes(0),
m_material(0)
{
    m_nextVolume     = boundarysurface.m_nextVolume;
    m_previousVolume = boundarysurface.m_previousVolume;
    m_nextVolumes    = boundarysurface.getNextVolumes();
    m_previousVolumes= boundarysurface.getPreviousVolumes();
    m_material       = boundarysurface.getMaterial();
}

Reco::BoundarySurface::~BoundarySurface()
{
    m_nextVolumes = 0;
    m_previousVolumes =0;
    m_material = 0;
}

Reco::BoundarySurface& Reco::BoundarySurface::operator=(const BoundarySurface& boundarysurface)
{
    if(this!=&boundarysurface)
    {
        m_nextVolume = boundarysurface.m_nextVolume;
        m_previousVolume = boundarysurface.m_previousVolume;
        delete m_nextVolumes;
        m_nextVolumes    = new VolumeArray(*boundarysurface.m_nextVolumes);
        delete m_previousVolumes;
        m_previousVolumes= new VolumeArray(*boundarysurface.m_previousVolumes);
        delete m_material;
        m_material       = new Material(*boundarysurface.getMaterial());
    }
    return (*this);
}

const Reco::Volume* Reco::BoundarySurface::nextVolume(const Alg::Point3D& glopos) const
{
    Alg::Point2D locpos;
    Alg::Vector3D dir;
    if (this->surfaceRepresentation()->globalToLocal(glopos,dir,locpos)) {
        if (m_nextVolumes) {
            return (m_nextVolumes->object(glopos));
        }
        else {
            return (m_nextVolume.get());
        }
    }
    return 0;
}

void Reco::BoundarySurface::setNextVolume(std::shared_ptr<const Reco::Volume> nextVolume) const
{
    m_nextVolume = nextVolume;
}

void Reco::BoundarySurface::setPreviousVolume(std::shared_ptr<const Reco::Volume> previousVolume) const
{
    m_previousVolume = previousVolume;
}

void Reco::BoundarySurface::setNextVolumes(const VolumeArray* nextVolumes) const
{
    m_nextVolumes = nextVolumes;
}

void Reco::BoundarySurface::setPreviousVolumes(const VolumeArray* previousVolumes) const
{
    m_previousVolumes = previousVolumes;
}

const Reco::Volume* Reco::BoundarySurface::getNextVolume() const
{
    return (m_nextVolume.get());
}

const Reco::Volume* Reco::BoundarySurface::getPreviousVolume() const
{
    return (m_previousVolume.get());
}

const Reco::BoundarySurface::VolumeArray* Reco::BoundarySurface::getNextVolumes() const
{
    return (m_nextVolumes);
}

const Reco::BoundarySurface::VolumeArray* Reco::BoundarySurface::getPreviousVolumes() const
{
    return (m_previousVolumes);
}

void Reco::BoundarySurface::setMaterial(Reco::Material* material)
{
    m_material = material;
}

Reco::Material* Reco::BoundarySurface::getMaterial() const
{
    return (m_material);
}







