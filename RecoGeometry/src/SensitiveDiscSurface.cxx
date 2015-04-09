//
//  SensitiveDiscSurface.cxx
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#include "RecoGeometry/SensitiveDiscSurface.h"

Reco::SensitiveDiscSurface::SensitiveDiscSurface(TGeoNode* node, TGeoConeSeg* tube, size_t binsR, size_t binsPhi, long long int volumeID) :
Reco::DiscSurface(node, tube),
Reco::SensitiveSurface(volumeID)
{
    m_binutility    = new Trk::BinUtility(binsR, DiscSurface::getRmin(), DiscSurface::getRmax(), Trk::open, Trk::binR);
    *m_binutility  += Trk::BinUtility(binsPhi, 0., 2.*M_PI, Trk::closed, Trk::binPhi);
}

Reco::SensitiveDiscSurface::SensitiveDiscSurface(TGeoConeSeg* tube, std::shared_ptr<const Alg::Transform3D> transf, size_t binsR , size_t binsPhi, long long int volumeID) :
Reco::DiscSurface(tube,transf),
Reco::SensitiveSurface(volumeID)
{
    m_binutility    = new Trk::BinUtility(binsR, DiscSurface::getRmin(), DiscSurface::getRmax(), Trk::open, Trk::binR);
    *m_binutility  += Trk::BinUtility(binsPhi, 0., 2.*M_PI, Trk::closed, Trk::binPhi);
}


Reco::SensitiveDiscSurface::SensitiveDiscSurface(TGeoNode* node, TGeoConeSeg* tube, MaterialMap* materialmap, size_t binsR, size_t binsPhi, long long int volumeID) :
Reco::DiscSurface(node, tube, materialmap),
Reco::SensitiveSurface(volumeID)
{
    m_binutility    = new Trk::BinUtility(binsR, DiscSurface::getRmin(), DiscSurface::getRmax(), Trk::open, Trk::binR);
    *m_binutility  += Trk::BinUtility(binsPhi, 0., 2.*M_PI, Trk::closed, Trk::binPhi);
}

Reco::SensitiveDiscSurface::SensitiveDiscSurface(TGeoConeSeg* tube, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, size_t binsR, size_t binsPhi, long long int volumeID) :
Reco::DiscSurface(tube, materialmap, transf),
Reco::SensitiveSurface(volumeID)
{
    m_binutility    = new Trk::BinUtility(binsR, DiscSurface::getRmin(), DiscSurface::getRmax(), Trk::open, Trk::binR);
    *m_binutility  += Trk::BinUtility(binsPhi, 0., 2.*M_PI, Trk::closed, Trk::binPhi);
}

Reco::SensitiveDiscSurface::SensitiveDiscSurface(std::shared_ptr<const Alg::Transform3D> transf, double Rmin, double Rmax, size_t binsR, size_t binsPhi, long long int volumeID) :
Reco::DiscSurface(transf, Rmin, Rmax),
Reco::SensitiveSurface(volumeID)
{
    m_binutility    = new Trk::BinUtility(binsR, DiscSurface::getRmin(), DiscSurface::getRmax(), Trk::open, Trk::binR);
    *m_binutility  += Trk::BinUtility(binsPhi, 0., 2.*M_PI, Trk::closed, Trk::binPhi);
}

Reco::SensitiveDiscSurface::SensitiveDiscSurface(const Reco::SensitiveDiscSurface& sensdisc) :
Reco::DiscSurface(sensdisc),
Reco::SensitiveSurface(sensdisc),
m_binutility(new Trk::BinUtility(*sensdisc.m_binutility))
{}

Reco::SensitiveDiscSurface::~SensitiveDiscSurface()
{
    delete m_binutility;
}

Reco::SensitiveDiscSurface* Reco::SensitiveDiscSurface::clone() const
{
    return (new Reco::SensitiveDiscSurface(*this));
}

Reco::SensitiveDiscSurface& Reco::SensitiveDiscSurface::operator=(const Reco::SensitiveDiscSurface& sensdisc)
{
    if (this!=&sensdisc) {
        delete m_binutility;
        m_binutility  = new Trk::BinUtility(*sensdisc.m_binutility);
    }
    return (*this);
}

size_t Reco::SensitiveDiscSurface::bin(const Alg::Point2D& locpos) const
{
    size_t i = m_binutility->bin(locpos,0);
    size_t j = m_binutility->bin(locpos,1);
    int n = m_binutility->bins(1);
    return(j+i*n);
}

