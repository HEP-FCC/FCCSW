
#include "DD4hep/DetFactoryHelper.h"

using dd4hep::Volume;
using dd4hep::DetElement;
using dd4hep::xml::Dimension;
using dd4hep::PlacedVolume;

namespace det {

static dd4hep::Ref_t createTrackerSimpleBarrel(dd4hep::Detector& lcdd,
                                                 dd4hep::xml::Handle_t xmlElement,
                                                 dd4hep::SensitiveDetector sensDet) {
  // shorthands
  dd4hep::xml::DetElement xmlDet = static_cast<dd4hep::xml::DetElement>(xmlElement);
  dd4hep::xml::Dimension dimensions(xmlDet.dimensions());
  // get sensitive detector type from xml
  dd4hep::xml::Dimension sdTyp = xmlElement.child(_Unicode(sensitive));
  // sensitive detector used for all sensitive parts of this detector
  sensDet.setType(sdTyp.typeStr());

  // definition of top volume
  // has min/max dimensions of tracker for visualization etc.
  std::string detectorName = xmlDet.nameStr();
  DetElement topDetElement(detectorName, xmlDet.id());
  dd4hep::Tube topVolumeShape(dimensions.rmin(), dimensions.rmax(), (dimensions.zmax() - dimensions.zmin()) * 0.5);
  Volume topVolume(detectorName, topVolumeShape, lcdd.air());
  topVolume.setVisAttributes(lcdd, xmlDet.visStr());

  // counts all layers - incremented in the inner loop over repeat - tags
  // loop over 'layer' nodes in xml
  // <layers> //<-xLayers 
  //  <layer/>
  //  <layer/> //<- xLayer
  //  <layer/>
  //    ...
  // <layers>
  unsigned int layerCounter = 0;
  dd4hep::xml::Component xBarrelLayers = xmlElement.child(_Unicode(barrel_layers));
  dd4hep::xml::Component xEndcapLayers = xmlElement.child(_Unicode(endcap_layers));
  auto tgeo_elem1 = TGeoElement("Element1", "a", 10, 10);
  auto tgeo_elem2 = TGeoElement("Element2", "a", 10, 10);
  auto tgeo_mat = new TGeoMixture("mix", 2);
  tgeo_mat->AddElement(10, 10, 0.35);
  tgeo_mat->AddElement(10, 10, 0.65);
  auto tgeo_med = new TGeoMedium("tgeo_med", 1,(TGeoMaterial*)tgeo_mat);
  dd4hep::Material mat = dd4hep::Material(tgeo_med);


  for (dd4hep::xml::Collection_t xLayerColl(xBarrelLayers, _U(layer)); nullptr != xLayerColl; ++xLayerColl) {
    dd4hep::xml::Component xLayer = static_cast<dd4hep::xml::Component>(xLayerColl);
    dd4hep::Tube layerShape(
      xLayer.radius() - xLayer.thickness() * 0.5, 
      xLayer.radius() + xLayer.thickness() * 0.5,
      xLayer.zhalf());
    Volume layerVolume("layer", layerShape, lcdd.material(xBarrelLayers.materialStr())); //lcdd.material("Air"));
    layerVolume.setSensitiveDetector(sensDet);
    layerVolume.setVisAttributes(lcdd, xBarrelLayers.visStr());
    PlacedVolume placedLayerVolume = topVolume.placeVolume(layerVolume);
    placedLayerVolume.addPhysVolID("layer", layerCounter);
    DetElement det_layer(topDetElement, "layer" + std::to_string(layerCounter), layerCounter);
    layerCounter++;
  }

  //unsigned int layerCounter = 0;
  for (dd4hep::xml::Collection_t xLayerColl(xEndcapLayers, _U(layer)); nullptr != xLayerColl; ++xLayerColl) {
    dd4hep::xml::Component xLayer = static_cast<dd4hep::xml::Component>(xLayerColl);
    dd4hep::Tube layerShape(
      xLayer.inner_radius(),
      xLayer.outer_radius(),
      xLayer.thickness());
    Volume layerVolume("layer", layerShape, lcdd.material(xEndcapLayers.materialStr()));
    layerVolume.setSensitiveDetector(sensDet);
    layerVolume.setVisAttributes(lcdd, xBarrelLayers.visStr());
    dd4hep::Position lTranslation(0, 0, xLayer.zhalf());
    dd4hep::Position lTranslationNeg(0, 0, -1* xLayer.zhalf());
    PlacedVolume placedLayerVolume = topVolume.placeVolume(layerVolume, lTranslation);
    PlacedVolume placedLayerVolumeNeg = topVolume.placeVolume(layerVolume, lTranslationNeg);
    placedLayerVolume.addPhysVolID("layer", layerCounter++);
    DetElement det_layer(topDetElement, "FCCTrackerLayer" + std::to_string(layerCounter), layerCounter);
    placedLayerVolume.addPhysVolID("layer", layerCounter++);
    DetElement det_layerNeg(topDetElement, "FCCTrackerLayer" + std::to_string(layerCounter), layerCounter);
  }

  Volume motherVol = lcdd.pickMotherVolume(topDetElement);
  PlacedVolume placedGenericTrackerBarrel = motherVol.placeVolume(topVolume);
  placedGenericTrackerBarrel.addPhysVolID("system", topDetElement.id());
  topDetElement.setPlacement(placedGenericTrackerBarrel);
  return topDetElement;
}
}  // namespace det

DECLARE_DETELEMENT(FCCTrackerSimpleBarrel, det::createTrackerSimpleBarrel)
