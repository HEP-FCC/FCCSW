
#include "DetCommon/DetUtils.h"
#include "DD4hep/DetFactoryHelper.h"

using DD4hep::Geometry::Volume;
using DD4hep::Geometry::DetElement;
using DD4hep::XML::Dimension;
using DD4hep::XML::Component;
using DD4hep::Geometry::PlacedVolume;

namespace det {
static DD4hep::Geometry::Ref_t createTkLayoutTrackerEndcap(DD4hep::Geometry::LCDD& lcdd,
                                                          DD4hep::XML::Handle_t xmlElement,
                                                          DD4hep::Geometry::SensitiveDetector sensDet) {
  DD4hep::XML::DetElement xmlDet = static_cast<DD4hep::XML::DetElement>(xmlElement);
  Dimension dimensions(xmlDet.dimensions());

  // get sensitive detector type from xml
  DD4hep::XML::Dimension sdTyp = xmlElement.child("sensitive");  // retrieve the type
  if (xmlDet.isSensitive()) {
    sensDet.setType(sdTyp.typeStr());  // set for the whole detector
  }

  // definition of top volume
  std::string detName = xmlDet.nameStr();
  DetElement GenericTrackerEndcapWorld(detName, xmlDet.id());

  // envelope volume with the max dimensions of tracker for visualization etc.
  // contains both endcaps, in forward and in backwards direction
  // the part between -z1 and z1 is subtracted from the envelope
  DD4hep::Geometry::Tube posnegEnvelopeShape_add(dimensions.rmin(), dimensions.rmax(), dimensions.zmax());
  // make the negative shape slighly larger in the radial direction 
  // to be sure that everything is subtracted between -z1 and z1
  DD4hep::Geometry::Box posnegEnvelopeShape_subtract(
      dimensions.rmax() * 1.001, dimensions.rmax() * 1.001, dimensions.zmin());
  DD4hep::Geometry::SubtractionSolid posnegEnvelopeShape(posnegEnvelopeShape_add, posnegEnvelopeShape_subtract);
  Volume posnegEnvelopeVolume(detName, posnegEnvelopeShape, lcdd.air());
  posnegEnvelopeVolume.setVisAttributes(lcdd.invisible());

  // envelope volume for one of the endcaps, either forward or backward
  DD4hep::Geometry::Tube envelopeShape(dimensions.rmin(), dimensions.rmax(), 0.5 * (dimensions.zmax() - dimensions.zmin()));
  Volume envelopeVolume(detName, envelopeShape, lcdd.air());
  //envelopeVolume.setVisAttributes(lcdd.invisible());

  Component xDiscs = xmlElement.child("discs");
  Component xFirstDisc = xDiscs.child("discZPls");
  Component xFirstDiscRings = xFirstDisc.child("rings");
  

  double discThickness =  (dimensions.zmax() - dimensions.zmin()) / xDiscs.repeat();
  DD4hep::Geometry::Tube discShape(dimensions.rmin(), dimensions.rmax(), 0.5 * discThickness);

  Volume discVolume("disc", discShape, lcdd.air());
  //discVolume.setVisAttributes(lcdd.invisible());
  // generate rings and place in  discs
  int moduleCounter = 0;
  for (DD4hep::XML::Collection_t xRingColl(xFirstDiscRings, _U(ring)); nullptr != xRingColl; ++xRingColl) {
    Component xRing = static_cast<Component>(xRingColl);
    Component xRingModules = xRing.child("modules");
    Component xModuleOdd = xRingModules.child("moduleOdd");
    Component xModuleEven = xRingModules.child("moduleEven");
    Component xModuleProperties = xRing.child("moduleProperties");
    Component xSensorProperties = xRing.child("sensorProperties");
    Volume moduleVolume(
      "module", 
      DD4hep::Geometry::Trapezoid(
      xModuleProperties.attr<double>("modWidthMin"),
      xModuleProperties.attr<double>("modWidthMax"),
      xModuleProperties.attr<double>("modThickness"),
      xModuleProperties.attr<double>("modThickness"),
      xSensorProperties.attr<double>("sensorLength")), // todo!
      lcdd.material("Air"));
    unsigned int nPhi = xRing.attr<int>("nModules");
    double lX, lY, lZ;
    for (unsigned int phiIndex = 0; phiIndex < nPhi; ++phiIndex) {
      double phi = 2 * dd4hep::pi * static_cast<double>(phiIndex) / static_cast<double>(nPhi);
      DD4hep::Geometry::RotationZ lRotation_PhiPos(phi);
      if (0 == phiIndex % 2) {
        lX = xModuleEven.X();
        lY = xModuleEven.Y();
        lZ = xModuleEven.Z() - dimensions.zmin();
      } else {
        lX = xModuleOdd.X();
        lY = xModuleOdd.Y();
        lZ = xModuleOdd.Z() - dimensions.zmin();
      }
      DD4hep::Geometry::Translation3D lTranslation(lX, lY, lZ);
      PlacedVolume placedModuleVolume = discVolume.placeVolume(moduleVolume, lRotation_PhiPos * lTranslation);
      placedModuleVolume.addPhysVolID("module", moduleCounter);
      ++moduleCounter;
    }
  }

  unsigned int discCounter = 0;
  double currentZ;
  for (DD4hep::XML::Collection_t xDiscColl(xDiscs, "discZPls"); nullptr != xDiscColl; ++xDiscColl) {
    Component xDisc = static_cast<Component>(xDiscColl);
    currentZ = xDisc.z() - dimensions.zmin()  - 0.5 *  (dimensions.zmax() - dimensions.zmin());
    PlacedVolume placedDiscVolume = envelopeVolume.placeVolume(
        discVolume, DD4hep::Geometry::Position(0, 0, currentZ));
    placedDiscVolume.addPhysVolID("disc", discCounter);
    ++discCounter;

  }

  // top of the hierarchy
  Volume motherVol = lcdd.pickMotherVolume(GenericTrackerEndcapWorld);
  PlacedVolume placedEnvelopeVolume = motherVol.placeVolume(posnegEnvelopeVolume);
  placedEnvelopeVolume.addPhysVolID("system", xmlDet.id());

  // place everything twice -- forward / backward
  DD4hep::Geometry::Translation3D lTranslation_posEnvelope(
      0, 0, - dimensions.zmin() - 0.5 * (dimensions.zmax() - dimensions.zmin()));
  PlacedVolume placedGenericTrackerEndcap_pos = posnegEnvelopeVolume.placeVolume(
      envelopeVolume, DD4hep::Geometry::Position(0, 0, dimensions.zmin() + 0.5 * (dimensions.zmax() - dimensions.zmin())));
  PlacedVolume placedGenericTrackerEndcap_neg = posnegEnvelopeVolume.placeVolume(
      envelopeVolume, lTranslation_posEnvelope * DD4hep::Geometry::RotationX(dd4hep::pi));
  placedGenericTrackerEndcap_pos.addPhysVolID("posneg", 0);
  placedGenericTrackerEndcap_neg.addPhysVolID("posneg", 1);
  GenericTrackerEndcapWorld.setPlacement(placedEnvelopeVolume);
  return GenericTrackerEndcapWorld;
}
}  // namespace det

DECLARE_DETELEMENT(TkLayoutEcapTracker, det::createTkLayoutTrackerEndcap)
