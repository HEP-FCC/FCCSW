#include "DD4hep/DetFactoryHelper.h"

namespace det {
/**
Factory for a shape constructed from two cylinders that are subtracted from each other.
rmin1, rmax1, z1 dimensions of the enveloping cylinder
rmin2, rmax2, z2 dimensions of the cylinder to be subtracted
@author: Joschka Lingemann
*/
static dd4hep::Ref_t createSubtractedCylinder(dd4hep::Detector& lcdd,
                                                        dd4hep::xml::Handle_t xmlElement,
                                                        dd4hep::SensitiveDetector /*sensDet*/) {
  dd4hep::xml::DetElement xmlDet = static_cast<dd4hep::xml::DetElement>(xmlElement);
  std::string name = xmlDet.nameStr();
  dd4hep::DetElement detElement(name, xmlDet.id());
  dd4hep::Volume experimentalHall = lcdd.pickMotherVolume(detElement);

  xml_comp_t dimensions(xmlDet.dimensions());
  dd4hep::Tube envelope(dimensions.rmin1(), dimensions.rmax1(), dimensions.z1());
  dd4hep::Tube negative(dimensions.rmin2(), dimensions.rmax2(), dimensions.z2());

  dd4hep::SubtractionSolid finalShape(envelope, negative);
  dd4hep::Volume cylVolume(name, finalShape, lcdd.material(dimensions.materialStr()));

  dd4hep::Position trans(0., 0., dimensions.z_offset());
  dd4hep::PlacedVolume conePhys =
      experimentalHall.placeVolume(cylVolume, dd4hep::Transform3D(dd4hep::RotationZ(0.), trans));

  conePhys.addPhysVolID("system", xmlDet.id());
  detElement.setPlacement(conePhys);
  detElement.setVisAttributes(lcdd, xmlDet.visStr(), cylVolume);

  return detElement;
}
}

DECLARE_DETELEMENT(SubtractedCylinder, det::createSubtractedCylinder)
