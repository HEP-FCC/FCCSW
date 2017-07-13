// DD4hep
#include "DD4hep/DetFactoryHelper.h"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

using DD4hep::Geometry::Volume;
using DD4hep::Geometry::DetElement;
using DD4hep::XML::Dimension;
using DD4hep::Geometry::PlacedVolume;

namespace det {

static DD4hep::Geometry::Ref_t
createHCal(DD4hep::Geometry::LCDD& lcdd, xml_h xmlElement, DD4hep::Geometry::SensitiveDetector sensDet) {
  // Get the Gaudi message service and message stream:
  ServiceHandle<IMessageSvc> msgSvc("MessageSvc", "ExtHCalConstruction");
  MsgStream lLog(&(*msgSvc), "ExtHCalConstruction");

  xml_det_t xmlDet = xmlElement;
  std::string detName = xmlDet.nameStr();
  // Make DetElement
  DetElement hCal(detName, xmlDet.id());

  // Make volume that envelopes the whole barrel; set material to air
  Dimension dimensions(xmlDet.dimensions());
  xml_comp_t xEndPlate = xmlElement.child(_Unicode(end_plate));
  double dZEndPlate = xEndPlate.thickness();
  xml_comp_t xFacePlate = xmlElement.child(_Unicode(face_plate));
  double dRhoFacePlate = xFacePlate.thickness();
  xml_comp_t xSpace = xmlElement.child(_Unicode(plate_space));  // to avoid overlaps
  double space = xSpace.thickness();
  xml_comp_t xSteelSupport = xmlElement.child(_Unicode(steel_support));
  double dSteelSupport = xSteelSupport.thickness();
  lLog << MSG::DEBUG << "steel support thickness " << dSteelSupport << endmsg;
  lLog << MSG::DEBUG << "steel support material  " << xSteelSupport.materialStr() << endmsg;

  double sensitiveBarrel1Rmin = dimensions.rmin1() + dRhoFacePlate + space;
  double sensitiveBarrel2Rmin = dimensions.rmin2() + dRhoFacePlate + space;

  // Offset in z is given as distance from 0 to the middle of the Calorimeter volume
  double extBarrelOffset1 = dimensions.offset();
  double extBarrelOffset2 = dimensions.z_offset();

  // Hard-coded assumption that we have two different sequences for the modules
  std::vector<xml_comp_t> sequences = {xmlElement.child(_Unicode(sequence_a)), xmlElement.child(_Unicode(sequence_b))};
  // NOTE: This assumes that both have the same dimensions!
  Dimension sequenceDimensions(sequences[1].dimensions());
  double dzSequence = sequenceDimensions.dz();
  lLog << MSG::DEBUG << "sequence thickness " << dzSequence << endmsg;

  // calculate the number of modules fitting in phi and Z
  unsigned int numSequencesPhi = sequenceDimensions.phiBins();
  double dphi = 2 * dd4hep::pi / static_cast<double>(numSequencesPhi);
  unsigned int numSequencesZ1 = static_cast<unsigned>((2*dimensions.width() - dZEndPlate  - space) / dzSequence);
  unsigned int numSequencesZ2 = static_cast<unsigned>((2*dimensions.dz() - dZEndPlate - space) / dzSequence);

  // Hard-coded assumption that we have three different layer types for the modules
  std::vector<xml_comp_t> Layers = {xmlElement.child(_Unicode(layer_1)), xmlElement.child(_Unicode(layer_2)), xmlElement.child(_Unicode(layer_3))};
  unsigned int numSequencesR1 = 0;
  unsigned int numSequencesR2 = 0;
  double moduleDepth1 = 0.;
  double moduleDepth2 = 0.;
  std::vector<double> layerDepths1 = std::vector<double>();
  std::vector<double> layerDepths2 = std::vector<double>();
  for(std::vector<xml_comp_t>::iterator it = Layers.begin(); it != Layers.end(); ++it){
    xml_comp_t layer = *it;
    Dimension layerDimension(layer.dimensions());
    numSequencesR1 += layerDimension.nmodules();
    numSequencesR2 += layerDimension.nModules();
    for(unsigned int nLayer = 0; nLayer < layerDimension.nModules(); nLayer++){
      moduleDepth2 += layerDimension.dr();
      layerDepths2.push_back(layerDimension.dr());
    }
    for(unsigned int nLayer = 0; nLayer < layerDimension.nmodules(); nLayer++){
      moduleDepth1 += layerDimension.dr();
      layerDepths1.push_back(layerDimension.dr());
    }
  }
  lLog << MSG::DEBUG << "retrieved number of layers in first ExtBarrel part:  " << numSequencesR1 << " , which end up to a full module depth in rho of " << moduleDepth1 << endmsg;
  lLog << MSG::DEBUG << "retrieved number of layers in first ExtBarrel part:  " << layerDepths1.size() << endmsg;
  lLog << MSG::DEBUG << "retrieved number of layers in second ExtBarrel part:  " << numSequencesR2 << " , which end up to a full module depth in rho of " << moduleDepth2 << endmsg;
  lLog << MSG::DEBUG << "retrieved number of layers in second ExtBarrel part:  " << layerDepths2.size() << endmsg;

  lLog << MSG::INFO << "constructing first part EB: with offset " << extBarrelOffset1 << ": " << numSequencesPhi << " modules, within the angle " << (dphi * 180. / dd4hep::pi)
       << " per ring in phi, " << numSequencesZ1 << " rings in Z, " << numSequencesR1 << " layers in Rho, "
       << numSequencesR1 * numSequencesZ1 * numSequencesPhi << " tiles" << endmsg;

  lLog << MSG::INFO << "constructing second part EB: with offset " << extBarrelOffset2 << ": " << numSequencesPhi << " modules, within the angle " << (dphi * 180. / dd4hep::pi)
       << " per ring in phi, " << numSequencesZ2 << " rings in Z, " << numSequencesR2 << " layers in Rho, "
       << numSequencesR2 * numSequencesZ2 * numSequencesPhi << " tiles" << endmsg;

  lLog << MSG::INFO << "total number of channels: " << 2 * (numSequencesR2 * numSequencesZ2 * numSequencesPhi) + 2*(numSequencesR1 * numSequencesZ1 * numSequencesPhi) << endmsg;

  // Calculate correction along z based on the module size (can only have natural number of modules)
  double dzDetector1 = (numSequencesZ1 * dzSequence) / 2 + dZEndPlate/2 + space/2;
  lLog << MSG::INFO << "correction of dz (negative = size reduced) first part EB :" << dzDetector1*2 - dimensions.width() << endmsg;
  double dzDetector2 = (numSequencesZ2 * dzSequence) / 2 + dZEndPlate/2 + space/2;
  lLog << MSG::INFO << "correction of dz (negative = size reduced) second part EB:" << dzDetector2*2 - dimensions.dz() << endmsg;

  DD4hep::Geometry::Tube envelopeShape(dimensions.rmin2(), dimensions.rmax2(), extBarrelOffset2 + dzDetector2);
  Volume envelopeVolume("envelopeVolume", envelopeShape, lcdd.air());
  envelopeVolume.setVisAttributes(lcdd, dimensions.visStr());

  // set the sensitive detector type to the DD4hep calorimeter -- including Birks Law
  sensDet.setType("BirksLawCalorimeterSD");

  // Add structural support made of steel inside of HCal
  DetElement facePlate1(hCal, "FacePlate1", 0);
  DD4hep::Geometry::Tube facePlateShape1(dimensions.rmin1(), sensitiveBarrel1Rmin, (dzDetector1 - dZEndPlate/2 - space/2));
  Volume facePlateVol1("facePlate1", facePlateShape1, lcdd.material(xFacePlate.materialStr()));
  facePlateVol1.setVisAttributes(lcdd, xFacePlate.visStr());
  DD4hep::Geometry::Position offsetFace1front(0, 0, extBarrelOffset1);
  DD4hep::Geometry::Position offsetFace1back(0, 0, -extBarrelOffset1);
  // Faceplate for 2nd part of extended Barrel
  DetElement facePlate2(hCal, "FacePlate2", 0);
  DD4hep::Geometry::Tube facePlateShape2(dimensions.rmin2(), sensitiveBarrel2Rmin, (dzDetector2 - dZEndPlate/2 - space/2));
  Volume facePlateVol2("facePlate2", facePlateShape2, lcdd.material(xFacePlate.materialStr()));
  facePlateVol2.setVisAttributes(lcdd, xFacePlate.visStr());
  DD4hep::Geometry::Position offsetFace2front(0, 0, extBarrelOffset2);
  DD4hep::Geometry::Position offsetFace2back(0, 0, -extBarrelOffset2);

  PlacedVolume placedFacePlate1front = envelopeVolume.placeVolume(facePlateVol1, offsetFace1front);
  facePlate1.setPlacement(placedFacePlate1front);
  PlacedVolume placedFacePlate1back = envelopeVolume.placeVolume(facePlateVol1, offsetFace1back);
  facePlate1.setPlacement(placedFacePlate1back);
  PlacedVolume placedFacePlate2front = envelopeVolume.placeVolume(facePlateVol2, offsetFace2front);
  facePlate2.setPlacement(placedFacePlate2front);
  PlacedVolume placedFacePlate2back = envelopeVolume.placeVolume(facePlateVol2, offsetFace2back);
  facePlate2.setPlacement(placedFacePlate2back);

  // Add structural support made of steel at both ends of extHCal
  DD4hep::Geometry::Tube endPlateShape1(dimensions.rmin1(), (dimensions.rmax1() - dSteelSupport), dZEndPlate / 2);
  Volume endPlateVol1("endPlate1", endPlateShape1, lcdd.material(xEndPlate.materialStr()));
  endPlateVol1.setVisAttributes(lcdd, xEndPlate.visStr());
  DD4hep::Geometry::Tube endPlateShape2(dimensions.rmin2(), (dimensions.rmax2() - dSteelSupport), dZEndPlate / 2);
  Volume endPlateVol2("endPlate2", endPlateShape2, lcdd.material(xEndPlate.materialStr()));
  endPlateVol2.setVisAttributes(lcdd, xEndPlate.visStr());

  // Endplates placed for the extended Barrels in front and in the back to the central Barrel
  DetElement endPlatePos(hCal, "endPlatePos", 0);
  DD4hep::Geometry::Position posOffsetFront(0, 0, extBarrelOffset2 + dzDetector2 - (dZEndPlate / 2) );
  DD4hep::Geometry::Position posOffsetBack(0, 0, -extBarrelOffset2 - dzDetector2 + (dZEndPlate / 2) );
  PlacedVolume placedEndPlatePosFront = envelopeVolume.placeVolume(endPlateVol2, posOffsetFront);
  endPlatePos.setPlacement(placedEndPlatePosFront);
  PlacedVolume placedEndPlatePosBack = envelopeVolume.placeVolume(endPlateVol2, posOffsetBack);
  endPlatePos.setPlacement(placedEndPlatePosBack);

  DetElement endPlateNeg(hCal, "endPlateNeg", 1);
  DD4hep::Geometry::Position negOffsetFront(0, 0, extBarrelOffset1 - dzDetector1 + (dZEndPlate / 2));
  DD4hep::Geometry::Position negOffsetBack(0, 0, -extBarrelOffset1 + dzDetector1 - (dZEndPlate / 2));
  PlacedVolume placedEndPlateNegFront = envelopeVolume.placeVolume(endPlateVol1, negOffsetFront);
  endPlateNeg.setPlacement(placedEndPlateNegFront);
  PlacedVolume placedEndPlateNegBack = envelopeVolume.placeVolume(endPlateVol1, negOffsetBack);
  endPlateNeg.setPlacement(placedEndPlateNegBack);

  // Calculation the dimensions of one whole module:
  double tn = tan(dphi / 2.);
  double spacing = sequenceDimensions.x();
  // the half depth of sequence (placed in z in HCAL coordinated) is y coordinate of trapezoid definition
  double dy0 = dzSequence * 0.5;

  double dx1Module1 = tn * sensitiveBarrel1Rmin - spacing;
  double dx2Module1 = tn * (sensitiveBarrel1Rmin + moduleDepth1) - spacing;
  double dzModule1 = moduleDepth1 / 2;

  double dx1Module2 = tn * sensitiveBarrel2Rmin - spacing;
  double dx2Module2 = tn * (sensitiveBarrel2Rmin + moduleDepth2) - spacing;
  double dzModule2 = moduleDepth2 / 2;

  lLog << MSG::DEBUG << "half height of full module (trapezoid side), first: " << dzModule1 << ", second: " << dzModule2 << endmsg;
  lLog << MSG::DEBUG << "half width  of full module (trapezoid side): " << dy0 << endmsg;
  lLog << MSG::DEBUG << "half width in phi (rmin) of full module (trapezoid side), first: " << dx1Module1 <<  ", second: " << dx1Module2 << endmsg;
  lLog << MSG::DEBUG << "half width in phi (rmax) of full module (trapezoid side), first: " << dx2Module1 <<  ", second: " << dx2Module2 << endmsg;

  double dx1Support1 = tn * (sensitiveBarrel1Rmin + moduleDepth1) - spacing;
  double dx2Support1 = tn * (sensitiveBarrel1Rmin + moduleDepth1 + dSteelSupport) - spacing;
  double dx1Support2 = tn * (sensitiveBarrel2Rmin + moduleDepth2) - spacing;
  double dx2Support2 = tn * (sensitiveBarrel2Rmin + moduleDepth2 + dSteelSupport) - spacing;
  double dzSupport = dSteelSupport / 2;
  
  // DetElement vectors for placement in loop at the end
  std::vector<DD4hep::Geometry::PlacedVolume> supports1;
  supports1.reserve(2*numSequencesPhi);
  std::vector<DD4hep::Geometry::PlacedVolume> modules1;
  modules1.reserve(2*numSequencesPhi);
  std::vector<DD4hep::Geometry::PlacedVolume> supports2;
  supports2.reserve(2*numSequencesPhi);
  std::vector<DD4hep::Geometry::PlacedVolume> modules2;
  modules2.reserve(2*numSequencesPhi);
  std::vector<DD4hep::Geometry::PlacedVolume> rows;
  rows.reserve(numSequencesZ1 + numSequencesZ2);
  std::vector<DD4hep::Geometry::PlacedVolume> layers;
  layers.reserve(layerDepths1.size() + layerDepths2.size());
  std::vector<std::vector<DD4hep::Geometry::PlacedVolume> > tilesInLayers;
  tilesInLayers.reserve(layerDepths1.size() + layerDepths2.size());
  
  // First we construct base wedges:
  Volume wedgeVolume1("wedgeVolume1", DD4hep::Geometry::Trapezoid(dx1Module1, dx2Module1, dy0, dy0, dzModule1),
		     lcdd.material("Air"));  
  Volume wedgeVolume2("wedgeVolume2", DD4hep::Geometry::Trapezoid(dx1Module2, dx2Module2, dy0, dy0, dzModule2),
		     lcdd.material("Air"));  
  double layerR=0.;

  // Placement of subWedges in Wedge
  for (unsigned int idxLayer = 0; idxLayer < layerDepths1.size(); ++idxLayer) {
    auto layerName = DD4hep::XML::_toString(idxLayer, "layer%d");
    unsigned int sequenceIdx = idxLayer % 2;
    
    // get length of layer in rho
    double dz0 = layerDepths1.at(idxLayer) * 0.5;
    
    // in Module rmin = 0  for first wedge, changed radius to the full radius starting at (0,0,0)
    double rminLayer = sensitiveBarrel1Rmin + layerR;
    double rmaxLayer = sensitiveBarrel1Rmin + layerR + layerDepths1.at(idxLayer);
    double dx1 = tn * rminLayer - spacing;
    double dx2 = tn * rmaxLayer - spacing;
    double rMiddle = (rminLayer - sensitiveBarrel1Rmin) + dz0 - dzModule1;
    
    layerR += layerDepths1.at(idxLayer);
    
    Volume layerVolume("layerVolume", DD4hep::Geometry::Trapezoid(dx1, dx2, dy0, dy0, dz0), lcdd.material("Air"));
    layerVolume.setVisAttributes(lcdd.invisible());
    unsigned int idxSubMod = 0;
    unsigned int idxActMod = 0;
    double modCompZOffset = -sequenceDimensions.dz() * 0.5;
    
    // layer middle offset
    DD4hep::Geometry::Position modOffset(0, 0, rMiddle);
    layers.push_back( wedgeVolume1.placeVolume(layerVolume, modOffset) );
    layers.back().addPhysVolID("layer", idxLayer);
    
    std::vector<DD4hep::Geometry::PlacedVolume> tiles;
    // Filling of the subWedge with coponents (submodules)
    for (xml_coll_t xCompColl(sequences[sequenceIdx], _Unicode(module_component)); xCompColl; ++xCompColl, ++idxSubMod) {
      xml_comp_t xComp = xCompColl;
      double dyComp = xComp.thickness() * 0.5;
      Volume modCompVol("modCompVolume", DD4hep::Geometry::Trapezoid(dx1, dx2, dyComp, dyComp, dz0),
			lcdd.material(xComp.materialStr()));
      modCompVol.setVisAttributes(lcdd, xComp.visStr());
      DD4hep::Geometry::Position offset(0, modCompZOffset + dyComp + xComp.y_offset() / 2, 0);     

      if (xComp.isSensitive()) {
	Volume tileVol("tileVolume", DD4hep::Geometry::Trapezoid(dx1, dx2, dyComp, dyComp, dz0),
		       lcdd.material(xComp.materialStr()));
	tileVol.setSensitiveDetector(sensDet);
	tiles.push_back(
                        layerVolume.placeVolume(tileVol, offset) );
	tiles.back().addPhysVolID("tile", idxActMod);
	idxActMod++;
      }
      else {
	tiles.push_back(
			layerVolume.placeVolume(modCompVol, offset) );
      }
      modCompZOffset += xComp.thickness() + xComp.y_offset();
    }
    // Fill vector for DetElements
    tilesInLayers.push_back(tiles);
  }
  layerR = 0;
  // Placement of subWedges in Wedge
  for (unsigned int idxLayer = 0; idxLayer < layerDepths2.size(); ++idxLayer) {
    auto layerName = DD4hep::XML::_toString(idxLayer, "layer%d");
    unsigned int sequenceIdx = idxLayer % 2;
    
    // get length of layer in rho
    double dz0 = layerDepths2.at(idxLayer) * 0.5;
    
    // in Module rmin = 0  for first wedge, changed radius to the full radius starting at (0,0,0)
    double rminLayer = sensitiveBarrel2Rmin + layerR;
    double rmaxLayer = sensitiveBarrel2Rmin + layerR + layerDepths2.at(idxLayer);
    double dx1 = tn * rminLayer - spacing;
    double dx2 = tn * rmaxLayer - spacing;
    double rMiddle = (rminLayer - sensitiveBarrel2Rmin) + dz0 - dzModule2;
    
    layerR += layerDepths2.at(idxLayer);
    
    Volume layerVolume("layerVolume", DD4hep::Geometry::Trapezoid(dx1, dx2, dy0, dy0, dz0), lcdd.material("Air"));
    layerVolume.setVisAttributes(lcdd.invisible());
    unsigned int idxSubMod = 0;
    unsigned int idxActMod = 0;
    double modCompZOffset = -sequenceDimensions.dz() * 0.5;
    
    // layer middle offset
    DD4hep::Geometry::Position modOffset(0, 0, rMiddle);
    layers.push_back( wedgeVolume2.placeVolume(layerVolume, modOffset) );
    layers.back().addPhysVolID("layer", idxLayer);
    
    std::vector<DD4hep::Geometry::PlacedVolume> tiles;
    // Filling of the subWedge with coponents (submodules)
    for (xml_coll_t xCompColl(sequences[sequenceIdx], _Unicode(module_component)); xCompColl; ++xCompColl, ++idxSubMod) {
      xml_comp_t xComp = xCompColl;
      double dyComp = xComp.thickness() * 0.5;
      Volume modCompVol("modCompVolume", DD4hep::Geometry::Trapezoid(dx1, dx2, dyComp, dyComp, dz0),
			lcdd.material(xComp.materialStr()));
      modCompVol.setVisAttributes(lcdd, xComp.visStr());
      DD4hep::Geometry::Position offset(0, modCompZOffset + dyComp + xComp.y_offset() / 2, 0);      
      tiles.push_back(
                      layerVolume.placeVolume(modCompVol, offset) );
      
      if (xComp.isSensitive()) {
	modCompVol.setSensitiveDetector(sensDet);
	tiles.back().addPhysVolID("tile", idxActMod);
	idxActMod++;
      }
      modCompZOffset += xComp.thickness() + xComp.y_offset();
    }
    // Fill vector for DetElements
    tilesInLayers.push_back(tiles);
  }
  
  Volume moduleVolume1("moduleVolume1",
		       DD4hep::Geometry::Trapezoid(dx1Module1, dx2Module1, (dzDetector1 - dZEndPlate/2 - space/2),
						   (dzDetector1 - dZEndPlate/2 - space/2), dzModule1),
		       lcdd.material("Air"));
  moduleVolume1.setVisAttributes(lcdd.invisible());
  
  Volume moduleVolume2("moduleVolume2",
		       DD4hep::Geometry::Trapezoid(dx1Module2, dx2Module2, (dzDetector2 - dZEndPlate/2 - space/2),
						   (dzDetector2 - dZEndPlate/2 - space/2), dzModule2),
		       lcdd.material("Air"));
  moduleVolume2.setVisAttributes(lcdd.invisible());
  
  Volume steelSupportVolume1("steelSupportVolume1",
			     DD4hep::Geometry::Trapezoid(dx1Support1, dx2Support1, (dzDetector1 - dZEndPlate/2 - space/2),
							 (dzDetector1 - dZEndPlate/2 - space/2), dzSupport),
			     lcdd.material(xSteelSupport.materialStr()) );
  Volume steelSupportVolume2("steelSupportVolume2",
			     DD4hep::Geometry::Trapezoid(dx1Support2, dx2Support2, (dzDetector2 - dZEndPlate/2 - space/2),
							 (dzDetector2 - dZEndPlate/2 - space/2), dzSupport),
			     lcdd.material(xSteelSupport.materialStr()) );
  
  // Placement of rings
  for (unsigned int idxZRow = 0; idxZRow < numSequencesZ1; ++idxZRow) {
    double zOffset = - dzDetector1 + dZEndPlate/2 + space/2 + (2 * idxZRow + 1) * (dzSequence * 0.5);
    lLog << MSG::DEBUG << "z offset of wedges = " << zOffset << std::endl;
    
    DD4hep::Geometry::Position wedgeOffset(0, zOffset, 0);
    // Fill vector for DetElements
    rows.push_back( moduleVolume1.placeVolume(wedgeVolume1, wedgeOffset) );
    rows.back().addPhysVolID("row", idxZRow);
  }
  for (unsigned int idxZRow = 0; idxZRow < numSequencesZ2; ++idxZRow) {
    double zOffset = - dzDetector2 + dZEndPlate/2 + space/2 + (2 * idxZRow + 1) * (dzSequence * 0.5);
    lLog << MSG::DEBUG << "z offset of wedges = " << zOffset << std::endl;
    
    DD4hep::Geometry::Position wedgeOffset(0, zOffset, 0);
    // Fill vector for DetElements
    rows.push_back( moduleVolume2.placeVolume(wedgeVolume2, wedgeOffset) );
    rows.back().addPhysVolID("row", (numSequencesZ1 + idxZRow));
  }  

  for (unsigned int idxPhi = 0; idxPhi < numSequencesPhi; ++idxPhi) {
    double phi = 0.5 * dphi + idxPhi * dphi;  // 0.5*dphi for middle of module
    double yPosModule1  = (sensitiveBarrel1Rmin + dzModule1) * cos(phi);
    double xPosModule1  = (sensitiveBarrel1Rmin + dzModule1) * sin(phi);
    double yPosSupport1 = (sensitiveBarrel1Rmin + 2*dzModule1 + dzSupport) * cos(phi);
    double xPosSupport1 = (sensitiveBarrel1Rmin + 2*dzModule1 + dzSupport) * sin(phi);
    double yPosModule2  = (sensitiveBarrel2Rmin + dzModule2) * cos(phi);
    double xPosModule2  = (sensitiveBarrel2Rmin + dzModule2) * sin(phi);
    double yPosSupport2 = (sensitiveBarrel2Rmin + 2*dzModule2 + dzSupport) * cos(phi);
    double xPosSupport2 = (sensitiveBarrel2Rmin + 2*dzModule2 + dzSupport) * sin(phi);

    DD4hep::Geometry::Position moduleOffset1 (xPosModule1,  yPosModule1,  extBarrelOffset1 );
    DD4hep::Geometry::Position moduleOffset1_2 (xPosModule1,  yPosModule1,  -extBarrelOffset1 );
    DD4hep::Geometry::Position supportOffset1(xPosSupport1, yPosSupport1, extBarrelOffset1 );
    DD4hep::Geometry::Position supportOffset1_2(xPosSupport1, yPosSupport1, -extBarrelOffset1 );
    DD4hep::Geometry::Position moduleOffset2 (xPosModule2,  yPosModule2,  extBarrelOffset2 );
    DD4hep::Geometry::Position moduleOffset2_2 (xPosModule2,  yPosModule2,  -extBarrelOffset2 );
    DD4hep::Geometry::Position supportOffset2(xPosSupport2, yPosSupport2, extBarrelOffset2 );
    DD4hep::Geometry::Position supportOffset2_2(xPosSupport2, yPosSupport2, -extBarrelOffset2 );
    
    // Placement in front of central Barrel
    DD4hep::Geometry::Transform3D trans1(
					 DD4hep::Geometry::RotationX(-0.5 * dd4hep::pi) * DD4hep::Geometry::RotationY(phi), moduleOffset1);
    
    DD4hep::Geometry::Transform3D trans1S(
					  DD4hep::Geometry::RotationX(-0.5 * dd4hep::pi) * DD4hep::Geometry::RotationY(phi), supportOffset1);   
    
    DD4hep::Geometry::Transform3D trans2(
					 DD4hep::Geometry::RotationX(-0.5 * dd4hep::pi) * DD4hep::Geometry::RotationY(phi), moduleOffset2);
    
    DD4hep::Geometry::Transform3D trans2S(
					  DD4hep::Geometry::RotationX(-0.5 * dd4hep::pi) * DD4hep::Geometry::RotationY(phi), supportOffset2);   
    // Placement in the back of central Barrel
    DD4hep::Geometry::Transform3D trans1B( 
					  DD4hep::Geometry::RotationX(-0.5 * dd4hep::pi) * DD4hep::Geometry::RotationY(phi), moduleOffset1_2);
    
    DD4hep::Geometry::Transform3D trans1BS(
					   DD4hep::Geometry::RotationX(-0.5 * dd4hep::pi) * DD4hep::Geometry::RotationY(phi), supportOffset1_2);   
    
    DD4hep::Geometry::Transform3D trans2B(
					  DD4hep::Geometry::RotationX(-0.5 * dd4hep::pi) * DD4hep::Geometry::RotationY(phi), moduleOffset2_2);
    
    DD4hep::Geometry::Transform3D trans2BS(
					   DD4hep::Geometry::RotationX(-0.5 * dd4hep::pi) * DD4hep::Geometry::RotationY(phi), supportOffset2_2);   
    
    // Fill the vectors of DetElements    
    modules1.push_back( envelopeVolume.placeVolume(moduleVolume1, trans1) );
    modules1.back().addPhysVolID("module1_", idxPhi);
    modules1.push_back( envelopeVolume.placeVolume(moduleVolume1, trans1B) );
    modules1.back().addPhysVolID("module1_", numSequencesPhi + idxPhi);

    modules2.push_back( envelopeVolume.placeVolume(moduleVolume2, trans2) );
    modules2.back().addPhysVolID("module2_", idxPhi);
    modules2.push_back( envelopeVolume.placeVolume(moduleVolume2, trans2B) );
    modules2.back().addPhysVolID("module2_", numSequencesPhi + idxPhi);

    supports1.push_back( envelopeVolume.placeVolume(steelSupportVolume1, trans1S) );
    supports1.push_back( envelopeVolume.placeVolume(steelSupportVolume1, trans1BS) );
    supports2.push_back( envelopeVolume.placeVolume(steelSupportVolume2, trans2S) );
    supports2.push_back( envelopeVolume.placeVolume(steelSupportVolume2, trans2BS) );
  }

  // Placement of DetElements
  lLog << MSG::DEBUG <<  "Modules in phi : " << modules1.size() + modules2.size() << std::endl; 
  lLog << MSG::DEBUG <<  "Rows in z :      " << rows.size() << std::endl;
  lLog << MSG::DEBUG <<  "Layers in r :    " << layers.size() << std::endl;
  lLog << MSG::DEBUG <<  "Tiles in layers :" << tilesInLayers[1].size() << std::endl;

  for (uint iPhi = 0; iPhi < 2*numSequencesPhi; iPhi++) {
    DetElement moduleDet1(hCal,  DD4hep::XML::_toString(iPhi, "module1_%d"), iPhi);
    moduleDet1.setPlacement(modules1[iPhi]);
    DetElement support1(hCal, DD4hep::XML::_toString(iPhi, "support1_%d"), iPhi);
    support1.setPlacement(supports1[iPhi]);
    DetElement moduleDet2(hCal,  DD4hep::XML::_toString(iPhi, "module2_%d"), iPhi);
    moduleDet2.setPlacement(modules2[iPhi]);
    DetElement support2(hCal, DD4hep::XML::_toString(iPhi, "support2_%d"), iPhi);
    support2.setPlacement(supports2[iPhi]);
    
    for (uint iZ = 0; iZ < numSequencesZ1; iZ++) {
      DetElement wedgeDet(moduleDet1, DD4hep::XML::_toString(iZ, "row%d"), iZ); 
      wedgeDet.setPlacement(rows[iZ]);
      
      for (uint iLayer = 0; iLayer < numSequencesR1; iLayer++){
	DetElement layerDet(wedgeDet, DD4hep::XML::_toString(iLayer, "layer%d"), iLayer);
	layerDet.setPlacement(layers[iLayer]);
	
	for (uint iTile = 0; iTile < tilesInLayers[iLayer].size(); iTile++){
	  DetElement tileDet(layerDet, DD4hep::XML::_toString(iTile, "tile%d"), iTile);
	  tileDet.setPlacement(tilesInLayers[iLayer][iTile]);
	}
      }
    }
    for (uint iZ = numSequencesZ1; iZ < (numSequencesZ1 + numSequencesZ2); iZ++) {
      DetElement wedgeDet(moduleDet2, DD4hep::XML::_toString(iZ, "row%d"), iZ); 
      wedgeDet.setPlacement(rows[iZ]);
      
      for (uint iLayer = numSequencesR1; iLayer < (numSequencesR1 + numSequencesR2); iLayer++){
	DetElement layerDet(wedgeDet, DD4hep::XML::_toString(iLayer, "layer%d"), iLayer);
	layerDet.setPlacement(layers[iLayer]);
	
	for (uint iTile = 0; iTile < tilesInLayers[iLayer].size(); iTile++){
	  DetElement tileDet(layerDet, DD4hep::XML::_toString(iTile, "tile%d"), iTile);
	  tileDet.setPlacement(tilesInLayers[iLayer][iTile]);
	}
      }
    }
  }
  // Place envelope (or barrel) volume
  Volume motherVol = lcdd.pickMotherVolume(hCal);
  PlacedVolume placedHCal = motherVol.placeVolume(envelopeVolume);
  placedHCal.addPhysVolID("system", hCal.id());
  hCal.setPlacement(placedHCal);
  return hCal;
}
}  // namespace hcal

DECLARE_DETELEMENT(CaloExtBarrel, det::createHCal)
