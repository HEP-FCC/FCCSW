//
//  SensitiveDiscSurface.cxx
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#include "RecoGeometry/SensitiveDiscSurface.h"

Reco::SensitiveDiscSurface::SensitiveDiscSurface(TGeoNode* node, TGeoConeSeg* tube, long long int volumeID, Trk::ReadoutSegmentation* segmentation) :
Reco::DiscSurface(node, tube),
Reco::SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}

Reco::SensitiveDiscSurface::SensitiveDiscSurface(TGeoConeSeg* tube, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, Trk::ReadoutSegmentation* segmentation) :
Reco::DiscSurface(tube,transf),
Reco::SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}


Reco::SensitiveDiscSurface::SensitiveDiscSurface(TGeoNode* node, TGeoConeSeg* tube, MaterialMap* materialmap, long long int volumeID, Trk::ReadoutSegmentation* segmentation) :
Reco::DiscSurface(node, tube, materialmap),
Reco::SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}

Reco::SensitiveDiscSurface::SensitiveDiscSurface(TGeoConeSeg* tube, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, Trk::ReadoutSegmentation* segmentation) :
Reco::DiscSurface(tube, materialmap, transf),
Reco::SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}

Reco::SensitiveDiscSurface::SensitiveDiscSurface(std::shared_ptr<const Alg::Transform3D> transf, double Rmin, double Rmax, long long int volumeID, Trk::ReadoutSegmentation* segmentation) :
Reco::DiscSurface(transf, Rmin, Rmax),
Reco::SensitiveSurface(volumeID),
m_segmentation(segmentation)
{}

Reco::SensitiveDiscSurface::SensitiveDiscSurface(const Reco::SensitiveDiscSurface& sensdisc) :
Reco::DiscSurface(sensdisc),
Reco::SensitiveSurface(sensdisc),
m_segmentation(sensdisc.m_segmentation->clone())
{}

Reco::SensitiveDiscSurface::~SensitiveDiscSurface()
{
    delete m_segmentation;
}

Reco::SensitiveDiscSurface* Reco::SensitiveDiscSurface::clone() const
{
    return (new Reco::SensitiveDiscSurface(*this));
}

Reco::SensitiveDiscSurface& Reco::SensitiveDiscSurface::operator=(const Reco::SensitiveDiscSurface& sensdisc)
{
    if (this!=&sensdisc) {
        Reco::DiscSurface::operator=(sensdisc);
        Reco::SensitiveSurface::operator=(sensdisc);
        delete m_segmentation;
        m_segmentation = m_segmentation->clone();
    }
    return (*this);
}

unsigned long Reco::SensitiveDiscSurface::bin(const Alg::Point2D& locpos) const
{
    return(m_segmentation->bin(locpos));
}

Alg::Point2D Reco::SensitiveDiscSurface::binToLocpos(unsigned long bin) const
{
    return(m_segmentation->binToLocpos(bin));
}

bool Reco::SensitiveDiscSurface::isSensitive() const
{
    return true;
}

float Reco::SensitiveDiscSurface::binWidth(const Alg::Point2D& locpos, size_t ba) const
{
    return(m_segmentation->binwidth(locpos, ba));
}

const std::vector<unsigned long> Reco::SensitiveDiscSurface::compatibleBins(const Alg::Point2D& locpos) const
{
    return(m_segmentation->compatibleBins(locpos));
}
