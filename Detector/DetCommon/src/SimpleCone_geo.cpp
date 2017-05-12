#include "DD4hep/DetFactoryHelper.h"

#include "DetCommon/DetUtils.h"

namespace det {
/**
  Simple cone using dimensions to be used to define cone composed of 1 single material
  @author Clement Helsens
**/
static DD4hep::Geometry::Ref_t
createSimpleCone(DD4hep::Geometry::LCDD& lcdd, xml_h e, DD4hep::Geometry::SensitiveDetector) {
  xml_det_t x_det = e;
  std::string name = x_det.nameStr();
  DD4hep::Geometry::DetElement coneDet(name, x_det.id());

  DD4hep::Geometry::Volume experimentalHall = lcdd.pickMotherVolume(coneDet);

  xml_comp_t coneDim(x_det.child(_U(dimensions)));
  DD4hep::Geometry::Cone cone(coneDim.dz(), coneDim.rmin1(), coneDim.rmax1(), coneDim.rmin2(), coneDim.rmax2());

  DD4hep::Geometry::Volume coneVol(x_det.nameStr() + "_SimpleCone", cone, lcdd.material(coneDim.materialStr()));

  DD4hep::Geometry::PlacedVolume conePhys;

  double zoff = coneDim.z_offset();
  if (fabs(zoff) > 0.000000000001) {
    double reflectionAngle = 0.;
    if (coneDim.hasAttr(_Unicode(reflect))) {
      if (coneDim.reflect()) {
        reflectionAngle = M_PI;
        }
    }
    DD4hep::Geometry::Position trans(0., 0., zoff);
    conePhys =
        experimentalHall.placeVolume(coneVol, DD4hep::Geometry::Transform3D(DD4hep::Geometry::RotationX(reflectionAngle), trans));
  } else
    conePhys = experimentalHall.placeVolume(coneVol);

  conePhys.addPhysVolID("system", x_det.id()).addPhysVolID("side", 0);

  coneDet.setPlacement(conePhys);

  coneDet.setVisAttributes(lcdd, x_det.visStr(), coneVol);
  return coneDet;
}
}

DECLARE_DETELEMENT(SimpleCone, det::createSimpleCone)
