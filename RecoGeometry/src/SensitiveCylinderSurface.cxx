//
//  SensitiveCylinderSurface.cxx
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#include "RecoGeometry/SensitiveCylinderSurface.h"

Reco::SensitiveCylinderSurface::SensitiveCylinderSurface(TGeoNode* node, TGeoConeSeg* tube, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
Reco::CylinderSurface(node, tube),
Reco::SensitiveSurface(volumeID, segmentation)
{}

Reco::SensitiveCylinderSurface::SensitiveCylinderSurface(TGeoConeSeg* tube, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
Reco::CylinderSurface(tube, transf),
Reco::SensitiveSurface(volumeID, segmentation)
{}

Reco::SensitiveCylinderSurface::SensitiveCylinderSurface(TGeoNode* node, TGeoConeSeg* tube, MaterialMap* materialmap, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
Reco::CylinderSurface(node, tube, materialmap),
Reco::SensitiveSurface(volumeID, segmentation)
{}

Reco::SensitiveCylinderSurface::SensitiveCylinderSurface(TGeoConeSeg* tube, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
Reco::CylinderSurface(tube, materialmap, transf),
Reco::SensitiveSurface(volumeID, segmentation)
{}

Reco::SensitiveCylinderSurface::SensitiveCylinderSurface(std::shared_ptr<const Alg::Transform3D> transf, double radius, double halfZ, double HalfThickness, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
Reco::CylinderSurface(transf, radius, halfZ, HalfThickness),
Reco::SensitiveSurface(volumeID, segmentation)
{}

Reco::SensitiveCylinderSurface::SensitiveCylinderSurface(double radius, double halfZ, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
Reco::CylinderSurface(radius, halfZ),
Reco::SensitiveSurface(volumeID, segmentation)
{}

Reco::SensitiveCylinderSurface:: SensitiveCylinderSurface(const Reco::SensitiveCylinderSurface& senscyl) :
Reco::CylinderSurface(senscyl),
Reco::SensitiveSurface(senscyl)
{}

Reco::SensitiveCylinderSurface::~SensitiveCylinderSurface()
{}

Reco::SensitiveCylinderSurface* Reco::SensitiveCylinderSurface::clone() const
{
    return (new Reco::SensitiveCylinderSurface(*this));
}

Reco::SensitiveCylinderSurface& Reco::SensitiveCylinderSurface::operator=(const Reco::SensitiveCylinderSurface& senscyl)
{
    if (this!=&senscyl) {
        Reco::CylinderSurface::operator=(senscyl);
        Reco::SensitiveSurface::operator=(senscyl);
    }
    
    return (*this);
}

bool Reco::SensitiveCylinderSurface::isSensitive() const
{
    return true;
}

const Reco::CylinderSurface* Reco::SensitiveCylinderSurface::surfaceRepresentation() const
{
    return (this);
}


