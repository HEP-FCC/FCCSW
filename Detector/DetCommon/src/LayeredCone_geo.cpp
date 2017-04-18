#include "DD4hep/DetFactoryHelper.h"
#include "XML/XMLElements.h"

namespace det {
/**
Factory for a shape from multiple cylinders. Meant for material approximations.
Expected xml structure:
<detector type="LayeredCylinder" ...>
  <dimensions rmin="..." rmax="..." dz="..." z_offset="..."> <!-- envelope -->
  <layer rmin="..." rmax="..." dz="..." z_offset="..." material="...">
  ...
  <layer rmin="..." rmax="..." dz="..." z_offset="..." material="...">
</detector>
@author: Joschka Lingemann
*/
static DD4hep::Geometry::Ref_t createLayeredCone(DD4hep::Geometry::LCDD& lcdd,
                                                 DD4hep::XML::Handle_t xmlElement,
                                                 DD4hep::Geometry::SensitiveDetector /*sensDet*/) {
  DD4hep::XML::DetElement xmlDet = static_cast<DD4hep::XML::DetElement>(xmlElement);
  std::string name = xmlDet.nameStr();
  DD4hep::Geometry::DetElement detElement(name, xmlDet.id());
  DD4hep::Geometry::Volume experimentalHall = lcdd.pickMotherVolume(detElement);
  xml_comp_t dimensions(xmlDet.dimensions());
  DD4hep::Geometry::Cone envelope(
      dimensions.dz(), dimensions.rmin1(), dimensions.rmax1(), dimensions.rmin2(), dimensions.rmax2());
  DD4hep::Geometry::Volume envVolume(name, envelope, lcdd.material(dimensions.materialStr()));
  envVolume.setVisAttributes(lcdd.invisible());
  // Create layer cylinders with their respective material, etc
  auto layers = xmlElement.children(_Unicode(layer));
  auto numLayers = xmlElement.numChildren(_Unicode(layer), true);
  // Joschka: Although this is awkward, it was the only way to loop through children I could find
  DD4hep::XML::Handle_t layer(layers.reset());
  for (unsigned layerIdx = 0; layerIdx < numLayers; ++layerIdx) {
    DD4hep::XML::DetElement layerDet = static_cast<DD4hep::XML::DetElement>(layer);
    DD4hep::Geometry::Cone layerShape(
        layerDet.dz(), layerDet.rmin1(), layerDet.rmax1(), layerDet.rmin2(), layerDet.rmax2());
    std::string layerName = DD4hep::XML::_toString(layerIdx, "layer%d");
    DD4hep::Geometry::Volume layerVolume(layerName, layerShape, lcdd.material(layer.attr<std::string>("material")));
    DD4hep::Geometry::Position transLayer(0., 0., layerDet.z_offset());
    envVolume.placeVolume(layerVolume, DD4hep::Geometry::Transform3D(DD4hep::Geometry::RotationZ(0.), transLayer));
    if (layer.hasAttr("vis")) {
      layerVolume.setVisAttributes(lcdd, layerDet.visStr());
    }
    layer.m_node = layers.next();
  }

  DD4hep::Geometry::Position trans(0., 0., dimensions.z_offset());
  DD4hep::Geometry::PlacedVolume envPhys =
      experimentalHall.placeVolume(envVolume, DD4hep::Geometry::Transform3D(DD4hep::Geometry::RotationZ(0.), trans));
  envPhys.addPhysVolID("system", xmlDet.id());
  detElement.setPlacement(envPhys);
  detElement.setVisAttributes(lcdd, xmlDet.visStr(), envVolume);

  return detElement;
}
}

DECLARE_DETELEMENT(LayeredCone, det::createLayeredCone)
