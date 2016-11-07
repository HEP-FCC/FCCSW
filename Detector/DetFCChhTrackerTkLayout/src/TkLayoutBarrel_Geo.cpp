
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
  // sensitive detector used for all sensitive parts of this detector
  sensDet.setType(sdTyp.typeStr());
  // definition of top volume
  // has min/max dimensions of tracker for visualization etc.
  std::string detectorName = xmlDet.nameStr();
  DetElement topDetElement(detectorName, xmlDet.id());
  DD4hep::Geometry::Tube topVolumeShape(
      dimensions.rmin(), dimensions.rmax(), (dimensions.zmax() - dimensions.zmin()) * 0.5);
  Volume topVolume(detectorName, topVolumeShape, lcdd.air());
  topVolume.setVisAttributes(lcdd.invisible());

  // counts all layers - incremented in the inner loop over repeat - tags
  unsigned int layerCounter = 0;
  double integratedModuleComponentThickness = 0;
  unsigned int moduleComponentCounter = 0;
  unsigned int nPhi;
  double phi = 0;
  // loop over 'layer' nodes in xml
  DD4hep::XML::Component xLayers = xmlElement.child("layers");
  for (DD4hep::XML::Collection_t xLayerColl(xLayers, _U(layer)); nullptr != xLayerColl; ++xLayerColl) {
    DD4hep::XML::Component xLayer = static_cast<DD4hep::XML::Component>(xLayerColl);
    DD4hep::XML::Component xRods = xLayer.child("rods");
    DD4hep::XML::Component xRodEven = xRods.child("rodOdd");
    DD4hep::XML::Component xRodOdd = xRods.child("rodEven");
    DD4hep::XML::Component xModulesEven = xRodEven.child("modules");
    DD4hep::XML::Component xModulePropertiesOdd = xRodOdd.child("moduleProperties");
    DD4hep::XML::Component xModulesOdd = xRodOdd.child("modules");
    Volume moduleVolume("module",
                        DD4hep::Geometry::Box(0.5 * xModulePropertiesOdd.attr<double>("modWidth"),
                                              0.5 * xModulePropertiesOdd.attr<double>("modThickness"),
                                              0.5 * xModulePropertiesOdd.attr<double>("modLength")),
                        lcdd.material("Air"));
    // moduleVolume.setVisAttributes(lcdd, xModulePropertiesOdd.visStr());
    DD4hep::XML::Component xModuleComponentsOdd = xModulePropertiesOdd.child("components");
    integratedModuleComponentThickness = 0;
    moduleComponentCounter = 0;
    for (DD4hep::XML::Collection_t xModuleComponentOddColl(xModuleComponentsOdd, _U(component));
         nullptr != xModuleComponentOddColl;
         ++xModuleComponentOddColl) {
      DD4hep::XML::Component xModuleComponentOdd = static_cast<DD4hep::XML::Component>(xModuleComponentOddColl);
      Volume moduleComponentVolume(xModuleComponentOdd.nameStr(),
                                   DD4hep::Geometry::Box(0.5 * xModulePropertiesOdd.attr<double>("modWidth"),
                                                         0.5 * xModuleComponentOdd.thickness(),
                                                         0.5 * xModulePropertiesOdd.attr<double>("modLength")),
                                   lcdd.material(xModuleComponentOdd.materialStr()));
      DD4hep::Geometry::Position offset(
          0, integratedModuleComponentThickness - 0.5 * xModulePropertiesOdd.attr<double>("modThickness"), 0);
      integratedModuleComponentThickness += xModuleComponentOdd.thickness();

      moduleComponentVolume.setSensitiveDetector(sensDet);
      PlacedVolume placedModuleComponentVolume = moduleVolume.placeVolume(moduleComponentVolume, offset);
      placedModuleComponentVolume.addPhysVolID("component", moduleComponentCounter);
      ++moduleComponentCounter;
    }
    // definition of layer volumes
    double lX, lY, lZ;
    DD4hep::Geometry::Tube layerShape(xLayer.rmin(), xLayer.rmax(), dimensions.zmax());
    Volume layerVolume("layer", layerShape, lcdd.material("Air"));
    layerVolume.setVisAttributes(lcdd.invisible());
    PlacedVolume placedLayerVolume = topVolume.placeVolume(layerVolume);
    placedLayerVolume.addPhysVolID("layer", layerCounter);
    nPhi = xRods.repeat();
    int moduleCounter = 0;
    DD4hep::XML::Handle_t currentComp;
    for (unsigned int phiIndex = 0; phiIndex < nPhi; ++phiIndex) {
      if (0 == phiIndex % 2) {
        phi = 2 * M_PI * static_cast<double>(phiIndex) / static_cast<double>(nPhi);
        currentComp = xModulesEven;
      } else {
        currentComp = xModulesOdd;
      }
      for (DD4hep::XML::Collection_t xModuleColl(currentComp, _U(module)); nullptr != xModuleColl; ++xModuleColl) {
        DD4hep::XML::Component xModule = static_cast<DD4hep::XML::Component>(xModuleColl);
        lX = xModule.X();
        lY = xModule.Y();
        lZ = xModule.Z();
        DD4hep::Geometry::Translation3D moduleOffset(lX, lY, lZ);
        DD4hep::Geometry::Transform3D lTrafo(DD4hep::Geometry::RotationZ(atan(lY / lX) + 0.5 * M_PI), moduleOffset);
        DD4hep::Geometry::RotationZ lRotation(phi);
        PlacedVolume placedModuleVolume = layerVolume.placeVolume(moduleVolume, lRotation * lTrafo);
        placedModuleVolume.addPhysVolID("module", moduleCounter);
        ++moduleCounter;
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
