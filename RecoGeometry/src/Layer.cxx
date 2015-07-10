//
//  Layer.cxx
//  
//
//  Created by Julia Hrdinka on 27/11/14.
//
//

#include "RecoGeometry/Layer.h"


Reco::Layer::Layer() :
m_surfaces(0),
m_nextLayer(0),
m_previousLayer(0)
{}

Reco::Layer::Layer(const Reco::Layer& layer) :
m_surfaces(layer.m_surfaces),
m_nextLayer(layer.m_nextLayer),
m_previousLayer(layer.m_previousLayer)
{}

Reco::Layer::Layer(Reco::SurfaceArray* sf) :
m_surfaces(sf),
m_nextLayer(0),
m_previousLayer(0)
{}

Reco::Layer::~Layer()
{
    delete m_surfaces;
}

const Reco::SurfaceVector Reco::Layer::getModules(const Alg::Point3D& glopos) const
{
    if (m_surfaces && this->onLayer(glopos)) {
        return (*(m_surfaces->object(glopos)));
 //       Alg::Point2D locpos(0.,0.);
 //       const Alg::Vector3D mom(0.,0.,0.);
 //       if (this->surfaceRepresentation()->globalToLocal(glopos,mom,locpos)) return (m_surfaces->object(locpos));
    }
    return SurfaceVector();
}

const Reco::SurfaceVector Reco::Layer::getModules(const Alg::Point2D& locpos) const
{
    Alg::Point3D glopos(0.,0.,0.);
    const Alg::Vector3D mom(0.,0.,0.);
    this->surfaceRepresentation()->localToGlobal(locpos,mom,glopos);
    return (Reco::Layer::getModules(glopos));
}

void Reco::Layer::setNextLayer(std::shared_ptr<const Layer> layer) const
{
    m_nextLayer = layer;
}

void Reco::Layer::setPreviousLayer(std::shared_ptr<const Layer> layer) const
{
    m_previousLayer = layer;
}

const Reco::Layer* Reco::Layer::getNextLayer() const
{
    return (m_nextLayer.get());
}

const Reco::Layer* Reco::Layer::getPreviousLayer() const
{
    return (m_previousLayer.get());
}

const Reco::Layer* Reco::Layer::getNextLayer(const Alg::Vector3D&) const
{
    return getNextLayer();
}

const Reco::Layer* Reco::Layer::getPreviousLayer(const Alg::Vector3D&) const
{
    return getPreviousLayer();
}