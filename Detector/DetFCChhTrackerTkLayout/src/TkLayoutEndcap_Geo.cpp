
#include "DD4hep/DetFactoryHelper.h"
#include "DetCommon/DetUtils.h"

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
  double l_overlapMargin = 0.01;

  // get sensitive detector type from xml
  DD4hep::XML::Dimension sdTyp = xmlElement.child(_Unicode(sensitive)); // retrieve the type
  sensDet.setType(sdTyp.typeStr()); // set for the whole detector

  // definition of top volume
  std::string detName = xmlDet.nameStr();
  DetElement worldDetElement(detName, xmlDet.id());
  DetElement posEcapDetElement(worldDetElement, "posEndcap", 0);

  Acts::ActsExtension::Config actsEcapConfig;
  actsEcapConfig.isEndcap = true;
  Acts::ActsExtension* ecapDetExt = new Acts::ActsExtension(actsEcapConfig);
  posEcapDetElement.addExtension<Acts::IActsExtension>(ecapDetExt);


  DD4hep::Geometry::Assembly envelopeVolume("endcapEnvelope");
  envelopeVolume.setVisAttributes(lcdd.invisible());

  Component xDiscs = xmlElement.child(_Unicode(discs));

  double envelopeThickness = 0.5 * (dimensions.zmax() - dimensions.zmin());

  l_overlapMargin *= 0.9;

  unsigned int discCounter = 0;
  unsigned int compCounter = 0;
  double currentZ;
  std::vector<Volume> discVolumeVec;
  std::vector<DetElement> discDetElementVec;
  /// iterate over discs
  for (DD4hep::XML::Collection_t xDiscColl(xDiscs, _Unicode(discZPls)); nullptr != xDiscColl; ++xDiscColl) {
    Component xDisc = static_cast<Component>(xDiscColl);
    Component xCurrentRings = xDisc.child(_Unicode(rings));
    // create disc volume
    double discThickness = 0.5 * (xDisc.zmax() - xDisc.zmin());
    currentZ = xDisc.z() - dimensions.zmin() - envelopeThickness;
    if(xCurrentRings.hasChild(_Unicode(ring))) { // we have information to construct a new volume
      DD4hep::Geometry::Tube discShape(
          xDisc.rmin() - l_overlapMargin, xDisc.rmax() + l_overlapMargin, discThickness + l_overlapMargin);

      discVolumeVec.emplace_back("disc", discShape, lcdd.air());
      discDetElementVec.emplace_back(posEcapDetElement, "disc" + std::to_string(discCounter), discCounter);
      Acts::ActsExtension::Config layConfig;
      // the local coordinate systems of modules in dd4hep and acts differ
      // see http://acts.web.cern.ch/ACTS/latest/doc/group__DD4hepPlugins.html
      layConfig.axes = "XZY";  // correct translation of local x axis in dd4hep to local x axis in acts
      layConfig.isLayer = true;
      Acts::ActsExtension* detlayer = new Acts::ActsExtension(layConfig);
      discDetElementVec.back().addExtension<Acts::IActsExtension>(detlayer);
      // iterate over rings
      for (DD4hep::XML::Collection_t xRingColl(xCurrentRings, _U(ring)); (nullptr != xRingColl); ++xRingColl) {
        Component xRing = static_cast<Component>(xRingColl);
        Component xRingModules = xRing.child(_Unicode(modules));
        Component xModuleOdd = xRingModules.child(_Unicode(moduleOdd));
        Component xModuleEven = xRingModules.child(_Unicode(moduleEven));
        Component xModuleProperties = xRing.child(_Unicode(moduleProperties));
        Component xModulePropertiesComp = xModuleProperties.child(_Unicode(components));
        Component xSensorProperties = xRing.child(_Unicode(sensorProperties));

        // place components in module
        double integratedCompThickness = 0.;
        for (DD4hep::XML::Collection_t xCompColl(xModulePropertiesComp, _U(component)); nullptr != xCompColl;
             ++xCompColl) {
          Component xComp = static_cast<Component>(xCompColl);
          Volume componentVolume("component",
                                 DD4hep::Geometry::Trapezoid(0.5 * xModuleProperties.attr<double>("modWidthMin"),
                                                             0.5 * xModuleProperties.attr<double>("modWidthMax"),
                                                             0.5 * xComp.thickness(),
                                                             0.5 * xComp.thickness(),
                                                             0.5 * xSensorProperties.attr<double>("sensorLength")),
                                 lcdd.material(xComp.materialStr()));
          componentVolume.setVisAttributes(lcdd.invisible());
          unsigned int nPhi = xRing.attr<int>("nModules");
          double phi = 0;
          for (unsigned int phiIndex = 0; phiIndex < nPhi; ++phiIndex) {
            double lX = 0;
            double lY = 0;
            double lZ = 0;
            double phiTilt = 0;
            double thetaTilt = 0;
            if (0 == phiIndex % 2) {
              // the rotation for the odd module is already taken care
              // of by the position in tklayout xml
              phi = 2 * dd4hep::pi * static_cast<double>(phiIndex) / static_cast<double>(nPhi);
              lX = xModuleEven.X();
              lY = xModuleEven.Y();
              lZ = xModuleEven.Z() - xDisc.zmin() - discThickness;
              phiTilt = xModuleEven.attr<double>("phiTilt");
              thetaTilt = xModuleEven.attr<double>("thetaTilt");
            } else {
              lX = xModuleOdd.X();
              lY = xModuleOdd.Y();
              lZ = xModuleOdd.Z() - xDisc.zmin() - discThickness;
              phiTilt = xModuleOdd.attr<double>("phiTilt");
              thetaTilt = xModuleOdd.attr<double>("thetaTilt");
            }
            // position module in the x-y plane, smaller end inward
            // and incorporate phi tilt if any
            DD4hep::Geometry::RotationY lRotation1(M_PI * 0.5);
            DD4hep::Geometry::RotationX lRotation2(M_PI * 0.5 + phiTilt);
            // align radially
            double componentOffset =
                integratedCompThickness - 0.5 * xModuleProperties.attr<double>("modThickness") + 0.5 * xComp.thickness();
            DD4hep::Geometry::RotationZ lRotation3(atan2(lY, lX));
            // theta tilt, if any -- note the different convention between
            // tklayout and here, thus the subtraction of pi / 2
            DD4hep::Geometry::RotationY lRotation4(thetaTilt - M_PI * 0.5);
            DD4hep::Geometry::RotationZ lRotation_PhiPos(phi);
            // position in  disk
            DD4hep::Geometry::Translation3D lTranslation(lX, lY, lZ + componentOffset);
            DD4hep::Geometry::Transform3D myTrafo(lRotation4 * lRotation3 * lRotation2 * lRotation1, lTranslation);
            PlacedVolume placedComponentVolume = discVolumeVec.back().placeVolume(componentVolume, lRotation_PhiPos * myTrafo);
            if (xComp.isSensitive()) {
              placedComponentVolume.addPhysVolID("component", compCounter);
              componentVolume.setSensitiveDetector(sensDet);
              DetElement moduleDetElement(discDetElementVec.back(), "comp" + std::to_string(compCounter), compCounter);
              moduleDetElement.setPlacement(placedComponentVolume);
              ++compCounter;
            }
          }
          integratedCompThickness += xComp.thickness();
        }
      }
    } else {
      discDetElementVec.emplace_back(discDetElementVec.back().clone("disc" + std::to_string(discCounter)));
      posEcapDetElement.add(discDetElementVec.back());

    }
    PlacedVolume placedDiscVolume = envelopeVolume.placeVolume(discVolumeVec.back(), DD4hep::Geometry::Position(0, 0, currentZ));
    placedDiscVolume.addPhysVolID("disc", discCounter);
    ++discCounter;

    discDetElementVec.back().setPlacement(placedDiscVolume);
  }

  DD4hep::Geometry::Assembly bothEndcaps("bothEndcapsEnvelope");

  DD4hep::Geometry::Translation3D envelopeTranslation(0, 0, dimensions.zmin() + envelopeThickness);

  DD4hep::Geometry::RotationX envelopeNegRotation(dd4hep::pi);
  DD4hep::Geometry::RotationX envelopePosRotation(0.);
  PlacedVolume placedEnvelopeVolume =
      bothEndcaps.placeVolume(envelopeVolume, envelopePosRotation * envelopeTranslation);
  PlacedVolume placedNegEnvelopeVolume =
      bothEndcaps.placeVolume(envelopeVolume, envelopeNegRotation * envelopeTranslation);
  placedEnvelopeVolume.addPhysVolID("posneg", 0);
  placedNegEnvelopeVolume.addPhysVolID("posneg", 1);
  auto negEcapDetElement = posEcapDetElement.clone("negEndcap");
  
  posEcapDetElement.setPlacement(placedEnvelopeVolume);
  negEcapDetElement.setPlacement(placedNegEnvelopeVolume);
  worldDetElement.add(negEcapDetElement);
  // top of the hierarchy
  PlacedVolume mplv = lcdd.pickMotherVolume(worldDetElement).placeVolume(bothEndcaps);
  worldDetElement.setPlacement(mplv);
  mplv.addPhysVolID("system", xmlDet.id());
  return worldDetElement;
}
}  // namespace det

DECLARE_DETELEMENT(TkLayoutEcapTracker, det::createTkLayoutTrackerEndcap)
