#include "DD4hep/DetFactoryHelper.h"

namespace det {
/**
  Simple cylinder using Tube to be used to define cylinder composed of 1 single material
  @author Clement Helsens
**/
static dd4hep::Ref_t
createSimpleCylinder(dd4hep::Detector& lcdd, xml_h e, dd4hep::SensitiveDetector sensDet) {
  xml_det_t x_det = e;
  std::string name = x_det.nameStr();
  dd4hep::DetElement cylinderDet(name, x_det.id());

  dd4hep::Volume experimentalHall = lcdd.pickMotherVolume(cylinderDet);

  xml_comp_t cylinderDim(x_det.child(_U(dimensions)));

  dd4hep::Tube cylinder(
      cylinderDim.rmin(), cylinderDim.rmax(), cylinderDim.dz(), cylinderDim.phi0(), cylinderDim.deltaphi());

  dd4hep::Volume cylinderVol(
      x_det.nameStr() + "_SimpleCylinder", cylinder, lcdd.material(cylinderDim.materialStr()));

  if (x_det.isSensitive()) {
    dd4hep::xml::Dimension sdType(x_det.child(_U(sensitive)));
    cylinderVol.setSensitiveDetector(sensDet);
    sensDet.setType(sdType.typeStr());
  }

  dd4hep::PlacedVolume cylinderPhys;

  double zoff = cylinderDim.z_offset();
  if (fabs(zoff) > 0.000000000001) {
    dd4hep::Position trans(0., 0., zoff);
    cylinderPhys = experimentalHall.placeVolume(cylinderVol,
                                                dd4hep::Transform3D(dd4hep::RotationZ(0.), trans));
  } else
    cylinderPhys = experimentalHall.placeVolume(cylinderVol);

  cylinderPhys.addPhysVolID("system", x_det.id());

  cylinderDet.setPlacement(cylinderPhys);

  cylinderDet.setVisAttributes(lcdd, x_det.visStr(), cylinderVol);

  return cylinderDet;
}
}
DECLARE_DETELEMENT(SimpleCylinder, det::createSimpleCylinder)
