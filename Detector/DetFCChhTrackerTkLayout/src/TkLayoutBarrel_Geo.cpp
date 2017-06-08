
#include "DetCommon/DetUtils.h"


#include "ACTS/Plugins/DD4hepPlugins/ActsExtension.hpp"
#include "ACTS/Plugins/DD4hepPlugins/IActsExtension.hpp"


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
  DD4hep::XML::Dimension sdTyp = xmlElement.child(_Unicode(sensitive));
  // sensitive detector used for all sensitive parts of this detector
  sensDet.setType(sdTyp.typeStr());

  // definition of top volume
  // has min/max dimensions of tracker for visualization etc.
  std::string detectorName = xmlDet.nameStr();
  DetElement topDetElement(detectorName, xmlDet.id());
  Acts::ActsExtension::Config barrelConfig;
  barrelConfig.isBarrel = true;
  // detElement owns extension
  Acts::ActsExtension* detWorldExt = new Acts::ActsExtension(barrelConfig);
  topDetElement.addExtension<Acts::IActsExtension>(detWorldExt);
  DD4hep::Geometry::Tube topVolumeShape(
      dimensions.rmin(), dimensions.rmax(), (dimensions.zmax() - dimensions.zmin()) * 0.5);
  Volume topVolume(detectorName, topVolumeShape, lcdd.air());
  topVolume.setVisAttributes(lcdd.invisible());

  // counts all layers - incremented in the inner loop over repeat - tags
  unsigned int layerCounter = 0;
  double integratedModuleComponentThickness = 0;
  double phi = 0;
  // loop over 'layer' nodes in xml
  DD4hep::XML::Component xLayers = xmlElement.child(_Unicode(layers));
  for (DD4hep::XML::Collection_t xLayerColl(xLayers, _U(layer)); nullptr != xLayerColl; ++xLayerColl) {
    DD4hep::XML::Component xLayer = static_cast<DD4hep::XML::Component>(xLayerColl);
    DD4hep::XML::Component xRods = xLayer.child("rods");
    DD4hep::XML::Component xRodEven = xRods.child("rodOdd");
    DD4hep::XML::Component xRodOdd = xRods.child("rodEven");
    DD4hep::XML::Component xModulesEven = xRodEven.child("modules");
    DD4hep::XML::Component xModulePropertiesOdd = xRodOdd.child("moduleProperties");
    DD4hep::XML::Component xModulesOdd = xRodOdd.child("modules");
    DD4hep::Geometry::Tube layerShape(xLayer.rmin(), xLayer.rmax(), dimensions.zmax());
    Volume layerVolume("layer", layerShape, lcdd.material("Air"));
    layerVolume.setVisAttributes(lcdd.invisible());
    PlacedVolume placedLayerVolume = topVolume.placeVolume(layerVolume);
    placedLayerVolume.addPhysVolID("layer", layerCounter);
    DetElement lay_det(topDetElement, "layer" + std::to_string(layerCounter), layerCounter);
    Acts::ActsExtension::Config layConfig;
    layConfig.isLayer = true;
    // the local coordinate systems of modules in dd4hep and acts differ
    // see http://acts.web.cern.ch/ACTS/latest/doc/group__DD4hepPlugins.html
    layConfig.axes = "XzY"; // correct translation of local x axis in dd4hep to local x axis in acts
    // detElement owns extension
    Acts::ActsExtension* layerExtension = new Acts::ActsExtension(layConfig);
    lay_det.addExtension<Acts::IActsExtension>(layerExtension);
    lay_det.setPlacement(placedLayerVolume);
    DD4hep::XML::Component xModuleComponentsOdd = xModulePropertiesOdd.child("components");
    integratedModuleComponentThickness = 0;
    int moduleCounter = 0;
    Volume moduleVolume;
    for (DD4hep::XML::Collection_t xModuleComponentOddColl(xModuleComponentsOdd, _U(component));
         nullptr != xModuleComponentOddColl;
         ++xModuleComponentOddColl) {
      DD4hep::XML::Component xModuleComponentOdd = static_cast<DD4hep::XML::Component>(xModuleComponentOddColl);
      moduleVolume = Volume("module",
                            DD4hep::Geometry::Box(0.5 * xModulePropertiesOdd.attr<double>("modWidth"),
                                                  0.5 * xModuleComponentOdd.thickness(),
                                                  0.5 * xModulePropertiesOdd.attr<double>("modLength")),
                            lcdd.material(xModuleComponentOdd.materialStr()));
      unsigned int nPhi = xRods.repeat();
      DD4hep::XML::Handle_t currentComp;
      for (unsigned int phiIndex = 0; phiIndex < nPhi; ++phiIndex) {
        double lX = 0;
        double lY = 0;
        double lZ = 0;
        if (0 == phiIndex % 2) {
          phi = 2 * M_PI * static_cast<double>(phiIndex) / static_cast<double>(nPhi);
          currentComp = xModulesEven;
        } else {
          currentComp = xModulesOdd;
        }
        for (DD4hep::XML::Collection_t xModuleColl(currentComp, _U(module)); nullptr != xModuleColl; ++xModuleColl) {
          DD4hep::XML::Component xModule = static_cast<DD4hep::XML::Component>(xModuleColl);
          double currentPhi = atan2(xModule.Y(), xModule.X());
          double componentOffset =  integratedModuleComponentThickness - 0.5 * xModulePropertiesOdd.attr<double>("modThickness") + 0.5 * xModuleComponentOdd.thickness();
          lX = xModule.X() + cos(currentPhi) * componentOffset;
          lY = xModule.Y() + sin(currentPhi) * componentOffset;
          lZ = xModule.Z();
          DD4hep::Geometry::Translation3D moduleOffset(lX, lY, lZ);
          DD4hep::Geometry::Transform3D lTrafo(DD4hep::Geometry::RotationZ(atan2(lY,  lX) + 0.5 * M_PI), moduleOffset);
          DD4hep::Geometry::RotationZ lRotation(phi);
          PlacedVolume placedModuleVolume = layerVolume.placeVolume(moduleVolume, lRotation * lTrafo);
          if (xModuleComponentOdd.isSensitive()) {
            placedModuleVolume.addPhysVolID("module", moduleCounter);
            moduleVolume.setSensitiveDetector(sensDet);
            DetElement mod_det(lay_det, "module" + std::to_string(moduleCounter), moduleCounter);
            mod_det.setPlacement(placedModuleVolume);
            ++moduleCounter;
          }
        }
      }
      integratedModuleComponentThickness += xModuleComponentOdd.thickness();
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
