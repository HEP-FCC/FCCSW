//
//  SensitiveCylinderSurface.cxx
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#include "RecoGeometry/SensitiveCylinderSurface.h"

Reco::SensitiveCylinderSurface::SensitiveCylinderSurface(TGeoNode* node, TGeoConeSeg* tube, size_t binsRPhi, size_t binsZ, long long int volumeID) :
Reco::CylinderSurface(node, tube),
Reco::SensitiveSurface(volumeID)
{
    m_binutility    = new Trk::BinUtility(binsRPhi, 0., 2.*M_PI, Trk::closed, Trk::binPhi);
    *m_binutility  += Trk::BinUtility(binsZ, -CylinderSurface::getHalfZ(), CylinderSurface::getHalfZ(), Trk::open, Trk::binZ);
}

Reco::SensitiveCylinderSurface::SensitiveCylinderSurface(TGeoConeSeg* tube, std::shared_ptr<const Alg::Transform3D> transf, size_t binsRPhi, size_t binsZ, long long int volumeID) :
Reco::CylinderSurface(tube, transf),
Reco::SensitiveSurface(volumeID)
{
    m_binutility    = new Trk::BinUtility(binsRPhi, 0., 2.*M_PI, Trk::closed, Trk::binPhi);
    *m_binutility  += Trk::BinUtility(binsZ, -CylinderSurface::getHalfZ(), CylinderSurface::getHalfZ(), Trk::open, Trk::binZ);
}

Reco::SensitiveCylinderSurface::SensitiveCylinderSurface(TGeoNode* node, TGeoConeSeg* tube, MaterialMap* materialmap, size_t binsRPhi, size_t binsZ, long long int volumeID) :
Reco::CylinderSurface(node, tube, materialmap),
Reco::SensitiveSurface(volumeID)
{
    m_binutility    = new Trk::BinUtility(binsRPhi, 0., 2.*M_PI, Trk::closed, Trk::binPhi);
    *m_binutility  += Trk::BinUtility(binsZ, -CylinderSurface::getHalfZ(), CylinderSurface::getHalfZ(), Trk::open, Trk::binZ);
}

Reco::SensitiveCylinderSurface::SensitiveCylinderSurface(TGeoConeSeg* tube, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, size_t binsRPhi, size_t binsZ, long long int volumeID) :
Reco::CylinderSurface(tube, materialmap, transf),
Reco::SensitiveSurface(volumeID)
{
    m_binutility    = new Trk::BinUtility(binsRPhi, 0., 2.*M_PI, Trk::closed, Trk::binPhi);
    *m_binutility  += Trk::BinUtility(binsZ, -CylinderSurface::getHalfZ(), CylinderSurface::getHalfZ(), Trk::open, Trk::binZ);
}

Reco::SensitiveCylinderSurface::SensitiveCylinderSurface(std::shared_ptr<const Alg::Transform3D> transf, double radius, double halfZ, size_t binsRPhi, size_t binsZ, long long int volumeID) :
Reco::CylinderSurface(transf, radius, halfZ),
Reco::SensitiveSurface(volumeID)
{
    m_binutility    = new Trk::BinUtility(binsRPhi, 0., 2.*M_PI, Trk::closed, Trk::binPhi);
    *m_binutility  += Trk::BinUtility(binsZ, -CylinderSurface::getHalfZ(), CylinderSurface::getHalfZ(), Trk::open, Trk::binZ);
}

Reco::SensitiveCylinderSurface::SensitiveCylinderSurface(double radius, double halfZ, size_t binsRPhi, size_t binsZ, long long int volumeID) :
Reco::CylinderSurface(radius, halfZ),
Reco::SensitiveSurface(volumeID)
{
    m_binutility    = new Trk::BinUtility(binsRPhi, 0., 2.*M_PI, Trk::closed, Trk::binPhi);
    *m_binutility  += Trk::BinUtility(binsZ, -CylinderSurface::getHalfZ(), CylinderSurface::getHalfZ(), Trk::open, Trk::binZ);
}

Reco::SensitiveCylinderSurface:: SensitiveCylinderSurface(const Reco::SensitiveCylinderSurface& senscyl) :
Reco::CylinderSurface(senscyl),
Reco::SensitiveSurface(senscyl),
m_binutility(new Trk::BinUtility(*senscyl.m_binutility))
{}

Reco::SensitiveCylinderSurface::~SensitiveCylinderSurface()
{
    delete m_binutility;
}

Reco::SensitiveCylinderSurface* Reco::SensitiveCylinderSurface::clone() const
{
    return (new Reco::SensitiveCylinderSurface(*this));
}

Reco::SensitiveCylinderSurface& Reco::SensitiveCylinderSurface::operator=(const Reco::SensitiveCylinderSurface& senscyl)
{
    if (this!=&senscyl) {
        Reco::CylinderSurface::operator=(senscyl);
        Reco::SensitiveSurface::operator=(senscyl);
        delete m_binutility;
        m_binutility = new Trk::BinUtility(*senscyl.m_binutility);
    }
    
    return (*this);
}


size_t Reco::SensitiveCylinderSurface::bin(const Alg::Point2D& locpos) const
{
    size_t i = m_binutility->bin(locpos,0);
    size_t j = m_binutility->bin(locpos,1);
    size_t n = m_binutility->bins(1);
    return(j+i*n);
}










