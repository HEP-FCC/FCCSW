//
//  SensitivePlaneSurface.cxx
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#include "RecoGeometry/SensitivePlaneSurface.h"

Reco::SensitivePlaneSurface::SensitivePlaneSurface(TGeoNode* node, TGeoBBox* box, size_t binsX, size_t binsY, long long int volumeID) :
Reco::PlaneSurface(node,box),
Reco::SensitiveSurface(volumeID)
{
    m_binutility    = new Trk::BinUtility(binsX, -PlaneSurface::getHalfX(), PlaneSurface::getHalfX(), Trk::open, Trk::binX);
    *m_binutility  += Trk::BinUtility(binsY, -PlaneSurface::getHalfY(), PlaneSurface::getHalfY(), Trk::open, Trk::binY);
}

Reco::SensitivePlaneSurface::SensitivePlaneSurface(TGeoBBox* box, std::shared_ptr<const Alg::Transform3D> transf, size_t binsX, size_t binsY, long long int volumeID) :
Reco::PlaneSurface(box, transf),
Reco::SensitiveSurface(volumeID)
{
    m_binutility    = new Trk::BinUtility(binsX, -PlaneSurface::getHalfX(), PlaneSurface::getHalfX(), Trk::open, Trk::binX);
    *m_binutility  += Trk::BinUtility(binsY, -PlaneSurface::getHalfY(), PlaneSurface::getHalfY(), Trk::open, Trk::binY);
}

Reco::SensitivePlaneSurface::SensitivePlaneSurface(TGeoNode* node, TGeoBBox* box, MaterialMap* materialmap, size_t binsX , size_t binsY, long long int volumeID) :
Reco::PlaneSurface(node, box, materialmap),
Reco::SensitiveSurface(volumeID)
{
    m_binutility    = new Trk::BinUtility(binsX, -PlaneSurface::getHalfX(), PlaneSurface::getHalfX(), Trk::open, Trk::binX);
    *m_binutility  += Trk::BinUtility(binsY, -PlaneSurface::getHalfY(), PlaneSurface::getHalfY(), Trk::open, Trk::binY);
}


Reco::SensitivePlaneSurface::SensitivePlaneSurface(TGeoBBox* box, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, size_t binsX, size_t binsY, long long int volumeID) :
Reco::PlaneSurface(box, materialmap, transf),
Reco::SensitiveSurface(volumeID)
{
    m_binutility    = new Trk::BinUtility(binsX, -PlaneSurface::getHalfX(), PlaneSurface::getHalfX(), Trk::open, Trk::binX);
    *m_binutility  += Trk::BinUtility(binsY, -PlaneSurface::getHalfY(), PlaneSurface::getHalfY(), Trk::open, Trk::binY);
}

Reco::SensitivePlaneSurface::SensitivePlaneSurface(std::shared_ptr<const Alg::Transform3D> transf, double halfX, double halfY, size_t binsX, size_t binsY, long long int volumeID) :
Reco::PlaneSurface(transf,halfX,halfY),
Reco::SensitiveSurface(volumeID)
{
    m_binutility    = new Trk::BinUtility(binsX, -PlaneSurface::getHalfX(), PlaneSurface::getHalfX(), Trk::open, Trk::binX);
    *m_binutility  += Trk::BinUtility(binsY, -PlaneSurface::getHalfY(), PlaneSurface::getHalfY(), Trk::open, Trk::binY);
}

Reco::SensitivePlaneSurface::SensitivePlaneSurface(const Reco::SensitivePlaneSurface& sensplane) :
Reco::PlaneSurface(sensplane),
Reco::SensitiveSurface(sensplane),
m_binutility(new Trk::BinUtility(*sensplane.m_binutility))
{}

Reco::SensitivePlaneSurface::~SensitivePlaneSurface()
{
    delete m_binutility;
}

Reco::SensitivePlaneSurface* Reco::SensitivePlaneSurface::clone() const
{
    return (new Reco::SensitivePlaneSurface(*this));
}

Reco::SensitivePlaneSurface& Reco::SensitivePlaneSurface::operator=(const Reco::SensitivePlaneSurface& sensplane)
{
    if (this!=&sensplane) {
        Reco::PlaneSurface::operator=(sensplane);
        Reco::SensitiveSurface::operator=(sensplane);
        delete m_binutility;
        m_binutility = new Trk::BinUtility(*sensplane.m_binutility);
    }
    return (*this);
}
size_t Reco::SensitivePlaneSurface::bin(const Alg::Point2D&locpos) const
{
    size_t i = m_binutility->bin(locpos,0);
    size_t j = m_binutility->bin(locpos,1);
    int n = m_binutility->bins(1);
    return(j+i*n);
}




