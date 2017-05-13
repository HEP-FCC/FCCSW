#include "DD4hep/DetFactoryHelper.h"

namespace det {
static DD4hep::Geometry::Ref_t createSimpleBoxes(DD4hep::Geometry::LCDD& aLcdd,
                                                 DD4hep::XML::Handle_t aXmlElement,
                                                 DD4hep::Geometry::SensitiveDetector aSensDet) {
  DD4hep::XML::DetElement x_det = aXmlElement;
  std::string name = x_det.nameStr();
  DD4hep::XML::Dimension dim(x_det.dimensions());
  DD4hep::XML::Dimension pos(x_det.position());
  DD4hep::XML::Dimension rot(x_det.rotation());
  DD4hep::Geometry::DetElement det(name, x_det.id());
  DD4hep::Geometry::Material air = aLcdd.air();
  DD4hep::Geometry::Volume det_vol(name + "_vol", DD4hep::Geometry::Box(dim.x(), dim.y(), dim.z()),
                                   aLcdd.material(x_det.materialStr()));
  DD4hep::Geometry::Volume mother = aLcdd.pickMotherVolume(det);
  DD4hep::Geometry::Transform3D transform(
      DD4hep::Geometry::Rotation3D(DD4hep::Geometry::RotationZYX(rot.z(), rot.y(), rot.x())),
      DD4hep::Geometry::Position(pos.x(), pos.y(), pos.z()));

  // add cells :
  DD4hep::XML::DetElement cell_det = aXmlElement.child(_Unicode(module));
  DD4hep::XML::Dimension cell_dim(cell_det.dimensions());
  DD4hep::Geometry::Material cell_mat(aLcdd.material(cell_det.materialStr()));
  // assume same no of cells each direction (x,y,z)
  unsigned int numCells = dim.x() / cell_dim.x();
  double offset = -1. * dim.x() + cell_dim.x();
  // create volumes for cell, row and layer
  DD4hep::Geometry::Volume cell_vol("cell_vol", DD4hep::Geometry::Box(cell_dim.x(), cell_dim.y(), cell_dim.z()),
                                    cell_mat);
  DD4hep::Geometry::Volume row_vol("row_vol", DD4hep::Geometry::Box(dim.x(), cell_dim.y(), cell_dim.z()), air);
  DD4hep::Geometry::Volume layer_vol("layer_vol", DD4hep::Geometry::Box(dim.x(), dim.y(), cell_dim.z()), air);
  // attach sensitive detector
  DD4hep::Geometry::SensitiveDetector sd = aSensDet;
  DD4hep::XML::Dimension sd_typ = x_det.child(_U(sensitive));
  sd.setType(sd_typ.typeStr());
  cell_vol.setSensitiveDetector(aSensDet);
  // place cells in a row
  for (uint ix = 0; ix < numCells; ix++) {
    auto cellName = std::string("cell") + DD4hep::XML::_toString(ix, "x%d");
    DD4hep::Geometry::Transform3D transformcell(DD4hep::Geometry::Rotation3D(DD4hep::Geometry::RotationZYX(0, 0, 0)),
                                                DD4hep::Geometry::Position(offset + ix * 2 * cell_dim.x(), 0, 0));
    DD4hep::Geometry::PlacedVolume cell_physvol = row_vol.placeVolume(cell_vol, transformcell);
    cell_physvol.addPhysVolID("modulex", ix);
  }
  // place rows in a layer
  for (uint iy = 0; iy < numCells; iy++) {
    auto cellName = std::string("row") + DD4hep::XML::_toString(iy, "y%d");
    DD4hep::Geometry::Transform3D transformrow(DD4hep::Geometry::Rotation3D(DD4hep::Geometry::RotationZYX(0, 0, 0)),
                                               DD4hep::Geometry::Position(0, offset + iy * 2 * cell_dim.y(), 0));
    DD4hep::Geometry::PlacedVolume row_physvol = layer_vol.placeVolume(row_vol, transformrow);
    row_physvol.addPhysVolID("moduley", iy);
  }
  // place layers in the box volume
  for (uint iz = 0; iz < numCells; iz++) {
    auto cellName = std::string("layer") + DD4hep::XML::_toString(iz, "z%d");
    DD4hep::Geometry::Transform3D transformlayer(DD4hep::Geometry::Rotation3D(DD4hep::Geometry::RotationZYX(0, 0, 0)),
                                                 DD4hep::Geometry::Position(0, 0, offset + iz * 2 * cell_dim.z()));
    DD4hep::Geometry::PlacedVolume layer_physvol = det_vol.placeVolume(layer_vol, transformlayer);
    layer_physvol.addPhysVolID("modulez", iz);
  }

  // place box
  DD4hep::Geometry::PlacedVolume phv = mother.placeVolume(det_vol, transform);
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
