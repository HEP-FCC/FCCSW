#include "DD4hep/DetFactoryHelper.h"

namespace det {
static dd4hep::Ref_t createSlicedBarrel(dd4hep::Detector& aLcdd,
                                                  dd4hep::xml::Handle_t aXmlElement,
                                                  dd4hep::SensitiveDetector aSensDet) {
  dd4hep::xml::DetElement x_det = aXmlElement;
  std::string name = x_det.nameStr();
  dd4hep::xml::Dimension dim(x_det.dimensions());
  dd4hep::xml::Dimension pos(x_det.position());
  dd4hep::xml::Dimension rot(x_det.rotation());
  dd4hep::DetElement det(name, x_det.id());
  dd4hep::Volume det_vol(name + "_vol", dd4hep::Tube(dim.rmin(), dim.rmax(), dim.dz()),
                                   aLcdd.material(x_det.materialStr()));
  dd4hep::Volume mother = aLcdd.pickMotherVolume(det);
  dd4hep::Transform3D transform(
      dd4hep::Rotation3D(dd4hep::RotationZYX(rot.z(), rot.y(), rot.x())),
      dd4hep::Position(pos.x(), pos.y(), pos.z()));

  // here add cells :
  dd4hep::xml::DetElement cell_det = aXmlElement.child(_Unicode(module));
  std::string cell_name = cell_det.nameStr();
  dd4hep::xml::Dimension slice_dim(cell_det.dimensions());
  dd4hep::Material cell_mat(aLcdd.material(cell_det.materialStr()));
  // we assume no segmentation in R/phi direction
  unsigned int numSlices = dim.dz() / slice_dim.dz();
  double offset = -1. * dim.dz() + slice_dim.dz();
  dd4hep::Volume slice_vol(
      cell_name + "_vol", dd4hep::Tube(slice_dim.rmin(), slice_dim.rmax(), slice_dim.dz()), cell_mat);

  dd4hep::SensitiveDetector sd = aSensDet;
  dd4hep::xml::Dimension sd_typ = x_det.child(_U(sensitive));
  sd.setType(sd_typ.typeStr());
  slice_vol.setSensitiveDetector(aSensDet);
  for (uint iz = 0; iz < numSlices; iz++) {
    auto sliceName = cell_name + dd4hep::xml::_toString(iz, "z%d");
    dd4hep::Transform3D transformcell(dd4hep::Rotation3D(dd4hep::RotationZYX(0, 0, 0)),
                                                dd4hep::Position(0, 0, offset + iz * 2 * slice_dim.dz()));
    dd4hep::PlacedVolume slice_physvol = det_vol.placeVolume(slice_vol, transformcell);
    slice_physvol.addPhysVolID("z", iz);
  }
  // place outer box
  dd4hep::PlacedVolume phv = mother.placeVolume(det_vol, transform);
  det_vol.setVisAttributes(aLcdd, x_det.visStr());
  det_vol.setLimitSet(aLcdd, x_det.limitsStr());
  det_vol.setRegion(aLcdd, x_det.regionStr());
  phv.addPhysVolID("system", x_det.id());
  det.setPlacement(phv);
  return det;
}
}
// first argument is the type from the xml file
DECLARE_DETELEMENT(SlicedBarrel, det::createSlicedBarrel)
