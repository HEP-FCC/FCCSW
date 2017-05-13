#include "DD4hep/DetFactoryHelper.h"

namespace det {
/**
Factory for a shape constructed from two cylinders that are subtracted from each other.
rmin1, rmax1, z1 dimensions of the enveloping cylinder
rmin2, rmax2, z2 dimensions of the cylinder to be subtracted
@author: Joschka Lingemann
*/
static DD4hep::Geometry::Ref_t createSubtractedCylinder(DD4hep::Geometry::LCDD& lcdd,
                                                        DD4hep::XML::Handle_t xmlElement,
                                                        DD4hep::Geometry::SensitiveDetector /*sensDet*/) {
  DD4hep::XML::DetElement xmlDet = static_cast<DD4hep::XML::DetElement>(xmlElement);
  std::string name = xmlDet.nameStr();
  DD4hep::Geometry::DetElement detElement(name, xmlDet.id());
  DD4hep::Geometry::Volume experimentalHall = lcdd.pickMotherVolume(detElement);

  xml_comp_t dimensions(xmlDet.dimensions());
  DD4hep::Geometry::Tube envelope(dimensions.rmin1(), dimensions.rmax1(), dimensions.z1());
  DD4hep::Geometry::Tube negative(dimensions.rmin2(), dimensions.rmax2(), dimensions.z2());

  DD4hep::Geometry::SubtractionSolid finalShape(envelope, negative);
  DD4hep::Geometry::Volume cylVolume(name, finalShape, lcdd.material(dimensions.materialStr()));

  DD4hep::Geometry::Position trans(0., 0., dimensions.z_offset());
  DD4hep::Geometry::PlacedVolume conePhys =
      experimentalHall.placeVolume(cylVolume, DD4hep::Geometry::Transform3D(DD4hep::Geometry::RotationZ(0.), trans));

  conePhys.addPhysVolID("system", xmlDet.id());
  detElement.setPlacement(conePhys);
  detElement.setVisAttributes(lcdd, xmlDet.visStr(), cylVolume);

  return detElement;
}
}

DECLARE_DETELEMENT(SubtractedCylinder, det::createSubtractedCylinder)
