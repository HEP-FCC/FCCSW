//
//  DiscLayer.cxx
//  
//
//  Created by Julia Hrdinka on 08/12/14.
//
//

#include "RecoGeometry/DiscLayer.h"

Reco::DiscLayer::DiscLayer() :
DiscSurface(),
Layer (),
m_dz (0.)
{}

Reco::DiscLayer::DiscLayer(TGeoNode* node, TGeoConeSeg* tube, Trk::BinnedArray<Surface>* sf) :
DiscSurface(node, tube),
Layer(sf),
m_dz(tube->GetDz())
{}

Reco::DiscLayer::DiscLayer(std::shared_ptr<const Alg::Transform3D> transf, TGeoConeSeg* tube, Trk::BinnedArray<Surface>* sf) :
DiscSurface(tube,transf),
Layer(sf),
m_dz(tube->GetDZ())
{}

Reco::DiscLayer::DiscLayer(std::shared_ptr<const Alg::Transform3D> transf, double rmin, double rmax, double dz, Trk::BinnedArray<Surface>* sf) :
DiscSurface(transf, rmin, rmax),
Layer(sf),
m_dz(dz)
{}

Reco::DiscLayer::~DiscLayer()
{}

Reco::DiscLayer* Reco::DiscLayer::clone() const
{
    return (new Reco::DiscLayer(*this));
}

double Reco::DiscLayer::getHalfZ() const
{
    return(m_dz);
}

const std::vector<const Reco::Surface*> Reco::DiscLayer::compatibleSurfaces(const Alg::Point3D& glopos) const
{
    std::vector <const Reco::Surface*> surfaces;
    double r    = 0.;
    double phi  = 0.;
    Trk::BinUtility binutil(*(m_surfaces->binUtility()));
    int m = binutil.bin(glopos,0);
    int n = binutil.bin(glopos,1);
    int maxR    = binutil.bins(0);
    int maxPhi  = binutil.bins(1);
    for (int i = m-1; i<=m+1; i++) {
        if (i >= 0 && i < maxR) {
            for (int j = n-1; j<=n+1; j++) {
                if (j>=0 && j<maxPhi) {
                    r   = binutil.bincenter(i,0);
                    phi = binutil.bincenter(j,1);
                    surfaces.push_back(m_surfaces->object(Alg::Point3D(r*cos(phi),r*sin(phi),glopos.Z())));
                }
            }
        }
    }
    
    return(surfaces);
}

void Reco::DiscLayer::setNextLayer(std::shared_ptr<const Layer> layer) const
{
    Reco::Layer::setNextLayer(layer);
}

void Reco::DiscLayer::setPreviousLayer(std::shared_ptr<const Layer> layer) const
{
    Reco::Layer::setPreviousLayer(layer);
}

const Reco::Layer* Reco::DiscLayer::getNextLayer() const
{
    return Reco::Layer::getNextLayer();
}

const Reco::Layer* Reco::DiscLayer::getPreviousLayer() const
{
    return Reco::Layer::getPreviousLayer();
}

const Reco::Layer* Reco::DiscLayer::getNextLayer(const Alg::Vector3D& dir) const
{
    if (dir.Z()*normal().Z()>0) return getNextLayer();
    if (dir.Z()*normal().Z()<0) return getPreviousLayer();
    else return 0;
    //noch ueberlegen wie das genau gehen soll eventuell mit punkt
    //was wenn
}

const Reco::Layer* Reco::DiscLayer::getPreviousLayer(const Alg::Vector3D& dir) const
{
    if (dir.Z()*normal().Z()>0) return getPreviousLayer();
    if (dir.Z()*normal().Z()<0) return getNextLayer();
    else return 0;
}

bool Reco::DiscLayer::onLayer(const Alg::Point3D& glopos, double tol) const
{
    Alg::Point3D locpos (transform().Inverse()*glopos);
    double r    = sqrt(locpos.X()*locpos.X()+locpos.Y()*locpos.Y());
    return ((r>=(getRmin()-tol)) && (r<=(getRmax()+tol)) && (fabs(locpos.Z()) <= (m_dz+tol)));
}

Reco::Layer::LayerType Reco::DiscLayer::type() const
{
    return (Reco::Layer::disc);
}

const Reco::DiscSurface* Reco::DiscLayer::surfaceRepresentation() const
{
    return (this);
}



