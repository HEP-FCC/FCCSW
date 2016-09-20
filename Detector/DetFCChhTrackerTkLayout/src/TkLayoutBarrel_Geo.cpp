
#include "DetCommon/DetUtils.h"

#include "DD4hep/DetFactoryHelper.h"

using DD4hep::Geometry::Volume;
using DD4hep::Geometry::DetElement;
using DD4hep::XML::Dimension;
using DD4hep::Geometry::PlacedVolume;

namespace det {

static DD4hep::Geometry::Ref_t createTkLayoutTrackerBarrel(DD4hep::Geometry::LCDD& lcdd,
                                                          DD4hep::XML::Handle_t xmlElement,
                                                          DD4hep::Geometry::SensitiveDetector sensDet) {
  // shorthands
  DD4hep::XML::DetElement xmlDet = static_cast<DD4hep::XML::DetElement>(xmlElement);
  Dimension dimensions(xmlDet.dimensions());
  // get sensitive detector type from xml
  DD4hep::XML::Dimension sdTyp = xmlElement.child("sensitive");
  if (xmlDet.isSensitive()) {
    // sensitive detector used for all sensitive parts of this detector
    sensDet.setType(sdTyp.typeStr());
  }
  // definition of top volume
  // has min/max dimensions of tracker for visualization etc.
  std::string detectorName = xmlDet.nameStr();
  DetElement topDetElement(detectorName, xmlDet.id());
  DD4hep::Geometry::Tube topVolumeShape(dimensions.rmin(), dimensions.rmax(), (dimensions.zmax() - dimensions.zmin()) * 0.5);
  Volume topVolume(detectorName, topVolumeShape, lcdd.air());
  topVolume.setVisAttributes(lcdd.invisible());

  // counts all layers - incremented in the inner loop over repeat - tags
  unsigned int layerCounter = 0;
  double integratedModuleComponentThickness = 0;
  unsigned int moduleComponentCounter = 0;
  unsigned int nPhi;
  double phi;
  // loop over 'layer' nodes in xml
  DD4hep::XML::Component xLayers = xmlElement.child("layers");
  for (DD4hep::XML::Collection_t xLayerColl(xLayers, _U(layer)); nullptr != xLayerColl; ++xLayerColl) {
    DD4hep::XML::Component xLayer = static_cast<DD4hep::XML::Component>(xLayerColl);

    DD4hep::XML::Component xRods = xLayer.child("rods");
    DD4hep::XML::Component xRodEven = xRods.child("rodOdd");
    DD4hep::XML::Component xRodOdd = xRods.child("rodEven");
    DD4hep::XML::Component xModulePropertiesEven = xRodEven.child("moduleProperties");
    DD4hep::XML::Component xModulesEven = xRodEven.child("modules");
    DD4hep::XML::Component xModulePropertiesOdd = xRodOdd.child("moduleProperties");
    DD4hep::XML::Component xModulesOdd = xRodOdd.child("modules");
    // definition of rod volume (longitudinal arrangement of modules)
    Volume rodVolumeOdd("rod",
                     DD4hep::Geometry::Box(
                       xModulePropertiesOdd.attr<double>("modWidth"), 
                       xModulePropertiesOdd.attr<double>("modThickness"),
                       (dimensions.zmax() - dimensions.zmin()) * 0.5),
                     lcdd.material("Air"));
    rodVolumeOdd.setVisAttributes(lcdd.invisible());

    Volume rodVolumeEven("rod",
                     DD4hep::Geometry::Box(
                       xModulePropertiesEven.attr<double>("modWidth"), 
                       xModulePropertiesEven.attr<double>("modThickness"),
                       (dimensions.zmax() - dimensions.zmin()) * 0.5),
                     lcdd.material("Air"));
    rodVolumeEven.setVisAttributes(lcdd.invisible());

    Volume moduleVolumeOdd(
      "module", 
      DD4hep::Geometry::Box(
        xModulePropertiesOdd.attr<double>("modWidth"), 
        xModulePropertiesOdd.attr<double>("modThickness"),
        xModulePropertiesOdd.attr<double>("modLength")), 
      lcdd.material("Air"));
    //moduleVolumeOdd.setVisAttributes(lcdd, xModulePropertiesOdd.visStr());
    DD4hep::XML::Component xModuleComponentsOdd = xModulePropertiesOdd.child("components");
    integratedModuleComponentThickness = 0;
    moduleComponentCounter = 0;
    for (DD4hep::XML::Collection_t xModuleComponentOddColl(xModuleComponentsOdd, _U(component)); nullptr != xModuleComponentOddColl; ++xModuleComponentOddColl) {
      DD4hep::XML::Component xModuleComponentOdd = static_cast<DD4hep::XML::Component>(xModuleComponentOddColl);
      Volume moduleComponentVolume(xModuleComponentOdd.nameStr(),
                                   DD4hep::Geometry::Box(xModulePropertiesOdd.attr<double>("modWidth"), 
                                     xModuleComponentOdd.thickness(),
                                     xModulePropertiesOdd.attr<double>("modLength")),
                                   lcdd.material(xModuleComponentOdd.materialStr()));
    DD4hep::Geometry::Position offset(0, integratedModuleComponentThickness, 0);
    integratedModuleComponentThickness += xModuleComponentOdd.thickness();
    PlacedVolume placedModuleComponentVolume = moduleVolumeOdd.placeVolume(moduleComponentVolume, offset);
    placedModuleComponentVolume.addPhysVolID("component", moduleComponentCounter);
    ++moduleComponentCounter;
    }


    // place modules in rods
    unsigned int moduleCounter = 0;
    for (DD4hep::XML::Collection_t xModuleOddColl(xModulesOdd, _U(module)); nullptr != xModuleOddColl; ++xModuleOddColl) {
      DD4hep::XML::Component xModuleOdd = static_cast<DD4hep::XML::Component>(xModuleOddColl);
      //DD4hep::Geometry::Position moduleOffset(xModuleOdd.X(), xModuleOdd.Y(), xModuleOdd.Z()); 
      DD4hep::Geometry::Position moduleOffset(0, 0, xModuleOdd.Z()); 
      PlacedVolume placedModuleVolume = rodVolumeOdd.placeVolume(moduleVolumeOdd, moduleOffset);
      placedModuleVolume.addPhysVolID("module", moduleCounter);
      ++moduleCounter;
    }

    Volume moduleVolumeEven(
      "module", 
      DD4hep::Geometry::Box(
        xModulePropertiesEven.attr<double>("modWidth"), 
        xModulePropertiesEven.attr<double>("modThickness"),
        xModulePropertiesEven.attr<double>("modLength")), 
      lcdd.material("Air"));
    //moduleVolumeEven.setVisAttributes(lcdd, xModule.visStr());
    DD4hep::XML::Component xModuleComponentsEven = xModulePropertiesEven.child("components");
    integratedModuleComponentThickness = 0;
    moduleComponentCounter = 0;
    for (DD4hep::XML::Collection_t xModuleComponentEvenColl(xModuleComponentsEven, _U(component)); nullptr != xModuleComponentEvenColl; ++xModuleComponentEvenColl) {
      DD4hep::XML::Component xModuleComponentEven = static_cast<DD4hep::XML::Component>(xModuleComponentEvenColl);
      Volume moduleComponentVolume(xModuleComponentEven.nameStr(),
                                   DD4hep::Geometry::Box(xModulePropertiesEven.attr<double>("modWidth"), 
                                     xModuleComponentEven.thickness(),
                                     xModulePropertiesEven.attr<double>("modLength")),
                                   lcdd.material(xModuleComponentEven.materialStr()));
    DD4hep::Geometry::Position offset(0, integratedModuleComponentThickness, 0);
    integratedModuleComponentThickness += xModuleComponentEven.thickness();
    PlacedVolume placedModuleComponentVolume = moduleVolumeEven.placeVolume(moduleComponentVolume, offset);
    placedModuleComponentVolume.addPhysVolID("component", moduleComponentCounter);
    ++moduleComponentCounter;
    }

    // place modules in rods
    moduleCounter = 0;
    for (DD4hep::XML::Collection_t xModuleEvenColl(xModulesEven, _U(module)); nullptr != xModuleEvenColl; ++xModuleEvenColl) {
      DD4hep::XML::Component xModuleEven = static_cast<DD4hep::XML::Component>(xModuleEvenColl);
      //DD4hep::Geometry::Position moduleOffset(xModuleEven.X(), xModuleEven.Y(), xModuleEven.Z()); 
      DD4hep::Geometry::Position moduleOffset(0, 0, xModuleEven.Z()); 
      PlacedVolume placedModuleVolume = rodVolumeEven.placeVolume(moduleVolumeEven, moduleOffset);
      placedModuleVolume.addPhysVolID("module", moduleCounter);
      ++moduleCounter;
    }

      // definition of layer volumes
      double r = xLayer.radius();
      double layerThickness = (dimensions.rmax() - dimensions.rmin()) / xLayers.repeat();
      DD4hep::Geometry::Tube layerShape(r - 0.5*layerThickness, r + 0.5*layerThickness, dimensions.zmax());
      Volume layerVolume("layer", layerShape, lcdd.material("Air"));
      layerVolume.setVisAttributes(lcdd.invisible());
      PlacedVolume placedLayerVolume = topVolume.placeVolume(layerVolume);
      placedLayerVolume.addPhysVolID("layer", layerCounter);
      nPhi = xRods.repeat();
      for (unsigned int phiIndex = 0; phiIndex < nPhi; ++phiIndex) {
        phi = 2 * M_PI * static_cast<double>(phiIndex) / static_cast<double>(nPhi);// + xLayer.attr<double>("phi0");
        DD4hep::Geometry::Translation3D lTranslation(r * cos(phi), r * sin(phi), 0);
        DD4hep::Geometry::RotationZ lRotation(phi + 0.5 * M_PI);
        if (0 == phiIndex % 2) {
          PlacedVolume placedRodVolume = layerVolume.placeVolume(rodVolumeEven, lTranslation * lRotation);
          placedRodVolume.addPhysVolID("rod", phiIndex);
        } else {
          PlacedVolume placedRodVolume = layerVolume.placeVolume(rodVolumeOdd, lTranslation * lRotation);
          placedRodVolume.addPhysVolID("rod", phiIndex);
        }
      }
  ++layerCounter;
  }
  Volume motherVol = lcdd.pickMotherVolume(topDetElement);
  PlacedVolume placedGenericTrackerBarrel = motherVol.placeVolume(topVolume);
  placedGenericTrackerBarrel.addPhysVolID("system", topDetElement.id());
  topDetElement.setPlacement(placedGenericTrackerBarrel);
  return topDetElement;
}
}  // namespace det

DECLARE_DETELEMENT(TkLayoutBrlTracker, det::createTkLayoutTrackerBarrel)
