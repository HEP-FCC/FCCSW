#include "DD4hep/DetFactoryHelper.h"

// Gaudi
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

namespace det {
static DD4hep::Geometry::Ref_t createCaloEndcapDiscs(DD4hep::Geometry::LCDD& aLcdd,
                                                        DD4hep::XML::Handle_t aXmlElement,
                                                        DD4hep::Geometry::SensitiveDetector aSensDet) {
  ServiceHandle<IMessageSvc> msgSvc("MessageSvc", "CalEndcapConstruction");
  MsgStream lLog(&(*msgSvc), "CalEndcapConstruction");

  DD4hep::XML::DetElement xmlDetElem = aXmlElement;
  std::string nameDet = xmlDetElem.nameStr();
  int idDet = xmlDetElem.id();
  DD4hep::XML::Dimension dim(xmlDetElem.dimensions());
  DD4hep::Geometry::DetElement caloEndcapDetElem(nameDet, idDet);

  // Create air envelope for the whole endcap
  DD4hep::Geometry::Volume envelopeVol(nameDet + "_vol",
    DD4hep::Geometry::Cone(dim.dz(), dim.rmin1(), dim.rmax(), dim.rmin2(), dim.rmax()),
    aLcdd.material("Air"));

  DD4hep::XML::DetElement active = aXmlElement.child(_Unicode(active));
  std::string activeMaterial = active.materialStr();
  double activeThickness = active.thickness();

  DD4hep::XML::DetElement readout = aXmlElement.child(_Unicode(readout));
  std::string readoutMaterial = readout.materialStr();
  double readoutThickness = readout.thickness();

  DD4hep::XML::DetElement passive = aXmlElement.child(_Unicode(passive));
  DD4hep::XML::DetElement passiveInner = passive.child(_Unicode(inner));
  DD4hep::XML::DetElement passiveOuter = passive.child(_Unicode(outer));
  DD4hep::XML::DetElement passiveGlue = passive.child(_Unicode(glue));
  double passiveInnerThickness = passiveInner.thickness();
  double passiveOuterThickness = passiveOuter.thickness();
  double passiveGlueThickness = passiveGlue.thickness();
  double passiveThickness = passiveInnerThickness + passiveOuterThickness + passiveGlueThickness;
  std::string passiveInnerMaterial = passiveInner.materialStr();
  std::string passiveOuterMaterial = passiveOuter.materialStr();
  std::string passiveGlueMaterial = passiveGlue.materialStr();
  std::string passiveMaterial;
  if (passiveInnerThickness < passiveThickness) {
    passiveMaterial = "Air";
  } else {
    passiveMaterial = passiveInnerMaterial;
  }

  DD4hep::Geometry::SensitiveDetector sensDet = aSensDet;
  DD4hep::XML::Dimension sensDetType = aXmlElement.child(_U(sensitive));
  sensDet.setType(sensDetType.typeStr());
  lLog << MSG::INFO << "Endcap volume named " << nameDet << " with ID " << idDet
       << " rmin (cm) = " << dim.rmin1() << " rmin (cm) = " << dim.rmin2()
       << " rmax (cm) = " << dim.rmax()
       << " length (cm) = " << dim.dz()
       << " Sensitive volume of type: " << sensDetType.typeStr() << endmsg;
  double length = dim.dz() * 2;
  // First disc set is different: readout is first (HV), then half of active material, then absorber (GND)
  // Next disc sets have active material on both sides
  double lengthWithoutFirst = length - readoutThickness - activeThickness / 2. - passiveThickness;
  uint numDiscs = floor(lengthWithoutFirst / (activeThickness + readoutThickness + passiveThickness));
  double marginOutside = (lengthWithoutFirst - numDiscs * (activeThickness + readoutThickness + passiveThickness)) / 2.;
  // add the first disc set to the number of all discs
  numDiscs += 1;
  lLog << MSG::INFO << "Thickness of active material in between absorbers (cm) = " << activeThickness
       << "\nThickness of absorber discs (cm) = " << passiveThickness
       << "\nThickness of readout disc placed in between absorber plates (cm) = " << readoutThickness
       << "\nNumber of absorber/readout discs: " << numDiscs
       << "\nMargin outside first readout disc and last absorber disc, filled with non-sensitive active medium (cm) = "
       << marginOutside << endmsg;
  lLog << MSG::INFO << "Detector offset: " <<  dim.z_offset()
       << " Detector half-length: " <<  length / 2. << endmsg;
  // Place components starting from closer to the collision-point
  int sign =  dim.z_offset() / abs(dim.z_offset());
  std::string side = (sign > 0) ? "positive" : "negative";
  lLog << MSG::INFO << "Detector placed at z " << side << "." << endmsg;
  double zOffset = (length / 2. - marginOutside) * - sign;
  double rMax = dim.rmax();
// First disc to place is readout
  zOffset += sign * (readoutThickness / 2.);
  double nonAbsorberRmin = std::min(dim.rmin1(), dim.rmin2());
  double tanTheta = fabs(dim.rmin2() - dim.rmin1()) / (2 * dim.dz());
  nonAbsorberRmin += (marginOutside + readoutThickness + activeThickness / 2.) * tanTheta; // for first readout position
  double dR1 = passiveThickness * tanTheta; // between readout and passive
  double dR2 = (activeThickness + readoutThickness + passiveThickness) * tanTheta; // between two readout discs
  lLog << MSG::WARNING << " RMIN1: " << dim.rmin1() << " RMIN@: " << dim.rmin2()
       << " tan theta: " <<  sign * fabs(dim.rmin2() - dim.rmin1()) / (2 * dim.dz()) << " dR1: " << dR1 << " dR2: " <<dR2 <<endmsg;
  DD4hep::Geometry::Tube readoutShapePre(nonAbsorberRmin, rMax, readoutThickness / 2.);
  DD4hep::Geometry::Tube activeShapePre(nonAbsorberRmin, rMax, activeThickness / 4.);
  DD4hep::Geometry::Volume readoutVolPre("readoutPre", readoutShapePre, aLcdd.material(readoutMaterial));
  DD4hep::Geometry::Volume activeVolPre("activePre", activeShapePre, aLcdd.material(activeMaterial));
  activeVolPre.setSensitiveDetector(sensDet);
  DD4hep::Geometry::PlacedVolume readoutPhysVolPre = envelopeVol.placeVolume(readoutVolPre,
    DD4hep::Geometry::Position(0, 0, zOffset));
  DD4hep::Geometry::PlacedVolume activePhysVolPre = envelopeVol.placeVolume(activeVolPre,
    DD4hep::Geometry::Position(0, 0, zOffset + sign * (readoutThickness / 2. + activeThickness / 4.)));
  lLog << MSG::DEBUG << "Placing first readout at " << zOffset  <<
    " and active at z= " << zOffset + sign * (activeThickness / 4. + readoutThickness / 2.) <<endmsg;
// Now place complete sets of discs: absorber|active|readout|active
  zOffset += sign * (readoutThickness / 2. + activeThickness / 2. + passiveThickness / 2.);
// Loop placing readout, active and passive discs
  for (uint iDiscs = 0; iDiscs < numDiscs - 1; iDiscs++) {
    nonAbsorberRmin += dR2;
    // readout and active discs on both sides of readout have the same radius, but different thickness
    DD4hep::Geometry::Tube activeShape(nonAbsorberRmin, rMax, activeThickness / 4.);
    DD4hep::Geometry::Tube readoutShape(nonAbsorberRmin, rMax, readoutThickness / 2.);
    DD4hep::Geometry::Tube passiveShape(nonAbsorberRmin + dR1, rMax, passiveThickness / 2.);
    DD4hep::Geometry::Volume activeVol("active", activeShape, aLcdd.material(activeMaterial));
    DD4hep::Geometry::Volume readoutVol("readout", readoutShape, aLcdd.material(readoutMaterial));
    DD4hep::Geometry::Volume passiveVol("passive", passiveShape, aLcdd.material(passiveMaterial));
    activeVol.setSensitiveDetector(sensDet);
    // absorber may consist of inner and outer material
    if (passiveInnerThickness < passiveThickness) {
      // create shapes
      DD4hep::Geometry::Tube passiveInnerShape(nonAbsorberRmin + dR1, rMax, passiveInnerThickness / 2.);
      DD4hep::Geometry::Tube passiveGlueShape(nonAbsorberRmin + dR1, rMax, passiveGlueThickness / 4.);
      DD4hep::Geometry::Tube passiveOuterShape(nonAbsorberRmin + dR1, rMax, passiveOuterThickness / 4.);
      // create volumes
      DD4hep::Geometry::Volume passiveInnerVol(passiveInnerMaterial + "_passive", passiveInnerShape,
        aLcdd.material(passiveInnerMaterial));
      DD4hep::Geometry::Volume passiveOuterVol(passiveOuterMaterial + "_passive", passiveOuterShape,
        aLcdd.material(passiveOuterMaterial));
      DD4hep::Geometry::Volume passiveGlueVol(passiveGlueMaterial + "_passive", passiveGlueShape,
        aLcdd.material(passiveGlueMaterial));
      // place volumes
      DD4hep::Geometry::PlacedVolume passiveInnerPhysVol =
        passiveVol.placeVolume(passiveInnerVol, DD4hep::Geometry::Position(0, 0, 0));
      DD4hep::Geometry::PlacedVolume passiveOuterPhysVolBelow = passiveVol.placeVolume( passiveOuterVol,
        DD4hep::Geometry::Position(0,0,passiveInnerThickness / 2. + passiveGlueThickness / 2. + passiveOuterThickness / 4.));
      DD4hep::Geometry::PlacedVolume passiveOuterPhysVolAbove = passiveVol.placeVolume( passiveOuterVol,
        DD4hep::Geometry::Position(0,0,- passiveInnerThickness / 2. - passiveGlueThickness / 2. - passiveOuterThickness / 4.));
      DD4hep::Geometry::PlacedVolume passiveGluePhysVolBelow = passiveVol.placeVolume( passiveGlueVol,
        DD4hep::Geometry::Position(0,0,-passiveInnerThickness / 2. - passiveGlueThickness / 4.));
      DD4hep::Geometry::PlacedVolume passiveGluePhysVolAbove = passiveVol.placeVolume(passiveGlueVol,
        DD4hep::Geometry::Position(0,0,passiveInnerThickness / 2. + passiveGlueThickness / 4.));
    }
    DD4hep::Geometry::PlacedVolume passivePhysVol = envelopeVol.placeVolume(passiveVol,
      DD4hep::Geometry::Position(0, 0, zOffset));
    DD4hep::Geometry::PlacedVolume activePhysVol1 = envelopeVol.placeVolume(activeVol,
      DD4hep::Geometry::Position(0, 0, zOffset + sign * (passiveThickness / 2. + activeThickness / 4.)));
    DD4hep::Geometry::PlacedVolume readoutPhysVol = envelopeVol.placeVolume(readoutVol,
      DD4hep::Geometry::Position(0, 0, zOffset + sign * (passiveThickness / 2.  + activeThickness / 2. + readoutThickness / 2.)));
    DD4hep::Geometry::PlacedVolume activePhysVol2 = envelopeVol.placeVolume(activeVol,
      DD4hep::Geometry::Position(0, 0, zOffset + sign * (passiveThickness / 2.  + readoutThickness + activeThickness * 3 / 4.)));
    lLog << MSG::DEBUG << "Placing passive at z= " << zOffset
         << " active1 at " << zOffset + sign * (passiveThickness/ 2.  + activeThickness / 4. )
         << " readout at z= " << zOffset + sign * (passiveThickness/ 2.  + activeThickness / 2. + readoutThickness / 2.)
         << " active2 at " << zOffset + sign * (passiveThickness/ 2.  + readoutThickness + activeThickness * 3 / 4. ) << endmsg;
        zOffset +=  sign * (readoutThickness + activeThickness + passiveThickness);
    if (iDiscs == numDiscs - 2) {
      // finish detector with the last disc of abosrber (for GND layer)
      DD4hep::Geometry::PlacedVolume passivePhysVolPost = envelopeVol.placeVolume(passiveVol,
        DD4hep::Geometry::Position(0, 0, zOffset));
      lLog << MSG::DEBUG << "Placing last passive disc at z= " << zOffset << endmsg;
    }
  }
  // Place the envelope
  DD4hep::Geometry::Volume motherVol = aLcdd.pickMotherVolume(caloEndcapDetElem);
  DD4hep::Geometry::PlacedVolume envelopePhysVol = motherVol.placeVolume(envelopeVol, DD4hep::Geometry::Position(0., 0., dim.z_offset()));
  caloEndcapDetElem.setPlacement(envelopePhysVol);
  return caloEndcapDetElem;
}
}  // namespace det

DECLARE_DETELEMENT(CaloEndcapDiscs, det::createCaloEndcapDiscs)
