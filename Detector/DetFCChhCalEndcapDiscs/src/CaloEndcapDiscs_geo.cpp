#include "DD4hep/DetFactoryHelper.h"

// Gaudi
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

namespace det {
void buildOneEndcap(MsgStream& lLog, DD4hep::Geometry::LCDD& aLcdd, DD4hep::Geometry::SensitiveDetector& aSensDet, DD4hep::Geometry::Volume& aEnvelope, DD4hep::XML::Handle_t& aXmlElement, int sign) {

  DD4hep::XML::Dimension dim(aXmlElement.child(_Unicode(dimensions)));

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
  lLog << MSG::INFO << " rmin (cm) = " << dim.rmin1() << " rmin (cm) = " << dim.rmin2()
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
  lLog << MSG::INFO << "Detector length: (cm) " <<  length << endmsg;
  // Place components starting from closer to the collision-point
  double zOffset = (length / 2. - marginOutside) * - sign;
  double rMax = dim.rmax();
// First disc to place is readout
  zOffset += sign * (readoutThickness / 2.);
  double nonAbsorberRmin = std::min(dim.rmin1(), dim.rmin2());
  double tanTheta = fabs(dim.rmin2() - dim.rmin1()) / (2 * dim.dz());
  nonAbsorberRmin += (marginOutside + readoutThickness + activeThickness / 2.) * tanTheta; // for first readout position
  double dR1 = passiveThickness * tanTheta; // between readout and passive
  double dR2 = (activeThickness + readoutThickness + passiveThickness) * tanTheta; // between two readout discs
  DD4hep::Geometry::Tube readoutShapePre(nonAbsorberRmin, rMax, readoutThickness / 2.);
  DD4hep::Geometry::Tube activeShapePre(nonAbsorberRmin, rMax, activeThickness / 4.);
  DD4hep::Geometry::Volume readoutVolPre("readoutPre", readoutShapePre, aLcdd.material(readoutMaterial));
  DD4hep::Geometry::Volume activeVolPre("activePre", activeShapePre, aLcdd.material(activeMaterial));
  activeVolPre.setSensitiveDetector(sensDet);
  DD4hep::Geometry::PlacedVolume readoutPhysVolPre = aEnvelope.placeVolume(readoutVolPre,
    DD4hep::Geometry::Position(0, 0, zOffset));
  std::vector<DD4hep::Geometry::PlacedVolume> activePhysVols;
  activePhysVols.reserve(numDiscs * 2);
  activePhysVols.push_back( aEnvelope.placeVolume(activeVolPre,
      DD4hep::Geometry::Position(0, 0, zOffset + sign * (readoutThickness / 2. + activeThickness / 4.))));
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
    aEnvelope.placeVolume(passiveVol, DD4hep::Geometry::Position(0, 0, zOffset));
    activePhysVols.push_back( aEnvelope.placeVolume(activeVol,
        DD4hep::Geometry::Position(0, 0, zOffset + sign * (passiveThickness / 2. + activeThickness / 4.))));
    aEnvelope.placeVolume(readoutVol,
      DD4hep::Geometry::Position(0, 0, zOffset + sign * (passiveThickness / 2.  + activeThickness / 2. + readoutThickness / 2.)));
    activePhysVols.push_back( aEnvelope.placeVolume(activeVol,
        DD4hep::Geometry::Position(0, 0, zOffset + sign * (passiveThickness / 2.  + readoutThickness + activeThickness * 3 / 4.))));
    lLog << MSG::DEBUG << "Placing passive at z= " << zOffset
         << " active1 at " << zOffset + sign * (passiveThickness/ 2.  + activeThickness / 4. )
         << " readout at z= " << zOffset + sign * (passiveThickness/ 2.  + activeThickness / 2. + readoutThickness / 2.)
         << " active2 at " << zOffset + sign * (passiveThickness/ 2.  + readoutThickness + activeThickness * 3 / 4. ) << endmsg;
        zOffset +=  sign * (readoutThickness + activeThickness + passiveThickness);
    if (iDiscs == numDiscs - 2) {
      // finish detector with the last disc of abosrber (for GND layer)
      DD4hep::Geometry::PlacedVolume passivePhysVolPost = aEnvelope.placeVolume(passiveVol,
        DD4hep::Geometry::Position(0, 0, zOffset));
      lLog << MSG::DEBUG << "Placing last passive disc at z= " << zOffset << endmsg;
    }
    for(uint iActive = 0; iActive < activePhysVols.size(); iActive++) {
      activePhysVols[iActive].addPhysVolID("layer", iActive);
    }
  }
  return;
}

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
  DD4hep::Geometry::Cone envelopePositive(dim.dz(), dim.rmin1(), dim.rmax(), dim.rmin2(), dim.rmax());
  DD4hep::Geometry::Cone envelopeNegative(dim.dz(), dim.rmin2(), dim.rmax(), dim.rmin1(), dim.rmax());
  DD4hep::Geometry::UnionSolid envelopeShape(envelopePositive, envelopeNegative, DD4hep::Geometry::Position(0,0,-2*dim.z_offset()));
  DD4hep::Geometry::Volume envelopeVol(nameDet + "_vol", envelopeShape, aLcdd.material("Air"));
  DD4hep::Geometry::Volume envelopePositiveVol(nameDet + "_positive_vol", envelopePositive, aLcdd.material("Air"));
  DD4hep::Geometry::Volume envelopeNegativeVol(nameDet + "_negative_vol", envelopeNegative, aLcdd.material("Air"));

  lLog << MSG::INFO << "Placing dector on the positive side: (cm) " <<  dim.z_offset() << endmsg;
  buildOneEndcap(lLog, aLcdd, aSensDet, envelopePositiveVol, aXmlElement, 1);
  lLog << MSG::INFO << "Placing dector on the negative side: (cm) " <<  - dim.z_offset() << endmsg;
  buildOneEndcap(lLog, aLcdd, aSensDet, envelopeNegativeVol, aXmlElement, -1);

  // Place the envelope
  DD4hep::Geometry::PlacedVolume envelopePositivePhysVol = envelopeVol.placeVolume(envelopePositiveVol);
  envelopePositivePhysVol.addPhysVolID("subsystem", 0);
  DD4hep::Geometry::DetElement caloEndcapPositiveDetElem(caloEndcapDetElem, "positive", 0);
  caloEndcapPositiveDetElem.setPlacement(envelopePositivePhysVol);
  DD4hep::Geometry::PlacedVolume envelopeNegativePhysVol = envelopeVol.placeVolume(envelopeNegativeVol, DD4hep::Geometry::Position(0,0,-2 * dim.z_offset()));
  envelopeNegativePhysVol.addPhysVolID("subsystem", 1);
  DD4hep::Geometry::DetElement caloEndcapNegativeDetElem(caloEndcapDetElem, "negative", 0);
  caloEndcapNegativeDetElem.setPlacement(envelopeNegativePhysVol);
  DD4hep::Geometry::Volume motherVol = aLcdd.pickMotherVolume(caloEndcapDetElem);
  DD4hep::Geometry::PlacedVolume envelopePhysVol = motherVol.placeVolume(envelopeVol, DD4hep::Geometry::Position(0., 0., dim.z_offset()));
  caloEndcapDetElem.setPlacement(envelopePhysVol);
  envelopePhysVol.addPhysVolID("system", idDet);
  return caloEndcapDetElem;
}
}  // namespace det

DECLARE_DETELEMENT(CaloEndcapDiscs, det::createCaloEndcapDiscs)
