#include "DD4hep/DetFactoryHelper.h"

namespace det {
/**
  Simple parallelogram using dimensions to be used to define cone composed of 1 single material
  @author Clement Helsens
**/
static DD4hep::Geometry::Ref_t
createSimpleParallelogram(DD4hep::Geometry::LCDD& lcdd, xml_h e, DD4hep::Geometry::SensitiveDetector) {
  xml_det_t x_det = e;
  std::string name = x_det.nameStr();
  DD4hep::Geometry::DetElement parallelogramDet(name, x_det.id());

  DD4hep::Geometry::Volume experimentalHall = lcdd.pickMotherVolume(parallelogramDet);

  xml_comp_t parallelogramDim(x_det.child(_U(dimensions)));
  /// correct implementation but bug in the xml parser??

  const std::array<double, 16> parallelogramPoints = {parallelogramDim.x0(),
                                                      parallelogramDim.y0(),
                                                      parallelogramDim.x0(),
                                                      parallelogramDim.y0() + parallelogramDim.dy(),
                                                      parallelogramDim.x0() + parallelogramDim.dx(),
                                                      parallelogramDim.y0() + parallelogramDim.dy(),
                                                      parallelogramDim.x0() + parallelogramDim.dx(),
                                                      parallelogramDim.y0(),
                                                      parallelogramDim.x1(),
                                                      parallelogramDim.y1(),
                                                      parallelogramDim.x1(),
                                                      parallelogramDim.y1() + parallelogramDim.dy(),
                                                      parallelogramDim.x1() + parallelogramDim.dx(),
                                                      parallelogramDim.y1() + parallelogramDim.dy(),
                                                      parallelogramDim.x1() + parallelogramDim.dx(),
                                                      parallelogramDim.y1()};

  DD4hep::Geometry::EightPointSolid parallelogram(parallelogramDim.dz(), parallelogramPoints.data());

  DD4hep::Geometry::Volume parallelogramVol(
      x_det.nameStr() + "_SimpleParallelogram", parallelogram, lcdd.material(parallelogramDim.materialStr()));

  DD4hep::Geometry::PlacedVolume parallelogramPhys;

  double zoff = parallelogramDim.z_offset();

  if (fabs(zoff) > 0.000000000001) {
    DD4hep::Geometry::Position trans(0., 0., zoff);
    parallelogramPhys = experimentalHall.placeVolume(
        parallelogramVol, DD4hep::Geometry::Transform3D(DD4hep::Geometry::RotationZ(0.), trans));
  } else
    parallelogramPhys = experimentalHall.placeVolume(parallelogramVol);

  parallelogramPhys.addPhysVolID("system", x_det.id()).addPhysVolID("side", 0);

  parallelogramDet.setPlacement(parallelogramPhys);

  parallelogramDet.setVisAttributes(lcdd, x_det.visStr(), parallelogramVol);

  return parallelogramDet;
}
}

DECLARE_DETELEMENT(SimpleParallelogram, det::createSimpleParallelogram)
