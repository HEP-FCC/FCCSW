#include "DD4hep/DetFactoryHelper.h"

#include "DetCommon/DetUtils.h"

namespace det {
/**
  Simple cone using dimensions to be used to define cone composed of 1 single material
  @author Clement Helsens
**/
static dd4hep::Ref_t
createSimpleCone(dd4hep::Detector& lcdd, xml_h e, dd4hep::SensitiveDetector sensDet) {
  xml_det_t x_det = e;
  std::string name = x_det.nameStr();
  dd4hep::DetElement coneDet(name, x_det.id());

  dd4hep::Volume experimentalHall = lcdd.pickMotherVolume(coneDet);

  xml_comp_t coneDim(x_det.child(_U(dimensions)));
  dd4hep::Cone cone(coneDim.dz(), coneDim.rmin1(), coneDim.rmax1(), coneDim.rmin2(), coneDim.rmax2());

  dd4hep::Volume coneVol(x_det.nameStr() + "_SimpleCone", cone, lcdd.material(coneDim.materialStr()));

  if (x_det.isSensitive()) {
    dd4hep::xml::Dimension sdType(x_det.child(_U(sensitive)));
    coneVol.setSensitiveDetector(sensDet);
    sensDet.setType(sdType.typeStr());  
  }

  dd4hep::PlacedVolume conePhys;

  double zoff = coneDim.z_offset();
  if (fabs(zoff) > 0.000000000001) {
    double reflectionAngle = 0.;
    if (coneDim.hasAttr(_Unicode(reflect))) {
      if (coneDim.reflect()) {
        reflectionAngle = M_PI;
        }
    }
    dd4hep::Position trans(0., 0., zoff);
    conePhys =
        experimentalHall.placeVolume(coneVol, dd4hep::Transform3D(dd4hep::RotationX(reflectionAngle), trans));
  } else
    conePhys = experimentalHall.placeVolume(coneVol);

  conePhys.addPhysVolID("system", x_det.id());

  coneDet.setPlacement(conePhys);

  coneDet.setVisAttributes(lcdd, x_det.visStr(), coneVol);
  return coneDet;
}
}

DECLARE_DETELEMENT(SimpleCone, det::createSimpleCone)
