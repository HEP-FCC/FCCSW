#include "DD4hep/DetFactoryHelper.h"

namespace det {
/**
  Simple parallelogram using dimensions to be used to define cone composed of 1 single material
  @author Clement Helsens
**/
static dd4hep::Ref_t
createSimpleParallelogram(dd4hep::Detector& lcdd, xml_h e, dd4hep::SensitiveDetector) {
  xml_det_t x_det = e;
  std::string name = x_det.nameStr();
  dd4hep::DetElement parallelogramDet(name, x_det.id());

  dd4hep::Volume experimentalHall = lcdd.pickMotherVolume(parallelogramDet);

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

  dd4hep::EightPointSolid parallelogram(parallelogramDim.dz(), parallelogramPoints.data());

  dd4hep::Volume parallelogramVol(
      x_det.nameStr() + "_SimpleParallelogram", parallelogram, lcdd.material(parallelogramDim.materialStr()));

  dd4hep::PlacedVolume parallelogramPhys;

  double zoff = parallelogramDim.z_offset();

  if (fabs(zoff) > 0.000000000001) {
    dd4hep::Position trans(0., 0., zoff);
    parallelogramPhys = experimentalHall.placeVolume(
        parallelogramVol, dd4hep::Transform3D(dd4hep::RotationZ(0.), trans));
  } else
    parallelogramPhys = experimentalHall.placeVolume(parallelogramVol);

  parallelogramPhys.addPhysVolID("system", x_det.id()).addPhysVolID("side", 0);

  parallelogramDet.setPlacement(parallelogramPhys);

  parallelogramDet.setVisAttributes(lcdd, x_det.visStr(), parallelogramVol);

  return parallelogramDet;
}
}

DECLARE_DETELEMENT(SimpleParallelogram, det::createSimpleParallelogram)
