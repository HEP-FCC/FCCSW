#include "DD4hep/DetFactoryHelper.h"
#include "XML/XMLElements.h"

namespace det {
/**
Factory for a shape from multiple cylinders. Meant for material approximations.
Expected xml structure:
<detector type="SimpleLayeredCylinder" ...>
  <dimensions rmin="..." rmax="..." dz="..." z_offset="..."> <!-- envelope -->
  <layer rmin="..." rmax="..." dz="..." z_offset="..." material="...">
  ...
  <layer rmin="..." rmax="..." dz="..." z_offset="..." material="...">
</detector>
@author: Joschka Lingemann
*/
static dd4hep::Ref_t createSimpleLayeredCylinder(dd4hep::Detector& lcdd,
                                                           dd4hep::xml::Handle_t xmlElement,
                                                           dd4hep::SensitiveDetector /*sensDet*/) {
  dd4hep::xml::DetElement xmlDet = static_cast<dd4hep::xml::DetElement>(xmlElement);
  std::string name = xmlDet.nameStr();
  dd4hep::DetElement detElement(name, xmlDet.id());
  dd4hep::Volume experimentalHall = lcdd.pickMotherVolume(detElement);
  xml_comp_t dimensions(xmlDet.dimensions());
  dd4hep::Tube envelope(dimensions.rmin(), dimensions.rmax(), dimensions.dz());
  dd4hep::Volume envVolume(name, envelope, lcdd.material(dimensions.materialStr()));

  // Create layer cylinders with their respective material, etc
  auto layers = xmlElement.children(_Unicode(layer));
  auto numLayers = xmlElement.numChildren(_Unicode(layer), true);
  // Joschka: Although this is awkward, it was the only way to loop through children I could find
  dd4hep::xml::Handle_t layer(layers.reset());
  for (unsigned layerIdx = 0; layerIdx < numLayers; ++layerIdx) {
    dd4hep::xml::DetElement layerDet = static_cast<dd4hep::xml::DetElement>(layer);
    dd4hep::Tube layerShape(layerDet.rmin(), layerDet.rmax(), layerDet.dz());
    std::string layerName = dd4hep::xml::_toString(layerIdx, "layer%d");
    dd4hep::Volume layerVolume(layerName, layerShape, lcdd.material(layer.attr<std::string>("material")));
    dd4hep::Position transLayer(0., 0., layerDet.z_offset());
    envVolume.placeVolume(layerVolume, dd4hep::Transform3D(dd4hep::RotationZ(0.), transLayer));
    if (layer.hasAttr("vis")) {
      layerVolume.setVisAttributes(lcdd, layerDet.visStr());
    }
    layer.m_node = layers.next();
  }

  dd4hep::Position trans(0., 0., dimensions.z_offset());
  dd4hep::PlacedVolume envPhys =
      experimentalHall.placeVolume(envVolume, dd4hep::Transform3D(dd4hep::RotationZ(0.), trans));
  envPhys.addPhysVolID("system", xmlDet.id());
  detElement.setPlacement(envPhys);
  detElement.setVisAttributes(lcdd, xmlDet.visStr(), envVolume);

  return detElement;
}
}

DECLARE_DETELEMENT(SimpleLayeredCylinder, det::createSimpleLayeredCylinder)
