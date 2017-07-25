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

  void buildEB(MsgStream& lLog, DD4hep::Geometry::LCDD& aLcdd, DD4hep::Geometry::SensitiveDetector& aSensDet,
	       DD4hep::Geometry::Volume& aEnvelope, DD4hep::Geometry::DetElement& aHCal, DD4hep::XML::Handle_t& aXmlElement, int sign) {
 
    Dimension sensDetType = aXmlElement.child(_Unicode(sensitive));
    aSensDet.setType(sensDetType.typeStr());
    
    Dimension dimensions(aXmlElement.child(_Unicode(dimensions)));
    xml_comp_t xEndPlate = aXmlElement.child(_Unicode(end_plate));
    double dZEndPlate = xEndPlate.thickness()/2.;
    xml_comp_t xFacePlate = aXmlElement.child(_Unicode(face_plate));
    double dRhoFacePlate = xFacePlate.thickness()/2.;
    xml_comp_t xSpace = aXmlElement.child(_Unicode(plate_space));  // to avoid overlaps
    double space = xSpace.thickness();
    xml_comp_t xSteelSupport = aXmlElement.child(_Unicode(steel_support));
    double dSteelSupport = xSteelSupport.thickness();
    lLog << MSG::DEBUG << "steel support thickness " << dSteelSupport << endmsg;
    lLog << MSG::DEBUG << "steel support material  " << xSteelSupport.materialStr() << endmsg;    
    double sensitiveBarrel1Rmin = dimensions.rmin1() + 2*dRhoFacePlate + space;
    double sensitiveBarrel2Rmin = dimensions.rmin2() + 2*dRhoFacePlate + space;
    
    // Offset in z is given as distance from 0 to the middle of the Calorimeter volume
    double extBarrelOffset1 = dimensions.offset();
    double extBarrelOffset2 = dimensions.z_offset();

    // Hard-coded assumption that we have two different sequences for the modules
    std::vector<xml_comp_t> sequences = {aXmlElement.child(_Unicode(sequence_a)), aXmlElement.child(_Unicode(sequence_b))};
    // NOTE: This assumes that both have the same dimensions!
    Dimension sequenceDimensions(sequences[1].dimensions());
    double dzSequence = sequenceDimensions.dz();
    lLog << MSG::DEBUG << "sequence thickness " << dzSequence << endmsg;
    
    // calculate the number of modules fitting in phi and Z
    unsigned int numSequencesPhi = sequenceDimensions.phiBins();
    double dphi = 2 * dd4hep::pi / static_cast<double>(numSequencesPhi);
    unsigned int numSequencesZ1 = static_cast<unsigned>((2*dimensions.width() - 2*dZEndPlate  - space) / dzSequence);
    unsigned int numSequencesZ2 = static_cast<unsigned>((2*dimensions.dz() - 2*dZEndPlate - space) / dzSequence);
    
    // Hard-coded assumption that we have three different layer types for the modules
    std::vector<xml_comp_t> Layers = {aXmlElement.child(_Unicode(layer_1)), aXmlElement.child(_Unicode(layer_2)), aXmlElement.child(_Unicode(layer_3))};
    unsigned int numSequencesR1 = 0;
    unsigned int numSequencesR2 = 0;
    double moduleDepth1 = 0.;
    double moduleDepth2 = 0.;
    std::vector<double> layerDepths1 = std::vector<double>();
    std::vector<double> layerDepths2 = std::vector<double>();
    // Loop over each type of layers (10, 15, 25 cm)
    for(std::vector<xml_comp_t>::iterator it = Layers.begin(); it != Layers.end(); ++it){
      xml_comp_t layer = *it;
      Dimension layerDimension(layer.dimensions());
      numSequencesR1 += layerDimension.nmodules();
      numSequencesR2 += layerDimension.nModules();
      for(int nLayer = 0; nLayer < layerDimension.nModules(); nLayer++){
	moduleDepth2 += layerDimension.dr();
	layerDepths2.push_back(layerDimension.dr());
      }
      for(int nLayer = 0; nLayer < layerDimension.nmodules(); nLayer++){
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
	 << layerDepths2.size() * numSequencesZ2 * numSequencesPhi << " tiles" << endmsg;
    
    lLog << MSG::INFO << "total number of channels: " << 2 * (numSequencesR2 * numSequencesZ2 * numSequencesPhi) + 2*(numSequencesR1 * numSequencesZ1 * numSequencesPhi) << endmsg;
    
    // Calculate correction along z based on the module size (can only have natural number of modules)
    double dzDetector1 = (numSequencesZ1 * dzSequence) / 2 + 2*dZEndPlate + space;
    lLog << MSG::INFO << "correction of dz (negative = size reduced) first part EB :" << dzDetector1*2 - dimensions.width() << endmsg;
    double dzDetector2 = (numSequencesZ2 * dzSequence) / 2 + 2*dZEndPlate + space;
    lLog << MSG::INFO << "correction of dz (negative = size reduced) second part EB:" << dzDetector2*2 - dimensions.dz() << endmsg;

    // Add structural support made of steel inside of HCal
    DetElement facePlate1(aHCal, "FacePlate_" + std::to_string(1*sign), 0);
    DD4hep::Geometry::Tube facePlateShape1(dimensions.rmin1(), (sensitiveBarrel1Rmin - space), (dzDetector1 - 2*dZEndPlate - space));
    Volume facePlateVol1("facePlateVol1", facePlateShape1, aLcdd.material(xFacePlate.materialStr()));
    //facePlateVol1.setVisAttributes(aLcdd, xFacePlate.visStr());
    DD4hep::Geometry::Position offsetFace1(0, 0, sign*(-dzDetector2));
   
    // Faceplate for 2nd part of extended Barrel
    DetElement facePlate2(aHCal, "FacePlate_" + std::to_string(2*sign), 0);
    DD4hep::Geometry::Tube facePlateShape2(dimensions.rmin2(), (sensitiveBarrel2Rmin - space), (dzDetector2 - 2*dZEndPlate - space));
    Volume facePlateVol2("facePlateVol2", facePlateShape2, aLcdd.material(xFacePlate.materialStr()));
    //facePlateVol2.setVisAttributes(aLcdd, xFacePlate.visStr());
    DD4hep::Geometry::Position offsetFace2(0, 0, sign*(dzDetector1));

    PlacedVolume placedFacePlate1 = aEnvelope.placeVolume(facePlateVol1, offsetFace1);
    facePlate1.setPlacement(placedFacePlate1);
    PlacedVolume placedFacePlate2 = aEnvelope.placeVolume(facePlateVol2, offsetFace2);
    facePlate2.setPlacement(placedFacePlate2);
    
    // Add structural support made of steel at both ends of extHCal
    DD4hep::Geometry::Tube endPlateShape1(dimensions.rmin1(), (dimensions.rmax1() - dSteelSupport), dZEndPlate);
    Volume endPlateVol1("endPlateVol1", endPlateShape1, aLcdd.material(xEndPlate.materialStr()));
    endPlateVol1.setVisAttributes(aLcdd, xEndPlate.visStr());
    DD4hep::Geometry::Tube endPlateShape2(dimensions.rmin2(), (dimensions.rmax2() - dSteelSupport), dZEndPlate);
    Volume endPlateVol2("endPlateVol2", endPlateShape2, aLcdd.material(xEndPlate.materialStr()));
    endPlateVol2.setVisAttributes(aLcdd, xEndPlate.visStr());

    // Endplates placed for the extended Barrels in front and in the back to the central Barrel
    DetElement endPlatePos(aHCal, "endPlate_" + std::to_string(1*sign), 0);
    DD4hep::Geometry::Position posOffset(0, 0, sign*(dzDetector1 + dzDetector2 - dZEndPlate));
    PlacedVolume placedEndPlatePos = aEnvelope.placeVolume(endPlateVol2, posOffset);
    endPlatePos.setPlacement(placedEndPlatePos);

    DetElement endPlateNeg(aHCal, "endPlate_" + std::to_string(2*sign), 0);
    DD4hep::Geometry::Position negOffset(0, 0, sign*(-dzDetector2 - dzDetector1 + dZEndPlate));
    PlacedVolume placedEndPlateNeg = aEnvelope.placeVolume(endPlateVol1, negOffset);
    endPlateNeg.setPlacement(placedEndPlateNeg);
    
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
    double dzSupport = dSteelSupport / 2.;
    
    // DetElement vectors for placement in loop at the end
    std::vector<DD4hep::Geometry::PlacedVolume> supports1;
    supports1.reserve(numSequencesPhi);
    std::vector<DD4hep::Geometry::PlacedVolume> modules1;
    modules1.reserve(numSequencesPhi);
    std::vector<DD4hep::Geometry::PlacedVolume> supports2;
    supports2.reserve(numSequencesPhi);
    std::vector<DD4hep::Geometry::PlacedVolume> modules2;
    modules2.reserve(numSequencesPhi);
    std::vector<DD4hep::Geometry::PlacedVolume> rows;
    rows.reserve(numSequencesZ1 + numSequencesZ2);
    std::vector<DD4hep::Geometry::PlacedVolume> layers;
    layers.reserve(layerDepths1.size() + layerDepths2.size());
    std::vector<std::vector<DD4hep::Geometry::PlacedVolume> > tilesInLayers;
    tilesInLayers.reserve(layerDepths1.size() + layerDepths2.size());
    
    // First we construct base wedges:
    Volume wedgeVolume1("wedgeVolume_"+ std::to_string(1*sign), DD4hep::Geometry::Trapezoid(dx1Module1, dx2Module1, dy0, dy0, dzModule1),
			aLcdd.material("Air"));  
    Volume wedgeVolume2("wedgeVolume_"+ std::to_string(2*sign), DD4hep::Geometry::Trapezoid(dx1Module2, dx2Module2, dy0, dy0, dzModule2),
			aLcdd.material("Air"));  
    double layerR=0.;
    // Placement of subWedges in Wedge
    for (unsigned int idxLayer = 0; idxLayer < layerDepths1.size(); ++idxLayer) {
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
      
      Volume layerVolume("layerVolume", DD4hep::Geometry::Trapezoid(dx1, dx2, dy0, dy0, dz0), aLcdd.material("Air"));
      //layerVolume.setVisAttributes(aLcdd.invisible());
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
			  aLcdd.material(xComp.materialStr()));
	//modCompVol.setVisAttributes(aLcdd, xComp.visStr());
	DD4hep::Geometry::Position offset(0, modCompZOffset + dyComp + xComp.y_offset() / 2, 0);     
	
	if (xComp.isSensitive()) {
	  Volume tileVol("tileVolume", DD4hep::Geometry::Trapezoid(dx1, dx2, dyComp, dyComp, dz0),
			 aLcdd.material(xComp.materialStr()));
	  // tileVol.setVisAttributes(aLcdd, xComp.visStr());
	  tileVol.setSensitiveDetector(aSensDet);
	  tiles.push_back( layerVolume.placeVolume(modCompVol, offset) );
	  tiles.back().addPhysVolID("tile", 1);
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

    layerR = 0.;
    // Placement of subWedges in Wedge, 2nd part 
    for (unsigned int idxLayer = 0; idxLayer < layerDepths2.size(); ++idxLayer) {
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
      
      Volume layerVolume("layerVolume", DD4hep::Geometry::Trapezoid(dx1, dx2, dy0, dy0, dz0), aLcdd.material("Air"));
      //      layerVolume.setVisAttributes(aLcdd.invisible());
      unsigned int idxSubMod = 0;
      unsigned int idxActMod = 0;
      double modCompZOffset = -sequenceDimensions.dz() * 0.5;
      
      // layer middle offset
      DD4hep::Geometry::Position modOffset(0, 0, rMiddle);
      layers.push_back( wedgeVolume2.placeVolume(layerVolume, modOffset) );
      layers.back().addPhysVolID("layer", (layerDepths1.size()+idxLayer));
      
      std::vector<DD4hep::Geometry::PlacedVolume> tiles;
      // Filling of the subWedge with coponents (submodules)
      for (xml_coll_t xCompColl(sequences[sequenceIdx], _Unicode(module_component)); xCompColl; ++xCompColl, ++idxSubMod) {
	xml_comp_t xComp = xCompColl;
	double dyComp = xComp.thickness() * 0.5;
	Volume modCompVol("modCompVolume", DD4hep::Geometry::Trapezoid(dx1, dx2, dyComp, dyComp, dz0),
			  aLcdd.material(xComp.materialStr()));
	//modCompVol.setVisAttributes(aLcdd, xComp.visStr());
	DD4hep::Geometry::Position offset(0, modCompZOffset + dyComp + xComp.y_offset() / 2, 0);      
	
	if (xComp.isSensitive()) {
	  Volume tileVol("tileVolume", DD4hep::Geometry::Trapezoid(dx1, dx2, dyComp, dyComp, dz0),
			 aLcdd.material(xComp.materialStr()));
	  //tileVol.setVisAttributes(aLcdd, xComp.visStr());
	  tileVol.setSensitiveDetector(aSensDet);
	  tiles.push_back( layerVolume.placeVolume(modCompVol, offset) );
	  tiles.back().addPhysVolID("tile", 2);
	  idxActMod++;
	}
	else{
	  tiles.push_back(
			  layerVolume.placeVolume(modCompVol, offset) );
	}
	modCompZOffset += xComp.thickness() + xComp.y_offset();
      }
      // Fill vector for DetElements
      tilesInLayers.push_back(tiles);
    }
    
    Volume moduleVolume1("moduleVolume_"+ std::to_string(1*sign),
			 DD4hep::Geometry::Trapezoid(dx1Module1, dx2Module1, (dzDetector1 - 2*dZEndPlate - space),
						     (dzDetector1 - 2*dZEndPlate - space), dzModule1),
			 aLcdd.material("Air"));
    //moduleVolume1.setVisAttributes(aLcdd.invisible());
    
    Volume moduleVolume2("moduleVolume_"+ std::to_string(2*sign),
			 DD4hep::Geometry::Trapezoid(dx1Module2, dx2Module2, (dzDetector2 - 2*dZEndPlate - space),
						     (dzDetector2 - 2*dZEndPlate - space), dzModule2),
			 aLcdd.material("Air"));
    //moduleVolume2.setVisAttributes(aLcdd.invisible());
    
    Volume steelSupportVolume1("steelSupportVolume_"+ std::to_string(1*sign),
			       DD4hep::Geometry::Trapezoid(dx1Support1, dx2Support1, (dzDetector1 - 2*dZEndPlate - space),
							   (dzDetector1 - 2*dZEndPlate - space), dzSupport),
			       aLcdd.material(xSteelSupport.materialStr()) );
    //steelSupportVolume1.setVisAttributes(aLcdd, xSteelSupport.visStr());
    
    Volume steelSupportVolume2("steelSupportVolume_"+ std::to_string(2*sign),
			       DD4hep::Geometry::Trapezoid(dx1Support2, dx2Support2, (dzDetector2 - 2*dZEndPlate - space),
							   (dzDetector2 - 2*dZEndPlate - space), dzSupport),
			       aLcdd.material(xSteelSupport.materialStr()) );
    //steelSupportVolume2.setVisAttributes(aLcdd, xSteelSupport.visStr());
     
    // Placement of rings
    for (unsigned int idxZRow = 0; idxZRow < numSequencesZ1; ++idxZRow) {
      double zOffset = - dzDetector1 + 2*dZEndPlate + space + (2 * idxZRow + 1) * (dzSequence * 0.5);
      lLog << MSG::DEBUG << "z offset of wedges = " << zOffset << std::endl;
      
      DD4hep::Geometry::Position wedgeOffset(0, zOffset, 0);
      // Fill vector for DetElements
      rows.push_back( moduleVolume1.placeVolume(wedgeVolume1, wedgeOffset) );
      rows.back().addPhysVolID("row", idxZRow);
    }
    for (unsigned int idxZRow = 0; idxZRow < numSequencesZ2; ++idxZRow) {
      double zOffset = - dzDetector2 + 2*dZEndPlate + space + (2 * idxZRow + 1) * (dzSequence * 0.5);
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
      
      // Placement of type 1 modules is shifted in opposite direction as modules type2 due to opposite placement of endplates
      // Placement within envelopeVolumeNeg and Pos (--> total offset in z later)
      // --> Offsets calculated relative to middle of envelope in z
      DD4hep::Geometry::Position moduleOffset1 (xPosModule1,  yPosModule1, sign*( -dzDetector2 + 2*dZEndPlate + space));//extBarrelOffset1 ));
      DD4hep::Geometry::Position supportOffset1(xPosSupport1, yPosSupport1, sign*( -dzDetector2 + 2*dZEndPlate + space));
      DD4hep::Geometry::Position moduleOffset2 (xPosModule2,  yPosModule2, sign*( dzDetector1 - 2*dZEndPlate - space));
      DD4hep::Geometry::Position supportOffset2(xPosSupport2, yPosSupport2, sign*( dzDetector1 - 2*dZEndPlate - space));
      
      // Placement around central Barrel
      DD4hep::Geometry::Transform3D trans1(
					   DD4hep::Geometry::RotationX(-0.5 * dd4hep::pi) * DD4hep::Geometry::RotationY(phi), moduleOffset1);
      
      DD4hep::Geometry::Transform3D trans1S(
					    DD4hep::Geometry::RotationX(-0.5 * dd4hep::pi) * DD4hep::Geometry::RotationY(phi), supportOffset1);   
      
      DD4hep::Geometry::Transform3D trans2(
					   DD4hep::Geometry::RotationX(-0.5 * dd4hep::pi) * DD4hep::Geometry::RotationY(phi), moduleOffset2);
      
      DD4hep::Geometry::Transform3D trans2S(
					    DD4hep::Geometry::RotationX(-0.5 * dd4hep::pi) * DD4hep::Geometry::RotationY(phi), supportOffset2);   
      
      // Fill the vectors of DetElements    
      modules1.push_back( aEnvelope.placeVolume(moduleVolume1, trans1) );
      modules1.back().addPhysVolID("module1_", idxPhi);
      //      modules1.back().addPhysVolID("module2_", 0);
      modules2.push_back( aEnvelope.placeVolume(moduleVolume2, trans2) );
      modules2.back().addPhysVolID("module2_", idxPhi);
      //      modules2.back().addPhysVolID("module1_", 0);
      
      supports1.push_back( aEnvelope.placeVolume(steelSupportVolume1, trans1S) );
      supports2.push_back( aEnvelope.placeVolume(steelSupportVolume2, trans2S) );
    }
    
    // Placement of DetElements
    lLog << MSG::DEBUG <<  "Modules in phi : " << modules1.size() + modules2.size() << std::endl; 
    lLog << MSG::DEBUG <<  "Rows in z :      " << rows.size() << std::endl;
    lLog << MSG::DEBUG <<  "Layers in r :    " << layers.size() << std::endl;
    lLog << MSG::DEBUG <<  "Tiles in layers :" << tilesInLayers[1].size() << std::endl;
    
    for (uint iPhi = 0; iPhi < numSequencesPhi; iPhi++) {
     DetElement moduleDet1(aHCal,  DD4hep::XML::_toString(sign*iPhi, "module1_%d"), iPhi);
     moduleDet1.setPlacement(modules1[iPhi]);
     DetElement support1(aHCal, DD4hep::XML::_toString(sign*iPhi, "support1_%d"), iPhi);
     support1.setPlacement(supports1[iPhi]);
     DetElement moduleDet2(aHCal,  DD4hep::XML::_toString(sign*iPhi, "module2_%d"), iPhi);
     moduleDet2.setPlacement(modules2[iPhi]);
     DetElement support2(aHCal, DD4hep::XML::_toString(sign*iPhi, "support2_%d"), iPhi);
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
    return;   
  }
  
  static DD4hep::Geometry::Ref_t
  createHCal(DD4hep::Geometry::LCDD& lcdd, 
	     xml_h xmlElement, 
	     DD4hep::Geometry::SensitiveDetector sensDet) {
    
    // Get the Gaudi message service and message stream:
    ServiceHandle<IMessageSvc> msgSvc("MessageSvc", "ExtHCalConstruction");
    MsgStream lLog(&(*msgSvc), "ExtHCalConstruction");
    
    xml_det_t xmlDet = xmlElement;
    std::string detName = xmlDet.nameStr();
    
    // Make DetElement 
    DD4hep::Geometry::DetElement hCal(detName, xmlDet.id());
    DD4hep::Geometry::DetElement posHCal(hCal, "positive", 0);
    DD4hep::Geometry::DetElement negHCal(hCal, "negative", 0);
    
    // Make volume that envelopes the whole barrel; set material to air
    Dimension dimensions(xmlDet.dimensions());

    // Envelope Volumes for positive and negative EB
    DD4hep::Geometry::Tube envelopeShapeNeg(dimensions.rmin2(), dimensions.rmax1(), (dimensions.width()+dimensions.dz()));
    DD4hep::Geometry::Tube envelopeShapePos(dimensions.rmin2(), dimensions.rmax1(), (dimensions.width()+dimensions.dz()));
    DD4hep::Geometry::UnionSolid envelopeShape(envelopeShapeNeg, envelopeShapePos, DD4hep::Geometry::Position(0,0, 2*(dimensions.offset() + dimensions.dz())) );

    Volume envelopeVolume(detName + "_volume", envelopeShape, lcdd.air());
    envelopeVolume.setVisAttributes(lcdd, dimensions.visStr());
    Volume envelopeVolumePos(detName + "_posVolume", envelopeShapePos, lcdd.air());
    envelopeVolumePos.setVisAttributes(lcdd, dimensions.visStr()); 
    Volume envelopeVolumeNeg(detName + "_negVolume", envelopeShapeNeg, lcdd.air());
    envelopeVolumeNeg.setVisAttributes(lcdd, dimensions.visStr());
  
    buildEB(lLog, lcdd, sensDet, envelopeVolumePos, posHCal, xmlElement, 1);
    buildEB(lLog, lcdd, sensDet, envelopeVolumeNeg, negHCal, xmlElement, -1);

    // Place envelope volume                                        
    Volume motherVol = lcdd.pickMotherVolume(hCal);

    PlacedVolume placedEnvelopePosVol = envelopeVolume.placeVolume(envelopeVolumePos, DD4hep::Geometry::Position(0,0, 2*(dimensions.offset() + dimensions.dz())) );
    placedEnvelopePosVol.addPhysVolID("subsystem",0);
    posHCal.setPlacement(placedEnvelopePosVol);

    PlacedVolume placedEnvelopeNegVol = envelopeVolume.placeVolume(envelopeVolumeNeg);
    placedEnvelopeNegVol.addPhysVolID("subsystem",1);
    negHCal.setPlacement(placedEnvelopeNegVol);
    
    PlacedVolume placedHCal = motherVol.placeVolume(envelopeVolume, DD4hep::Geometry::Position(0,0, -(dimensions.offset() + dimensions.dz())) );
    placedHCal.addPhysVolID("system", xmlDet.id());
    hCal.setPlacement(placedHCal);

    return hCal;
  }
}  // namespace hcal

DECLARE_DETELEMENT(CaloExtBarrel, det::createHCal)
