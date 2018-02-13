#include "DD4hep/DetFactoryHelper.h"

namespace det {
static dd4hep::Ref_t createSimpleBox(dd4hep::Detector& aLcdd,
                                               dd4hep::xml::Handle_t aXmlElement,
                                               dd4hep::SensitiveDetector aSensDet) {
  dd4hep::xml::DetElement x_det = aXmlElement;
  std::string name = x_det.nameStr();
  dd4hep::xml::Dimension dim(x_det.dimensions());
  dd4hep::xml::Dimension pos(x_det.position());
  dd4hep::xml::Dimension rot(x_det.rotation());
  dd4hep::Material mat(aLcdd.material(x_det.materialStr()));
  dd4hep::DetElement det(name, x_det.id());
  dd4hep::Volume det_vol(name + "_vol", dd4hep::Box(dim.x(), dim.y(), dim.z()), mat);
  dd4hep::Volume mother = aLcdd.pickMotherVolume(det);
  dd4hep::Transform3D transform(
      dd4hep::Rotation3D(dd4hep::RotationZYX(rot.z(), rot.y(), rot.x())),
      dd4hep::Position(pos.x(), pos.y(), pos.z()));
  dd4hep::PlacedVolume phv = mother.placeVolume(det_vol, transform);

  det_vol.setVisAttributes(aLcdd, x_det.visStr());
  det_vol.setLimitSet(aLcdd, x_det.limitsStr());
  det_vol.setRegion(aLcdd, x_det.regionStr());
  if (x_det.isSensitive()) {
    dd4hep::SensitiveDetector sd = aSensDet;
    dd4hep::xml::Dimension sd_typ = x_det.child(_U(sensitive));
    det_vol.setSensitiveDetector(aSensDet);
    sd.setType(sd_typ.typeStr());
  }
  if (x_det.hasAttr(_U(id))) {
    phv.addPhysVolID("system", x_det.id());
  }
  det.setPlacement(phv);
  return det;
}
}
// first argument is the type from the xml file
DECLARE_DETELEMENT(SimpleBox, det::createSimpleBox)
