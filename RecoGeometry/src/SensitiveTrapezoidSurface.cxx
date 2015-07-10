//
//  TrapezoidSensitiveSurface.cxx
//  
//
//  Created by Julia Hrdinka on 30/03/15.
//
//

#include "RecoGeometry/SensitiveTrapezoidSurface.h"

Reco::SensitiveTrapezoidSurface::SensitiveTrapezoidSurface(TGeoNode* node, TGeoTrd2* trapezoid, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
TrapezoidSurface(node, trapezoid),
SensitiveSurface(volumeID, segmentation)
{}

Reco::SensitiveTrapezoidSurface::SensitiveTrapezoidSurface(TGeoTrd2* trapezoid, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
TrapezoidSurface(trapezoid, transf),
SensitiveSurface(volumeID, segmentation)
{}

Reco::SensitiveTrapezoidSurface::SensitiveTrapezoidSurface(TGeoNode* node, TGeoTrd2* trapezoid, MaterialMap* materialmap, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
Reco::TrapezoidSurface(node, trapezoid, materialmap),
Reco::SensitiveSurface(volumeID, segmentation)
{}

Reco::SensitiveTrapezoidSurface::SensitiveTrapezoidSurface(TGeoTrd2* trapezoid, MaterialMap* materialmap, std::shared_ptr<const Alg::Transform3D> transf, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
Reco::TrapezoidSurface(trapezoid, materialmap, transf),
Reco::SensitiveSurface(volumeID, segmentation)
{}
                                                           
Reco::SensitiveTrapezoidSurface::SensitiveTrapezoidSurface(std::shared_ptr<const Alg::Transform3D> transf, double halfX1, double halfX2, double halfY, double HalfThickness, long long int volumeID, std::shared_ptr<const DD4hep::Geometry::Segmentation> segmentation) :
Reco::TrapezoidSurface(transf, halfX1, halfX2, halfY, HalfThickness),
Reco::SensitiveSurface(volumeID, segmentation)
{}

Reco::SensitiveTrapezoidSurface::SensitiveTrapezoidSurface(const Reco::SensitiveTrapezoidSurface& senstrap) :
Reco::TrapezoidSurface(senstrap),
Reco::SensitiveSurface(senstrap)
{}

Reco::SensitiveTrapezoidSurface::~SensitiveTrapezoidSurface()
{}

Reco::SensitiveTrapezoidSurface* Reco::SensitiveTrapezoidSurface::clone() const
{
    return (new Reco::SensitiveTrapezoidSurface(*this));
}
                                                           
Reco::SensitiveTrapezoidSurface& Reco::SensitiveTrapezoidSurface::operator=(const Reco::SensitiveTrapezoidSurface& senstrap)
{
    if (this!=&senstrap) {
        Reco::TrapezoidSurface::operator=(senstrap);
        Reco::SensitiveSurface::operator=(senstrap);
    }
    return (*this);
}

bool Reco::SensitiveTrapezoidSurface::isSensitive() const
{
    return true;
}

const Reco::TrapezoidSurface* Reco::SensitiveTrapezoidSurface::surfaceRepresentation() const
{
    return (this);
}
