#include "DD4hep/DetFactoryHelper.h"

namespace det {
/**
  Simple cylinder using Tube to be used to define cylinder composed of 1 single material
  @author Clement Helsens
**/
static DD4hep::Geometry::Ref_t
createSimpleCylinder(DD4hep::Geometry::LCDD& lcdd, xml_h e, DD4hep::Geometry::SensitiveDetector sensDet) {
  xml_det_t x_det = e;
  std::string name = x_det.nameStr();
  DD4hep::Geometry::DetElement cylinderDet(name, x_det.id());

  DD4hep::Geometry::Volume experimentalHall = lcdd.pickMotherVolume(cylinderDet);

  xml_comp_t cylinderDim(x_det.child(_U(dimensions)));

  DD4hep::Geometry::Tube cylinder(
      cylinderDim.rmin(), cylinderDim.rmax(), cylinderDim.dz(), cylinderDim.phi0(), cylinderDim.deltaphi());

  DD4hep::Geometry::Volume cylinderVol(
      x_det.nameStr() + "_SimpleCylinder", cylinder, lcdd.material(cylinderDim.materialStr()));

  if (x_det.isSensitive()) {
    DD4hep::XML::Dimension sdType(x_det.child(_U(sensitive)));
    cylinderVol.setSensitiveDetector(sensDet);
    sensDet.setType(sdType.typeStr());
  }

  DD4hep::Geometry::PlacedVolume cylinderPhys;

  double zoff = cylinderDim.z_offset();
  if (fabs(zoff) > 0.000000000001) {
    DD4hep::Geometry::Position trans(0., 0., zoff);
    cylinderPhys = experimentalHall.placeVolume(cylinderVol,
                                                DD4hep::Geometry::Transform3D(DD4hep::Geometry::RotationZ(0.), trans));
  } else
    cylinderPhys = experimentalHall.placeVolume(cylinderVol);

  cylinderPhys.addPhysVolID("system", x_det.id());

  cylinderDet.setPlacement(cylinderPhys);

  cylinderDet.setVisAttributes(lcdd, x_det.visStr(), cylinderVol);

  return cylinderDet;
}
}
DECLARE_DETELEMENT(SimpleCylinder, det::createSimpleCylinder)
