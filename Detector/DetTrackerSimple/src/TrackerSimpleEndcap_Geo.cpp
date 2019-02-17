
#include "DD4hep/DetFactoryHelper.h"

using dd4hep::Volume;
using dd4hep::DetElement;
using dd4hep::xml::Dimension;
using dd4hep::xml::Component;
using dd4hep::PlacedVolume;

namespace det {


static dd4hep::Ref_t createSimpleTrackerEndcap(dd4hep::Detector& lcdd,
                                                 dd4hep::xml::Handle_t xmlElement,
                                                 dd4hep::SensitiveDetector sensDet) {
  // shorthands
  dd4hep::xml::DetElement xmlDet = static_cast<dd4hep::xml::DetElement>(xmlElement);
  Dimension dimensions(xmlDet.dimensions());
  double l_overlapMargin = 0.01;

  // get sensitive detector type from xml
  dd4hep::xml::Dimension sdTyp = xmlElement.child(_Unicode(sensitive));  // retrieve the type
  sensDet.setType(sdTyp.typeStr());                                      // set for the whole detector

  // definition of top volume
  std::string detName = xmlDet.nameStr();
  DetElement worldDetElement(detName, xmlDet.id());
  DetElement posEcapDetElement(worldDetElement, "posEndcap", 0);


  dd4hep::Assembly envelopeVolume("endcapEnvelope");
  envelopeVolume.setVisAttributes(lcdd.invisible());

  Component xDiscs = xmlElement.child(_Unicode(discs));

  double envelopeThickness = 0.5 * (dimensions.zmax() - dimensions.zmin());

  l_overlapMargin *= 0.9;

  unsigned int discCounter = 0;
  double currentZ;
  std::vector<Volume> discVolumeVec;
  std::vector<DetElement> discDetElementVec;



  for (dd4hep::xml::Collection_t xDiscColl(xDiscs, _Unicode(discZPls)); nullptr != xDiscColl; ++xDiscColl) {
    Component xDisc = static_cast<Component>(xDiscColl);
    Component xCurrentRings = xDisc.child(_Unicode(rings));
    // create disc volume
    double discThickness = 0.1; //0.5 * (xDisc.zmax() - xDisc.zmin());
    currentZ = xDisc.z() - dimensions.zmin() - envelopeThickness;
    if (xCurrentRings.hasChild(_Unicode(ring))) {  // we have information to construct a new volume
      dd4hep::Tube discShape(
          xDisc.rmin() - l_overlapMargin, xDisc.rmax() + l_overlapMargin, discThickness + l_overlapMargin);

      discVolumeVec.emplace_back("disc", discShape, lcdd.air());
      discVolumeVec.back().setSensitiveDetector(sensDet);
      discDetElementVec.emplace_back(posEcapDetElement, "disc" + std::to_string(discCounter), discCounter);
    } else {
      discDetElementVec.emplace_back(discDetElementVec.back().clone("disc" + std::to_string(discCounter)));
      posEcapDetElement.add(discDetElementVec.back());
    }
    PlacedVolume placedDiscVolume = envelopeVolume.placeVolume(discVolumeVec.back(), dd4hep::Position(0, 0, currentZ));
    placedDiscVolume.addPhysVolID("disc", discCounter);
    ++discCounter;

    discDetElementVec.back().setPlacement(placedDiscVolume);
  }

  dd4hep::Assembly bothEndcaps("bothEndcapsEnvelope");

  dd4hep::Translation3D envelopeTranslation(0, 0, dimensions.zmin() + envelopeThickness);

  dd4hep::RotationX envelopeNegRotation(dd4hep::pi);
  dd4hep::RotationX envelopePosRotation(0.);
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

DECLARE_DETELEMENT(SimpleEcapTracker, det::createSimpleTrackerEndcap)
