#include "DD4hep/DetFactoryHelper.h"

namespace det {
static dd4hep::Ref_t createSimpleBoxes(dd4hep::Detector& aLcdd,
                                                 dd4hep::xml::Handle_t aXmlElement,
                                                 dd4hep::SensitiveDetector aSensDet) {
  dd4hep::xml::DetElement x_det = aXmlElement;
  std::string name = x_det.nameStr();
  dd4hep::xml::Dimension dim(x_det.dimensions());
  dd4hep::xml::Dimension pos(x_det.position());
  dd4hep::xml::Dimension rot(x_det.rotation());
  dd4hep::DetElement det(name, x_det.id());
  dd4hep::Material air = aLcdd.air();
  dd4hep::Volume det_vol(name + "_vol", dd4hep::Box(dim.x(), dim.y(), dim.z()),
                                   aLcdd.material(x_det.materialStr()));
  dd4hep::Volume mother = aLcdd.pickMotherVolume(det);
  dd4hep::Transform3D transform(
      dd4hep::Rotation3D(dd4hep::RotationZYX(rot.z(), rot.y(), rot.x())),
      dd4hep::Position(pos.x(), pos.y(), pos.z()));

  // add cells :
  dd4hep::xml::DetElement cell_det = aXmlElement.child(_Unicode(module));
  dd4hep::xml::Dimension cell_dim(cell_det.dimensions());
  dd4hep::Material cell_mat(aLcdd.material(cell_det.materialStr()));
  // assume same no of cells each direction (x,y,z)
  unsigned int numCells = dim.x() / cell_dim.x();
  double offset = -1. * dim.x() + cell_dim.x();
  // create volumes for cell, row and layer
  dd4hep::Volume cell_vol("cell_vol", dd4hep::Box(cell_dim.x(), cell_dim.y(), cell_dim.z()),
                                    cell_mat);
  dd4hep::Volume row_vol("row_vol", dd4hep::Box(dim.x(), cell_dim.y(), cell_dim.z()), air);
  dd4hep::Volume layer_vol("layer_vol", dd4hep::Box(dim.x(), dim.y(), cell_dim.z()), air);
  // attach sensitive detector
  dd4hep::SensitiveDetector sd = aSensDet;
  dd4hep::xml::Dimension sd_typ = x_det.child(_U(sensitive));
  sd.setType(sd_typ.typeStr());
  cell_vol.setSensitiveDetector(aSensDet);
  // place cells in a row
  for (uint ix = 0; ix < numCells; ix++) {
    auto cellName = std::string("cell") + dd4hep::xml::_toString(ix, "x%d");
    dd4hep::Transform3D transformcell(dd4hep::Rotation3D(dd4hep::RotationZYX(0, 0, 0)),
                                                dd4hep::Position(offset + ix * 2 * cell_dim.x(), 0, 0));
    dd4hep::PlacedVolume cell_physvol = row_vol.placeVolume(cell_vol, transformcell);
    cell_physvol.addPhysVolID("modulex", ix);
  }
  // place rows in a layer
  for (uint iy = 0; iy < numCells; iy++) {
    auto cellName = std::string("row") + dd4hep::xml::_toString(iy, "y%d");
    dd4hep::Transform3D transformrow(dd4hep::Rotation3D(dd4hep::RotationZYX(0, 0, 0)),
                                               dd4hep::Position(0, offset + iy * 2 * cell_dim.y(), 0));
    dd4hep::PlacedVolume row_physvol = layer_vol.placeVolume(row_vol, transformrow);
    row_physvol.addPhysVolID("moduley", iy);
  }
  // place layers in the box volume
  for (uint iz = 0; iz < numCells; iz++) {
    auto cellName = std::string("layer") + dd4hep::xml::_toString(iz, "z%d");
    dd4hep::Transform3D transformlayer(dd4hep::Rotation3D(dd4hep::RotationZYX(0, 0, 0)),
                                                 dd4hep::Position(0, 0, offset + iz * 2 * cell_dim.z()));
    dd4hep::PlacedVolume layer_physvol = det_vol.placeVolume(layer_vol, transformlayer);
    layer_physvol.addPhysVolID("modulez", iz);
  }

  // place box
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
DECLARE_DETELEMENT(SimpleBoxes, det::createSimpleBoxes)
