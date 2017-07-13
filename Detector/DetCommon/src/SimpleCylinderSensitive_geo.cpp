#include "DD4hep/DetFactoryHelper.h"

namespace det {
/**
  Simple sensitive cylinder using Tube to be used to define cylinder composed of 1 single material
  @author Clement Helsens, Coralie Neubueser
**/
static DD4hep::Geometry::Ref_t
createSimpleCylinderSensitive(DD4hep::Geometry::LCDD& lcdd, xml_h e, DD4hep::Geometry::SensitiveDetector sensDet) {
  xml_det_t x_det = e;
  std::string name = x_det.nameStr();
  DD4hep::Geometry::DetElement cylinderDet(name, x_det.id());

  DD4hep::Geometry::Volume experimentalHall = lcdd.pickMotherVolume(cylinderDet);

  xml_comp_t cylinderDim(x_det.child(_U(dimensions)));
  
  DD4hep::Geometry::Tube cylinder(
      cylinderDim.rmin(), cylinderDim.rmax(), cylinderDim.dz(), cylinderDim.phi0(), cylinderDim.deltaphi());

  DD4hep::Geometry::Volume cylinderVol(
      x_det.nameStr() + "_SimpleCylinderSensitive", cylinder, lcdd.material(cylinderDim.materialStr()));
  
  // set the sensitive detector type to the DD4hep calorimeter                                                                                                                           
  sensDet.setType("SimpleCalorimeterSD");
  cylinderVol.setSensitiveDetector(sensDet);

  DD4hep::Geometry::PlacedVolume cylinderPhys;

  double zoff = cylinderDim.z_offset();
  if (fabs(zoff) > 0.000000000001) {
    DD4hep::Geometry::Position trans(0., 0., zoff);
    cylinderPhys = experimentalHall.placeVolume(cylinderVol,
                                                DD4hep::Geometry::Transform3D(DD4hep::Geometry::RotationZ(0.), trans));
  } else
    cylinderPhys = experimentalHall.placeVolume(cylinderVol);

  cylinderPhys.addPhysVolID("system", x_det.id()).addPhysVolID("side", 0);

  cylinderDet.setPlacement(cylinderPhys);

  cylinderDet.setVisAttributes(lcdd, x_det.visStr(), cylinderVol);

  return cylinderDet;
}
}
DECLARE_DETELEMENT(SimpleCylinderSensitive, det::createSimpleCylinderSensitive)
