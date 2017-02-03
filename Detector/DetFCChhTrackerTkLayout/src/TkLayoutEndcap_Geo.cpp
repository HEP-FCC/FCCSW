
#include "DetCommon/DetUtils.h"
#include "DD4hep/DetFactoryHelper.h"

#include "ACTS/Plugins/DD4hepPlugins/ActsExtension.hpp"
#include "ACTS/Plugins/DD4hepPlugins/IActsExtension.hpp"

using DD4hep::Geometry::Volume;
using DD4hep::Geometry::DetElement;
using DD4hep::XML::Dimension;
using DD4hep::XML::Component;
using DD4hep::Geometry::PlacedVolume;

namespace det {
static DD4hep::Geometry::Ref_t createTkLayoutTrackerEndcap(DD4hep::Geometry::LCDD& lcdd,
                                                           DD4hep::XML::Handle_t xmlElement,
                                                           DD4hep::Geometry::SensitiveDetector sensDet) {
  // shorthands
  DD4hep::XML::DetElement xmlDet = static_cast<DD4hep::XML::DetElement>(xmlElement);
  Dimension dimensions(xmlDet.dimensions());

  // get sensitive detector type from xml
  DD4hep::XML::Dimension sdTyp = xmlElement.child("sensitive");  // retrieve the type
  sensDet.setType(sdTyp.typeStr());  // set for the whole detector

  // definition of top volume
  std::string detName = xmlDet.nameStr();
  DetElement worldDetElement(detName, xmlDet.id());
  Acts::ActsExtension::Config actsBarrelConfig;
  actsBarrelConfig.isEndcap             = true;
  Acts::ActsExtension* worldDetExt = new Acts::ActsExtension(actsBarrelConfig);
  worldDetElement.addExtension<Acts::IActsExtension>(worldDetExt);

  // envelope volume for one of the endcaps, either forward or backward
  double envelopeThickness = 0.5 * (dimensions.zmax() - dimensions.zmin());
  DD4hep::Geometry::Tube envelopeShape(
      dimensions.rmin(), dimensions.rmax(), envelopeThickness);
  Volume envelopeVolume(detName, envelopeShape, lcdd.air());
  envelopeVolume.setVisAttributes(lcdd.invisible());

  Component xDiscs = xmlElement.child("discs");
  Component xFirstDisc = xDiscs.child("discZPls");
  Component xFirstDiscRings = xFirstDisc.child("rings");

  // create disc volume
  double discThickness = 0.5 * (xFirstDisc.zmax() - xFirstDisc.zmin());
  DD4hep::Geometry::Tube discShape(dimensions.rmin(), dimensions.rmax(), discThickness);
  Volume discVolume("disc", discShape, lcdd.air());
  discVolume.setVisAttributes(lcdd.invisible());

  int moduleCounter = 0;

  unsigned int discCounter = 0;
  unsigned int compCounter = 0;
  double currentZ;
  for (DD4hep::XML::Collection_t xDiscColl(xDiscs, "discZPls"); nullptr != xDiscColl; ++xDiscColl) {
    Component xDisc = static_cast<Component>(xDiscColl);
    currentZ = xDisc.z() - dimensions.zmin() - envelopeThickness;
    DetElement disc_det(worldDetElement, "disc" + std::to_string(discCounter), discCounter);
    // generate rings and place in  discs
    for (DD4hep::XML::Collection_t xRingColl(xFirstDiscRings, _U(ring)); nullptr != xRingColl; ++xRingColl) {
      Component xRing = static_cast<Component>(xRingColl);
      Component xRingModules = xRing.child("modules");
      Component xModuleOdd = xRingModules.child("moduleOdd");
      Component xModuleEven = xRingModules.child("moduleEven");
      Component xModuleProperties = xRing.child("moduleProperties");
      Component xModulePropertiesComp = xModuleProperties.child("components");
      Component xSensorProperties = xRing.child("sensorProperties");
      Volume moduleVolume("module",
                          DD4hep::Geometry::Trapezoid(0.5 * xModuleProperties.attr<double>("modWidthMin"),
                                                      0.5 * xModuleProperties.attr<double>("modWidthMax"),
                                                      0.5 * xModuleProperties.attr<double>("modThickness"),
                                                      0.5 * xModuleProperties.attr<double>("modThickness"),
                                                      0.5 * xSensorProperties.attr<double>("sensorLength")),
                          lcdd.material("Air"));
      // place components in module
      double integratedCompThickness = 0;
      int componentCounter = 0;
      for (DD4hep::XML::Collection_t xCompColl(xModulePropertiesComp, _U(component)); nullptr != xCompColl; ++xCompColl) {
        Component xComp = static_cast<Component>(xCompColl);
        Volume componentVolume("component",
                               DD4hep::Geometry::Trapezoid(0.5 * xModuleProperties.attr<double>("modWidthMin"),
                                                           0.5 * xModuleProperties.attr<double>("modWidthMax"),
                                                           0.5 * xComp.thickness(),
                                                           0.5 * xComp.thickness(),
                                                           0.5 * xSensorProperties.attr<double>("sensorLength")),
                               lcdd.material(xComp.materialStr()));
        PlacedVolume placedComponentVolume = moduleVolume.placeVolume(
            componentVolume,
            DD4hep::Geometry::Position(
                0, integratedCompThickness - 0.5 * xModuleProperties.attr<double>("modThickness")  + 0.5 * xComp.thickness(), 0));
        placedComponentVolume.addPhysVolID("component", componentCounter);

        if( xComp.isSensitive() ){
          componentVolume.setSensitiveDetector(sensDet);
        }
        DetElement comp_det(disc_det, "comp" + std::to_string(compCounter), compCounter);
        comp_det.setPlacement(placedComponentVolume);
        ++compCounter;
        integratedCompThickness += xComp.thickness();
        ++componentCounter;
      }
      unsigned int nPhi = xRing.attr<int>("nModules");
      double lX, lY, lZ;
      double phi = 0;
      double phiTilt, thetaTilt;
      for (unsigned int phiIndex = 0; phiIndex < nPhi; ++phiIndex) {
        if (0 == phiIndex % 2) {
          // the rotation for the odd module is already taken care
          // of by the position in tklayout xml
          phi = 2 * dd4hep::pi * static_cast<double>(phiIndex) / static_cast<double>(nPhi);
          lX = xModuleEven.X();
          lY = xModuleEven.Y();
          lZ = xModuleEven.Z() - dimensions.zmin() - discThickness;
          phiTilt = xModuleEven.attr<double>("phiTilt");
          thetaTilt = xModuleEven.attr<double>("thetaTilt");
        } else {
          lX = xModuleOdd.X();
          lY = xModuleOdd.Y();
          lZ = xModuleOdd.Z() - dimensions.zmin() - discThickness;
          phiTilt = xModuleOdd.attr<double>("phiTilt");
          thetaTilt = xModuleOdd.attr<double>("thetaTilt");
        }
        // position module in the x-y plane, smaller end inward
        // and incorporate phi tilt if any
        DD4hep::Geometry::RotationY lRotation1(M_PI * 0.5);
        DD4hep::Geometry::RotationX lRotation2(M_PI * 0.5 + phiTilt);
        // align radially
        DD4hep::Geometry::RotationZ lRotation3(atan(lY / lX));
        // theta tilt, if any -- note the different convention between
        // tklayout and here, thus the subtraction of pi / 2
        DD4hep::Geometry::RotationY lRotation4(thetaTilt - M_PI * 0.5);
        DD4hep::Geometry::RotationZ lRotation_PhiPos(phi);
        // position in  disk
        DD4hep::Geometry::Translation3D lTranslation(lX, lY, lZ);
        DD4hep::Geometry::Transform3D myTrafo(lRotation4 * lRotation3 * lRotation2 * lRotation1, lTranslation);
        PlacedVolume placedModuleVolume = discVolume.placeVolume(moduleVolume, lRotation_PhiPos * myTrafo);
        placedModuleVolume.addPhysVolID("module", moduleCounter);
        DetElement mod_det("module" + std::to_string(moduleCounter), moduleCounter);
        mod_det.setPlacement(placedModuleVolume);
        disc_det.add(mod_det);
        ++moduleCounter;
      }
    }
  PlacedVolume placedDiscVolume = envelopeVolume.placeVolume(discVolume, DD4hep::Geometry::Position(0, 0, currentZ));
  placedDiscVolume.addPhysVolID("disc", discCounter);
  Acts::ActsExtension::Config layConfig;
  layConfig.axes = "XzY";
  layConfig.isLayer             = true;
  Acts::ActsExtension* detlayer = new Acts::ActsExtension(layConfig);
  disc_det.addExtension<Acts::IActsExtension>(detlayer);
  disc_det.setPlacement(placedDiscVolume);
  ++discCounter;
  }

  // top of the hierarchy
  Volume motherVol = lcdd.pickMotherVolume(worldDetElement);
  DD4hep::Geometry::Translation3D envelopeTranslation(0, 0, dimensions.zmin() + envelopeThickness);
  double envelopeNegRotAngle = 0;
  if (dimensions.reflect()) {
    envelopeNegRotAngle = dd4hep::pi;
  }
  DD4hep::Geometry::RotationX envelopeNegRotation(envelopeNegRotAngle);
  PlacedVolume placedEnvelopeVolume = motherVol.placeVolume(envelopeVolume, envelopeNegRotation * envelopeTranslation);
  placedEnvelopeVolume.addPhysVolID("system", xmlDet.id());
  worldDetElement.setPlacement(placedEnvelopeVolume);
  return worldDetElement;
}
}  // namespace det

DECLARE_DETELEMENT(TkLayoutEcapTracker, det::createTkLayoutTrackerEndcap)
