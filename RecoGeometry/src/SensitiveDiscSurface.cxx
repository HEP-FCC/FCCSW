//
//  SensitiveDiscSurface.cxx
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#include "RecoGeometry/SensitiveDiscSurface.h"

Reco::SensitiveDiscSurface::SensitiveDiscSurface(TGeoNode* node, TGeoConeSeg* tube, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
Reco::DiscSurface(node, tube),
Reco::SensitiveSurface(volumeID, segmentation)
{}

Reco::SensitiveDiscSurface::SensitiveDiscSurface(TGeoConeSeg* tube, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
Reco::DiscSurface(tube,transf),
Reco::SensitiveSurface(volumeID, segmentation)
{}


Reco::SensitiveDiscSurface::SensitiveDiscSurface(TGeoNode* node, TGeoConeSeg* tube, MaterialMap* materialmap, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
Reco::DiscSurface(node, tube, materialmap),
Reco::SensitiveSurface(volumeID, segmentation)
{}

Reco::SensitiveDiscSurface::SensitiveDiscSurface(TGeoConeSeg* tube, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
Reco::DiscSurface(tube, materialmap, transf),
Reco::SensitiveSurface(volumeID, segmentation)
{}

Reco::SensitiveDiscSurface::SensitiveDiscSurface(std::shared_ptr<const Alg::Transform3D> transf, double Rmin, double Rmax, double HalfThickness, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
Reco::DiscSurface(transf, Rmin, Rmax, HalfThickness),
Reco::SensitiveSurface(volumeID, segmentation)
{}

Reco::SensitiveDiscSurface::SensitiveDiscSurface(const Reco::SensitiveDiscSurface& sensdisc) :
Reco::DiscSurface(sensdisc),
Reco::SensitiveSurface(sensdisc)
{}

Reco::SensitiveDiscSurface::~SensitiveDiscSurface()
{}

Reco::SensitiveDiscSurface* Reco::SensitiveDiscSurface::clone() const
{
    return (new Reco::SensitiveDiscSurface(*this));
}

Reco::SensitiveDiscSurface& Reco::SensitiveDiscSurface::operator=(const Reco::SensitiveDiscSurface& sensdisc)
{
    if (this!=&sensdisc) {
        Reco::DiscSurface::operator=(sensdisc);
        Reco::SensitiveSurface::operator=(sensdisc);
    }
    return (*this);
}

bool Reco::SensitiveDiscSurface::isSensitive() const
{
    return true;
}

const Reco::DiscSurface* Reco::SensitiveDiscSurface::surfaceRepresentation() const
{
    return (this);
}