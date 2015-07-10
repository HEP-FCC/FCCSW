//
//  CylinderLayer.cxx
//  
//
//  Created by Julia Hrdinka on 26/11/14.
//
//

#include "RecoGeometry/CylinderLayer.h"

Reco::CylinderLayer::CylinderLayer() :
Layer (),
CylinderSurface(),
m_Rmin(0.),
m_Rmax(0.)
{}

Reco::CylinderLayer::CylinderLayer(TGeoNode* node, TGeoConeSeg* tube, Trk::BinnedArray<std::vector<std::shared_ptr<const Surface>>>* sf) :
Layer(sf),
CylinderSurface(node, tube),
m_Rmin(tube->GetRmin1()),
m_Rmax(tube->GetRmax1())
{}

Reco::CylinderLayer::CylinderLayer(std::shared_ptr<const Alg::Transform3D> transf, TGeoConeSeg* tube, Trk::BinnedArray<std::vector<std::shared_ptr<const Surface>>>* sf) :
Layer(sf),
CylinderSurface(tube, transf),
m_Rmin(tube->GetRmin1()),
m_Rmax(tube->GetRmax1())
{}

Reco::CylinderLayer::CylinderLayer(std::shared_ptr<const Alg::Transform3D> transf, double rmin, double rmax, double halfZ, double HalfThickness, Trk::BinnedArray<std::vector<std::shared_ptr<const Surface>>>* sf) :
Layer(sf),
CylinderSurface(transf, 0.5*(rmin+rmax), halfZ, HalfThickness),
m_Rmin(rmin),
m_Rmax(rmax)
{}

Reco::CylinderLayer::~CylinderLayer()
{}

Reco::CylinderLayer* Reco::CylinderLayer::clone() const
{
    return (new Reco::CylinderLayer(*this));
}

double Reco::CylinderLayer::getRmin() const
{
    return (m_Rmin);
}

double Reco::CylinderLayer::getRmax() const
{
    return (m_Rmax);
}
double Reco::CylinderLayer::getDR() const
{
    return (fabs(m_Rmax-m_Rmin));
}

const std::vector<Reco::SurfaceVector> Reco::CylinderLayer::compatibleSurfaces(const Alg::Point3D& glopos) const
{
    std::vector <Reco::SurfaceVector> surfaces;
    double r    = sqrt(glopos.Perp2());
    double phi  = 0.;
    double z    = 0.;
    Trk::BinUtility binutil(*(m_surfaces->binUtility()));
    int m = binutil.bin(glopos,0);
    int n = binutil.bin(glopos,1);
    int maxPhi   = binutil.bins(0);
    int maxZ     = binutil.bins(1);
    int k        = 0;
    for (int i = m-1; i<=m+1; i++) {
        if (i==-1)          k = maxPhi-1;
        else if (i==maxPhi) k = 0;
        else k = i;
            for (int j = n-1; j<=n+1; j++) {
                if (j>=0 && j<maxZ) {
                    phi = binutil.bincenter(k,0);
                    z   = binutil.bincenter(j,1);
                    std::vector<std::shared_ptr<const Reco::Surface>> surf(*m_surfaces->object(Alg::Point3D(r*cos(phi),r*sin(phi),z)));
                    surfaces.push_back(*(m_surfaces->object(Alg::Point3D(r*cos(phi),r*sin(phi),z))));
                }
            }
    }
    
    return(surfaces);
}

void Reco::CylinderLayer::setNextLayer(std::shared_ptr<const Layer> layer) const
{
    Reco::Layer::setNextLayer(layer);
}

void Reco::CylinderLayer::setPreviousLayer(std::shared_ptr<const Layer> layer) const
{
    Reco::Layer::setPreviousLayer(layer);
}

const Reco::Layer* Reco::CylinderLayer::getNextLayer() const
{
    return Reco::Layer::getNextLayer();
}

const Reco::Layer* Reco::CylinderLayer::getPreviousLayer() const
{
    return Reco::Layer::getPreviousLayer();
}

const Reco::Layer* Reco::CylinderLayer::getNextLayer(const Alg::Vector3D&) const
{
    return getNextLayer(); //noch ueberlegen wie das genau gehen soll eventuell mit punkt
}
const Reco::Layer* Reco::CylinderLayer::getPreviousLayer(const Alg::Vector3D&) const
{
    return getPreviousLayer();
}
bool Reco::CylinderLayer::onLayer(const Alg::Point3D& glopos, double tol) const
{
    Alg::Point3D locpos (transform().Inverse()*glopos);
    double r    = sqrt(locpos.X()*locpos.X()+locpos.Y()*locpos.Y());
    return ((r>=(m_Rmin-tol)) && (r<=(m_Rmax+tol)) && (fabs(locpos.Z()) <= (getHalfZ()+tol)));
}

Reco::Layer::LayerType Reco::CylinderLayer::type() const
{
    return (Reco::Layer::cylinder); 
}

const Reco::CylinderSurface* Reco::CylinderLayer::surfaceRepresentation() const
{
    return (this);
}





