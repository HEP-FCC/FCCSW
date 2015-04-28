//
//  SensitivePlaneSurface.cxx
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#include "RecoGeometry/SensitivePlaneSurface.h"

Reco::SensitivePlaneSurface::SensitivePlaneSurface(TGeoNode* node, TGeoBBox* box, long long int volumeID, Trk::ReadoutSegmentation* segmentation) :
Reco::PlaneSurface(node,box),
Reco::SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}

Reco::SensitivePlaneSurface::SensitivePlaneSurface(TGeoBBox* box, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, Trk::ReadoutSegmentation* segmentation) :
Reco::PlaneSurface(box, transf),
Reco::SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}

Reco::SensitivePlaneSurface::SensitivePlaneSurface(TGeoNode* node, TGeoBBox* box, MaterialMap* materialmap, long long int volumeID, Trk::ReadoutSegmentation* segmentation) :
Reco::PlaneSurface(node, box, materialmap),
Reco::SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}


Reco::SensitivePlaneSurface::SensitivePlaneSurface(TGeoBBox* box, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, Trk::ReadoutSegmentation* segmentation) :
Reco::PlaneSurface(box, materialmap, transf),
Reco::SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}

Reco::SensitivePlaneSurface::SensitivePlaneSurface(std::shared_ptr<const Alg::Transform3D> transf, double halfX, double halfY, long long int volumeID, Trk::ReadoutSegmentation* segmentation) :
Reco::PlaneSurface(transf,halfX,halfY),
Reco::SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}

Reco::SensitivePlaneSurface::SensitivePlaneSurface(const Reco::SensitivePlaneSurface& sensplane) :
Reco::PlaneSurface(sensplane),
Reco::SensitiveSurface(sensplane),
m_segmentation(sensplane.m_segmentation->clone())
{}

Reco::SensitivePlaneSurface::~SensitivePlaneSurface()
{
    delete m_segmentation;
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
        delete m_segmentation;
        m_segmentation = m_segmentation->clone();
    }
    return (*this);
}
unsigned long Reco::SensitivePlaneSurface::bin(const Alg::Point2D&locpos) const
{
    return(m_segmentation->bin(locpos));
}

Alg::Point2D Reco::SensitivePlaneSurface::binToLocpos(unsigned long bin) const
{
    return (binToLocpos(bin));
}

bool Reco::SensitivePlaneSurface::isSensitive() const
{
    return true;
}

float Reco::SensitivePlaneSurface::binWidth(const Alg::Point2D& locpos, size_t ba) const
{
    return(m_segmentation->binwidth(locpos, ba));
}

const std::vector<unsigned long> Reco::SensitivePlaneSurface::compatibleBins(const Alg::Point2D& locpos) const
{
    return(m_segmentation->compatibleBins(locpos));
}

