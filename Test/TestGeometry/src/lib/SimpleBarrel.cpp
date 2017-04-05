#include "DD4hep/DetFactoryHelper.h"

namespace det {
static DD4hep::Geometry::Ref_t createSimpleBarrel(DD4hep::Geometry::LCDD& aLcdd,
                                                  DD4hep::XML::Handle_t aXmlElement,
                                                  DD4hep::Geometry::SensitiveDetector aSensDet) {
  DD4hep::XML::DetElement x_det = aXmlElement;
  std::string name = x_det.nameStr();
  DD4hep::XML::Dimension dim(x_det.dimensions());
  DD4hep::XML::Dimension pos(x_det.position());
  DD4hep::XML::Dimension rot(x_det.rotation());
  DD4hep::Geometry::DetElement det(name, x_det.id());
  DD4hep::Geometry::Volume det_vol(name + "_vol", DD4hep::Geometry::Tube(dim.rmin(), dim.rmax(), dim.dz()),
                                   aLcdd.material(x_det.materialStr()));
  DD4hep::Geometry::Volume mother = aLcdd.pickMotherVolume(det);
  DD4hep::Geometry::Transform3D transform(
      DD4hep::Geometry::Rotation3D(DD4hep::Geometry::RotationZYX(rot.z(), rot.y(), rot.x())),
      DD4hep::Geometry::Position(pos.x(), pos.y(), pos.z()));
  DD4hep::Geometry::PlacedVolume phv = mother.placeVolume(det_vol, transform);
  det_vol.setVisAttributes(aLcdd, x_det.visStr());
  det_vol.setLimitSet(aLcdd, x_det.limitsStr());
  det_vol.setRegion(aLcdd, x_det.regionStr());
  if (x_det.isSensitive()) {
    DD4hep::Geometry::SensitiveDetector sd = aSensDet;
    DD4hep::XML::Dimension sd_typ = x_det.child(_U(sensitive));
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
DECLARE_DETELEMENT(SimpleBarrel, det::createSimpleBarrel)
