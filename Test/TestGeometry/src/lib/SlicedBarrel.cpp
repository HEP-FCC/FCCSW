#include "DD4hep/DetFactoryHelper.h"

namespace det {
static DD4hep::Geometry::Ref_t createSlicedBarrel(DD4hep::Geometry::LCDD& aLcdd,
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

  // here add cells :
  DD4hep::XML::DetElement cell_det = aXmlElement.child(_Unicode(module));
  std::string cell_name = cell_det.nameStr();
  DD4hep::XML::Dimension slice_dim(cell_det.dimensions());
  DD4hep::Geometry::Material cell_mat(aLcdd.material(cell_det.materialStr()));
  // we assume no segmentation in R/phi direction
  unsigned int numSlices = dim.dz() / slice_dim.dz();
  double offset = -1. * dim.dz() + slice_dim.dz();
  DD4hep::Geometry::Volume slice_vol(
      cell_name + "_vol", DD4hep::Geometry::Tube(slice_dim.rmin(), slice_dim.rmax(), slice_dim.dz()), cell_mat);

  DD4hep::Geometry::SensitiveDetector sd = aSensDet;
  DD4hep::XML::Dimension sd_typ = x_det.child(_U(sensitive));
  sd.setType(sd_typ.typeStr());
  slice_vol.setSensitiveDetector(aSensDet);
  for (uint iz = 0; iz < numSlices; iz++) {
    auto sliceName = cell_name + DD4hep::XML::_toString(iz, "z%d");
    DD4hep::Geometry::Transform3D transformcell(DD4hep::Geometry::Rotation3D(DD4hep::Geometry::RotationZYX(0, 0, 0)),
                                                DD4hep::Geometry::Position(0, 0, offset + iz * 2 * slice_dim.dz()));
    DD4hep::Geometry::PlacedVolume slice_physvol = det_vol.placeVolume(slice_vol, transformcell);
    slice_physvol.addPhysVolID("z", iz);
  }
  // place outer box
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
DECLARE_DETELEMENT(SlicedBarrel, det::createSlicedBarrel)
