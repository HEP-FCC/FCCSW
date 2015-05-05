//
//  TrapezoidSensitiveSurface.cxx
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#include "RecoGeometry/SensitiveTrapezoidSurface.h"

Reco::SensitiveTrapezoidSurface::SensitiveTrapezoidSurface(TGeoNode* node, TGeoTrd2* trapezoid, long long int volumeID, Reco::ReadoutSegmentation* segmentation) :
TrapezoidSurface(node, trapezoid),
SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}

Reco::SensitiveTrapezoidSurface::SensitiveTrapezoidSurface(TGeoTrd2* trapezoid, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, Reco::ReadoutSegmentation* segmentation) :
TrapezoidSurface(trapezoid, transf),
SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}

Reco::SensitiveTrapezoidSurface::SensitiveTrapezoidSurface(TGeoNode* node, TGeoTrd2* trapezoid, MaterialMap* materialmap, long long int volumeID, Reco::ReadoutSegmentation* segmentation) :
Reco::TrapezoidSurface(node, trapezoid, materialmap),
Reco::SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}

Reco::SensitiveTrapezoidSurface::SensitiveTrapezoidSurface(TGeoTrd2* trapezoid, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, Reco::ReadoutSegmentation* segmentation) :
Reco::TrapezoidSurface(trapezoid, materialmap, transf),
Reco::SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}
                                                           
Reco::SensitiveTrapezoidSurface::SensitiveTrapezoidSurface(std::shared_ptr<const Alg::Transform3D> transf, double halfX1, double halfX2, double halfY, double HalfThickness, long long int volumeID, Reco::ReadoutSegmentation* segmentation) :
Reco::TrapezoidSurface(transf, halfX1, halfX2, halfY, HalfThickness),
Reco::SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}

Reco::SensitiveTrapezoidSurface::SensitiveTrapezoidSurface(const Reco::SensitiveTrapezoidSurface& senstrap) :
Reco::TrapezoidSurface(senstrap),
Reco::SensitiveSurface(senstrap),
m_segmentation(senstrap.m_segmentation->clone())
{}

Reco::SensitiveTrapezoidSurface::~SensitiveTrapezoidSurface()
{
    delete m_segmentation;
}

Reco::SensitiveTrapezoidSurface* Reco::SensitiveTrapezoidSurface::clone() const
{
    return (new Reco::SensitiveTrapezoidSurface(*this));
}
                                                           
Reco::SensitiveTrapezoidSurface& Reco::SensitiveTrapezoidSurface::operator=(const Reco::SensitiveTrapezoidSurface& senstrap)
{
    if (this!=&senstrap) {
        Reco::TrapezoidSurface::operator=(senstrap);
        Reco::SensitiveSurface::operator=(senstrap);
        delete m_segmentation;
        m_segmentation = m_segmentation->clone();
    }
    return (*this);
}

unsigned long Reco::SensitiveTrapezoidSurface::bin(const Alg::Point2D& locpos) const
{
    return(m_segmentation->bin(locpos));
}

Alg::Point2D Reco::SensitiveTrapezoidSurface::binToLocpos(unsigned long bin) const
{
    return (binToLocpos(bin));
}

bool Reco::SensitiveTrapezoidSurface::isSensitive() const
{
    return true;
}

float Reco::SensitiveTrapezoidSurface::binWidth(const Alg::Point2D& locpos, size_t ba) const
{
    return(m_segmentation->binwidth(locpos, ba));
}

const std::vector<unsigned long> Reco::SensitiveTrapezoidSurface::compatibleBins(const Alg::Point2D& locpos) const
{
    return(m_segmentation->compatibleBins(locpos));
}



