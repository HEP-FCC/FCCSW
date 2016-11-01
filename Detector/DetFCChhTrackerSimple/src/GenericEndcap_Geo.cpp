
#include "DetCommon/DetUtils.h"
#include "DD4hep/DetFactoryHelper.h"

using DD4hep::Geometry::Volume;
using DD4hep::Geometry::DetElement;
using DD4hep::XML::Dimension;
using DD4hep::Geometry::PlacedVolume;

namespace det {
/**
Factory for a configurable, generic tracker endcap.
@author: Valentin Volkl
*/
static DD4hep::Geometry::Ref_t createGenericTrackerEndcap(DD4hep::Geometry::LCDD& lcdd,
                                                          DD4hep::XML::Handle_t xmlElement,
                                                          DD4hep::Geometry::SensitiveDetector sensDet) {
  // shorthands
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
  DD4hep::Geometry::Tube posnegEnvelopeShape_add(dimensions.rmin(), dimensions.rmax(), (dimensions.z2()));
  // make the negative shape slighly larger in the radial direction 
  // to be sure that everything is subtracted between -z1 and z1
  DD4hep::Geometry::Box posnegEnvelopeShape_subtract(
      dimensions.rmax() * 1.001, dimensions.rmax() * 1.001, dimensions.z1());
  DD4hep::Geometry::SubtractionSolid posnegEnvelopeShape(posnegEnvelopeShape_add, posnegEnvelopeShape_subtract);
  Volume posnegEnvelopeVolume(detName, posnegEnvelopeShape, lcdd.air());
  posnegEnvelopeVolume.setVisAttributes(lcdd.invisible());

  // envelope volume for one of the endcaps, either forward or backward
  DD4hep::Geometry::Tube envelopeShape(dimensions.rmin(), dimensions.rmax(), 0.5 * (dimensions.z2() - dimensions.z1()));
  Volume envelopeVolume(detName, envelopeShape, lcdd.air());
  envelopeVolume.setVisAttributes(lcdd.invisible());

  // loop over 'layer' nodes in xml
  unsigned int layerCounter = 0;
  for (DD4hep::XML::Collection_t xLayerColl(xmlElement, _U(layers)); nullptr != xLayerColl; ++xLayerColl) {
    DD4hep::XML::Component xLayer = static_cast<DD4hep::XML::Component>(xLayerColl);


    // create petals
    unsigned int nPhi = static_cast<unsigned int>(utils::getAttrValueWithFallback(xLayer, "nPhi", 16));

    const double lModuleTwistAngle = utils::getAttrValueWithFallback(xLayer, "module_twist_angle", 0.05 * M_PI);
    double dr = xLayer.rmax() - xLayer.rmin();
    double dphi = 2 * dd4hep::pi / static_cast<double>(nPhi);
    double tn = tan(dphi);
    Volume petalVolume(
        "petal",
        DD4hep::Geometry::Trapezoid(
            0.5 * xLayer.rmin() * tn, 0.5 * xLayer.rmax() * tn, xLayer.thickness(), xLayer.thickness(), 0.5 * dr),
        lcdd.material("Silicon"));
    petalVolume.setVisAttributes(lcdd, xLayer.visStr());
    petalVolume.setSensitiveDetector(sensDet);

    // handle repeat attribute in xml
    double layerThickness;
    unsigned int numLayers;
    double current_z;
    // "repeat" layers  equidistant between rmin and rmax
    numLayers = xLayer.repeat();
    layerThickness = (xLayer.z2() - xLayer.z1()) / numLayers;
    // create layers.
    for (unsigned int repeatIndex = 0; repeatIndex < numLayers; ++repeatIndex) {
      DD4hep::Geometry::Tube layerShape(xLayer.rmin(), xLayer.rmax(), 0.5 * layerThickness);
      Volume layerVolume("layer" + std::to_string(layerCounter), layerShape, lcdd.air());
      layerVolume.setVisAttributes(lcdd.invisible());
      ++layerCounter;
      // place layers not at center, but at z1 value of containing envelope
      // subtract half of the envelope length
      current_z = (repeatIndex + 0.5) * layerThickness + xLayer.z1() - dimensions.z1();
      PlacedVolume placedLayerVolume = envelopeVolume.placeVolume(
          layerVolume, DD4hep::Geometry::Position(0, 0, current_z - 0.5 * (dimensions.z2() - dimensions.z1())));
      placedLayerVolume.addPhysVolID("layer", layerCounter);

      double phi;
      double r = xLayer.rmin();
      for (unsigned int phiIndex = 0; phiIndex < nPhi; ++phiIndex) {
        phi = 2 * dd4hep::pi * static_cast<double>(phiIndex) / static_cast<double>(nPhi);
        // oriented along z at first
        DD4hep::Geometry::Translation3D lTranslation_ringPhiPos(0, 0, r + 0.5 * dr);
        DD4hep::Geometry::RotationY lRotation_ringPhiPos(phi);
        DD4hep::Geometry::RotationX lRotation_orientRing(0.5 * dd4hep::pi);
        // twist petals slightly so they can overlap
        DD4hep::Geometry::RotationZ lRotation_twist(lModuleTwistAngle);
        PlacedVolume placedPetalVolume = layerVolume.placeVolume(
            petalVolume, lRotation_orientRing * lRotation_ringPhiPos * lTranslation_ringPhiPos * lRotation_twist);
        placedPetalVolume.addPhysVolID("petal", phiIndex);
      }
    }
  }

  // top of the hierarchy
  Volume motherVol = lcdd.pickMotherVolume(GenericTrackerEndcapWorld);
  PlacedVolume placedEnvelopeVolume = motherVol.placeVolume(posnegEnvelopeVolume);
  placedEnvelopeVolume.addPhysVolID("system", xmlDet.id());

  // place everything twice -- forward / backward
  DD4hep::Geometry::Translation3D lTranslation_posEnvelope(
      0, 0, -dimensions.z1() - 0.5 * (dimensions.z2() - dimensions.z1()));
  PlacedVolume placedGenericTrackerEndcap_pos = posnegEnvelopeVolume.placeVolume(
      envelopeVolume, DD4hep::Geometry::Position(0, 0, dimensions.z1() + 0.5 * (dimensions.z2() - dimensions.z1())));
  PlacedVolume placedGenericTrackerEndcap_neg = posnegEnvelopeVolume.placeVolume(
      envelopeVolume, lTranslation_posEnvelope * DD4hep::Geometry::RotationX(dd4hep::pi));
  placedGenericTrackerEndcap_pos.addPhysVolID("posneg", 0);
  placedGenericTrackerEndcap_neg.addPhysVolID("posneg", 1);
  GenericTrackerEndcapWorld.setPlacement(placedEnvelopeVolume);
  return GenericTrackerEndcapWorld;
}
}  // namespace det

DECLARE_DETELEMENT(GenericTrackerEndcap, det::createGenericTrackerEndcap)
