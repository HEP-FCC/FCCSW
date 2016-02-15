#include "DD4hep/DetFactoryHelper.h"

namespace test {
static DD4hep::Geometry::Ref_t createTestBox(DD4hep::Geometry::LCDD& aLcdd,
  DD4hep::XML::Handle_t aXmlElement,
  DD4hep::Geometry::SensitiveDetector aSensDet) {
  DD4hep::XML::DetElement x_det = aXmlElement;
  std::string det_name = x_det.nameStr();
  //Make DetElement
  DD4hep::Geometry::DetElement box(det_name, x_det.id());
  //Make Volume
  DD4hep::XML::Dimension x_det_dim(x_det.dimensions());
  DD4hep::Geometry::Box box_shape(x_det_dim.x(),x_det_dim.y(),x_det_dim.z());
  std::cout<<"Box dim: "<<box_shape.x()<<"\t"<<box_shape.y()<<"\t"<<box_shape.z()<<"\n";
  DD4hep::Geometry::Volume box_vol(det_name,box_shape,aLcdd.material(x_det.materialStr()));
  box_vol.setVisAttributes(aLcdd, x_det_dim.visStr());
  //Place Volume
  DD4hep::Geometry::Volume mother_vol = aLcdd.pickMotherVolume(box);
  DD4hep::Geometry::Position pos(x_det_dim.x0(),x_det_dim.y0(),x_det_dim.z0());
  DD4hep::Geometry::PlacedVolume placedBox = mother_vol.placeVolume(box_vol, pos);
  placedBox.addPhysVolID(det_name,box.id());
  box.setPlacement(placedBox);
  aSensDet.setType("Geant4Calorimeter");
  return box;
}
}

DECLARE_DETELEMENT(TestBox, test::createTestBox)
















