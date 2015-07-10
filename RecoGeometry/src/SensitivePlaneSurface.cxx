//
//  SensitivePlaneSurface.cxx
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#include "RecoGeometry/SensitivePlaneSurface.h"

Reco::SensitivePlaneSurface::SensitivePlaneSurface(TGeoNode* node, TGeoBBox* box, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
Reco::PlaneSurface(node,box),
Reco::SensitiveSurface(volumeID, segmentation)
{}

Reco::SensitivePlaneSurface::SensitivePlaneSurface(TGeoBBox* box, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
Reco::PlaneSurface(box, transf),
Reco::SensitiveSurface(volumeID, segmentation)
{}

Reco::SensitivePlaneSurface::SensitivePlaneSurface(TGeoNode* node, TGeoBBox* box, MaterialMap* materialmap, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
Reco::PlaneSurface(node, box, materialmap),
Reco::SensitiveSurface(volumeID, segmentation)
{}


Reco::SensitivePlaneSurface::SensitivePlaneSurface(TGeoBBox* box, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
Reco::PlaneSurface(box, materialmap, transf),
Reco::SensitiveSurface(volumeID, segmentation)
{}

Reco::SensitivePlaneSurface::SensitivePlaneSurface(std::shared_ptr<const Alg::Transform3D> transf, double halfX, double halfY, double HalfThickness, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
Reco::PlaneSurface(transf, halfX, halfY, HalfThickness),
Reco::SensitiveSurface(volumeID, segmentation)
{}

Reco::SensitivePlaneSurface::SensitivePlaneSurface(const Reco::SensitivePlaneSurface& sensplane) :
Reco::PlaneSurface(sensplane),
Reco::SensitiveSurface(sensplane)
{}

Reco::SensitivePlaneSurface::~SensitivePlaneSurface()
{}

Reco::SensitivePlaneSurface* Reco::SensitivePlaneSurface::clone() const
{
    return (new Reco::SensitivePlaneSurface(*this));
}

Reco::SensitivePlaneSurface& Reco::SensitivePlaneSurface::operator=(const Reco::SensitivePlaneSurface& sensplane)
{
    if (this!=&sensplane) {
        Reco::PlaneSurface::operator=(sensplane);
        Reco::SensitiveSurface::operator=(sensplane);
    }
    return (*this);
}

bool Reco::SensitivePlaneSurface::isSensitive() const
{
    return true;
}

const Reco::PlaneSurface* Reco::SensitivePlaneSurface::surfaceRepresentation() const
{
    return (this);
}