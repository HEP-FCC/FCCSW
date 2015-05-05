//
//  SensitiveCylinderSurface.cxx
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#include "RecoGeometry/SensitiveCylinderSurface.h"

Reco::SensitiveCylinderSurface::SensitiveCylinderSurface(TGeoNode* node, TGeoConeSeg* tube, long long int volumeID, Reco::ReadoutSegmentation* segmentation) :
Reco::CylinderSurface(node, tube),
Reco::SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}

Reco::SensitiveCylinderSurface::SensitiveCylinderSurface(TGeoConeSeg* tube, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, Reco::ReadoutSegmentation* segmentation) :
Reco::CylinderSurface(tube, transf),
Reco::SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}

Reco::SensitiveCylinderSurface::SensitiveCylinderSurface(TGeoNode* node, TGeoConeSeg* tube, MaterialMap* materialmap, long long int volumeID, Reco::ReadoutSegmentation* segmentation) :
Reco::CylinderSurface(node, tube, materialmap),
Reco::SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}

Reco::SensitiveCylinderSurface::SensitiveCylinderSurface(TGeoConeSeg* tube, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, Reco::ReadoutSegmentation* segmentation) :
Reco::CylinderSurface(tube, materialmap, transf),
Reco::SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}

Reco::SensitiveCylinderSurface::SensitiveCylinderSurface(std::shared_ptr<const Alg::Transform3D> transf, double radius, double halfZ, double HalfThickness, long long int volumeID, Reco::ReadoutSegmentation* segmentation) :
Reco::CylinderSurface(transf, radius, halfZ, HalfThickness),
Reco::SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}

Reco::SensitiveCylinderSurface::SensitiveCylinderSurface(double radius, double halfZ, long long int volumeID, Reco::ReadoutSegmentation* segmentation) :
Reco::CylinderSurface(radius, halfZ),
Reco::SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}

Reco::SensitiveCylinderSurface:: SensitiveCylinderSurface(const Reco::SensitiveCylinderSurface& senscyl) :
Reco::CylinderSurface(senscyl),
Reco::SensitiveSurface(senscyl),
m_segmentation(senscyl.m_segmentation->clone())
{}

Reco::SensitiveCylinderSurface::~SensitiveCylinderSurface()
{
    delete m_segmentation;
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
        delete m_segmentation;
        m_segmentation = m_segmentation->clone();
    }
    
    return (*this);
}


unsigned long Reco::SensitiveCylinderSurface::bin(const Alg::Point2D& locpos) const
{
    return(m_segmentation->bin(locpos));
}

Alg::Point2D Reco::SensitiveCylinderSurface::binToLocpos(unsigned long bin) const
{
    return(m_segmentation->binToLocpos(bin));
}

bool Reco::SensitiveCylinderSurface::isSensitive() const
{
    return true;
}

float Reco::SensitiveCylinderSurface::binWidth(const Alg::Point2D& locpos, size_t ba) const
{
    return(m_segmentation->binwidth(locpos, ba));
}

const std::vector<unsigned long> Reco::SensitiveCylinderSurface::compatibleBins(const Alg::Point2D& locpos) const
{
    return(m_segmentation->compatibleBins(locpos));
}



