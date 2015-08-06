//
//  NavigationLayer.cxx
//  
//
//  Created by Julia Hrdinka on 14/01/15.
//
//

#include "RecoGeometry/NavigationLayer.h"

Reco::NavigationLayer::NavigationLayer() :
Reco::Layer()
{}

Reco::NavigationLayer::NavigationLayer(std::shared_ptr<const Layer> next, std::shared_ptr<const Layer> previous) :
Reco::Layer()
{
    Reco::Layer::setNextLayer(next);
    Reco::Layer::setPreviousLayer(previous);
}

Reco::NavigationLayer::~NavigationLayer()
{}

Reco::NavigationLayer* Reco::NavigationLayer::clone() const
{
    return (new Reco::NavigationLayer(*this));
}

const std::vector<Reco::SurfaceVector> Reco::NavigationLayer::compatibleSurfaces(const Alg::Point3D&) const
{
    return(std::vector <SurfaceVector>());
}

void Reco::NavigationLayer::setNextLayer(std::shared_ptr<const Layer> layer) const
{
    Reco::Layer::setNextLayer(layer);
}

void Reco::NavigationLayer::setPreviousLayer(std::shared_ptr<const Layer> layer) const
{
    Reco::Layer::setPreviousLayer(layer);
}

const Reco::Layer* Reco::NavigationLayer::getNextLayer() const
{
    return Reco::Layer::getNextLayer();
}

const Reco::Layer* Reco::NavigationLayer::getPreviousLayer() const
{
    return Reco::Layer::getPreviousLayer();
}

const Reco::Layer* Reco::NavigationLayer::getNextLayer(const Alg::Vector3D&) const
{
    return Reco::Layer::getNextLayer();
}

const Reco::Layer* Reco::NavigationLayer::getPreviousLayer(const Alg::Vector3D&) const
{
    return Reco::Layer::getPreviousLayer();
}
bool Reco::NavigationLayer::onLayer(const Alg::Point3D&, double) const
{
    return false;
}
Reco::Layer::LayerType Reco::NavigationLayer::type() const
{
    return (Reco::Layer::navigation);
}
const Reco::Surface* Reco::NavigationLayer::surfaceRepresentation() const
{
    return 0;
}