// DD4hep
#include "DD4hep/DetFactoryHelper.h"

// Gaudi
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

using dd4hep::Volume;
using dd4hep::DetElement;
using dd4hep::xml::Dimension;
using dd4hep::PlacedVolume;

namespace det {
void buildEB(MsgStream& lLog, dd4hep::Detector& aLcdd, dd4hep::SensitiveDetector& aSensDet, dd4hep::Volume& aEnvelope,
             dd4hep::DetElement& aHCal, xml_det_t aXmlElement, int sign) {

  dd4hep::SensitiveDetector sensDet = aSensDet;
  Dimension sensDetType = aXmlElement.child(_Unicode(sensitive));
  sensDet.setType(sensDetType.typeStr());

  Dimension dimensions(aXmlElement.child(_Unicode(dimensions)));
  xml_comp_t xEndPlate = aXmlElement.child(_Unicode(end_plate));
  double dZEndPlate = xEndPlate.thickness() / 2.;
  xml_comp_t xFacePlate = aXmlElement.child(_Unicode(face_plate));
  double dRhoFacePlate = xFacePlate.thickness() / 2.;
  xml_comp_t xSpace = aXmlElement.child(_Unicode(plate_space));  // to avoid overlaps
  double space = xSpace.thickness();
  xml_comp_t xSteelSupport = aXmlElement.child(_Unicode(steel_support));
  double dSteelSupport = xSteelSupport.thickness();
  lLog << MSG::DEBUG << "steel support thickness " << dSteelSupport << endmsg;
  lLog << MSG::DEBUG << "steel support material  " << xSteelSupport.materialStr() << endmsg;

  double sensitiveBarrel1Rmin = dimensions.rmin1() + 2 * dRhoFacePlate + space;
  double sensitiveBarrel2Rmin = dimensions.rmin2() + 2 * dRhoFacePlate + space;

  // Offset in z is given as distance from 0 to the middle of the Calorimeter volume
  double extBarrelOffset1 = dimensions.offset();
  double extBarrelOffset2 = dimensions.z_offset();

  // Hard-coded assumption that we have two different sequences for the modules
  std::vector<xml_comp_t> sequences = {aXmlElement.child(_Unicode(sequence_a)), aXmlElement.child(_Unicode(sequence_b))};
  // NOTE: This assumes that both have the same dimensions!
  Dimension sequenceDimensions(sequences[1].dimensions());
  double dzSequence = sequenceDimensions.dz();
  lLog << MSG::DEBUG << "sequence thickness " << dzSequence << endmsg;

  // calculate the number of modules fitting in  Z
  unsigned int numSequencesZ1 = static_cast<unsigned>((2 * dimensions.width() - 2 * dZEndPlate - space) / dzSequence);
  unsigned int numSequencesZ2 = static_cast<unsigned>((2 * dimensions.dz() - 2 * dZEndPlate - space) / dzSequence);

  // Hard-coded assumption that we have three different layer types for the modules
  std::vector<xml_comp_t> Layers = {aXmlElement.child(_Unicode(layer_1)), aXmlElement.child(_Unicode(layer_2)),
                                    aXmlElement.child(_Unicode(layer_3))};
  unsigned int numSequencesR1 = 0;
  unsigned int numSequencesR2 = 0;
  double moduleDepth1 = 0.;
  double moduleDepth2 = 0.;
  std::vector<double> layerDepths1 = std::vector<double>();
  std::vector<double> layerDepths2 = std::vector<double>();
  // Loop over each type of layers (10, 15, 25 cm)
  for (std::vector<xml_comp_t>::iterator it = Layers.begin(); it != Layers.end(); ++it) {
    xml_comp_t layer = *it;
    Dimension layerDimension(layer.dimensions());
    numSequencesR1 += layerDimension.nmodules();
    numSequencesR2 += layerDimension.nModules();
    for (int nLayer = 0; nLayer < layerDimension.nModules(); nLayer++) {
      moduleDepth2 += layerDimension.dr();
      layerDepths2.push_back(layerDimension.dr());
    }
    for (int nLayer = 0; nLayer < layerDimension.nmodules(); nLayer++) {
      moduleDepth1 += layerDimension.dr();
      layerDepths1.push_back(layerDimension.dr());
    }
  }
  lLog << MSG::DEBUG << "retrieved number of layers in first ExtBarrel part:  " << numSequencesR1
       << " , which end up to a full module depth in rho of " << moduleDepth1 << endmsg;
  lLog << MSG::DEBUG << "retrieved number of layers in first ExtBarrel part:  " << layerDepths1.size() << endmsg;
  lLog << MSG::DEBUG << "retrieved number of layers in second ExtBarrel part:  " << numSequencesR2
       << " , which end up to a full module depth in rho of " << moduleDepth2 << endmsg;
  lLog << MSG::DEBUG << "retrieved number of layers in second ExtBarrel part:  " << layerDepths2.size() << endmsg;

  lLog << MSG::INFO << "constructing first part EB: with offset " << extBarrelOffset1 << ": "<< numSequencesZ1
       << " rings in Z, " << numSequencesR1 << " layers in Rho, " << numSequencesR1 * numSequencesZ1
       << " tiles" << endmsg;

  lLog << MSG::INFO << "constructing second part EB: with offset " << extBarrelOffset2 << ": " << numSequencesZ2
       << " rings in Z, " << numSequencesR2 << " layers in Rho, "
       << layerDepths2.size() * numSequencesZ2 << " tiles" << endmsg;

  lLog << MSG::INFO << "number of channels: "
       << (numSequencesR2 * numSequencesZ2) + (numSequencesR1 * numSequencesZ1)
       << endmsg;

  // Calculate correction along z based on the module size (can only have natural number of modules)
  double dzDetector1 = (numSequencesZ1 * dzSequence) / 2 + 2 * dZEndPlate + space;
  lLog << MSG::INFO
       << "correction of dz (negative = size reduced) first part EB :" << dzDetector1*2 - dimensions.width()*2
       << endmsg;
  double dzDetector2 = (numSequencesZ2 * dzSequence) / 2 + 2 * dZEndPlate + space;
  lLog << MSG::INFO << "dz second part EB:" << dzDetector2 * 2
       << endmsg;
  lLog << MSG::INFO << "width second part EB:" << dimensions.dz() * 2
       << endmsg;
  lLog << MSG::INFO << "correction of dz (negative = size reduced) second part EB:" << dzDetector2*2 - dimensions.dz()*2
       << endmsg;

  // Add structural support made of steel inside of HCal
  DetElement facePlate1(aHCal, "FacePlate_" + std::to_string(1 * sign), 0);
  dd4hep::Tube facePlateShape1(dimensions.rmin1(), (sensitiveBarrel1Rmin - space),
                               (dzDetector1 - 2 * dZEndPlate - space));
  Volume facePlateVol1("facePlateVol1", facePlateShape1, aLcdd.material(xFacePlate.materialStr()));
  facePlateVol1.setVisAttributes(aLcdd, xFacePlate.visStr());
  dd4hep::Position offsetFace1(0, 0, sign * extBarrelOffset1);

  // Faceplate for 2nd part of extended Barrel
  DetElement facePlate2(aHCal, "FacePlate_" + std::to_string(2 * sign), 0);
  dd4hep::Tube facePlateShape2(dimensions.rmin2(), (sensitiveBarrel2Rmin - space),
                               (dzDetector2 - 2 * dZEndPlate - space));
  Volume facePlateVol2("facePlateVol2", facePlateShape2, aLcdd.material(xFacePlate.materialStr()));
  facePlateVol2.setVisAttributes(aLcdd, xFacePlate.visStr());
  dd4hep::Position offsetFace2(0, 0, sign * extBarrelOffset2);

  PlacedVolume placedFacePlate1 = aEnvelope.placeVolume(facePlateVol1, offsetFace1);
  facePlate1.setPlacement(placedFacePlate1);
  PlacedVolume placedFacePlate2 = aEnvelope.placeVolume(facePlateVol2, offsetFace2);
  facePlate2.setPlacement(placedFacePlate2);

  // Add structural support made of steel at both ends of extHCal
  dd4hep::Tube endPlateShape1(dimensions.rmin1(), (dimensions.rmax1() - dSteelSupport), dZEndPlate);
  Volume endPlateVol1("endPlateVol1", endPlateShape1, aLcdd.material(xEndPlate.materialStr()));
  endPlateVol1.setVisAttributes(aLcdd, xEndPlate.visStr());
  dd4hep::Tube endPlateShape2(dimensions.rmin2(), (dimensions.rmax2() - dSteelSupport), dZEndPlate);
  Volume endPlateVol2("endPlateVol2", endPlateShape2, aLcdd.material(xEndPlate.materialStr()));
  endPlateVol2.setVisAttributes(aLcdd, xEndPlate.visStr());

  // Endplates placed for the extended Barrels in front and in the back to the central Barrel
  DetElement endPlatePos(aHCal, "endPlate_" + std::to_string(1 * sign), 0);
  dd4hep::Position posOffset(0, 0, sign * (extBarrelOffset2 + dzDetector2 - dZEndPlate));
  PlacedVolume placedEndPlatePos = aEnvelope.placeVolume(endPlateVol2, posOffset);
  endPlatePos.setPlacement(placedEndPlatePos);

  DetElement endPlateNeg(aHCal, "endPlate_" + std::to_string(2 * sign), 0);
  dd4hep::Position negOffset(0, 0, sign * (extBarrelOffset1 - dzDetector1 + dZEndPlate));
  PlacedVolume placedEndPlateNeg = aEnvelope.placeVolume(endPlateVol1, negOffset);
  endPlateNeg.setPlacement(placedEndPlateNeg);

  std::vector<dd4hep::PlacedVolume> layers;
  layers.reserve(layerDepths1.size()+layerDepths2.size());
  std::vector<std::vector<dd4hep::PlacedVolume> > seqInLayers;
  seqInLayers.reserve(layerDepths1.size()+layerDepths2.size());
  std::vector<dd4hep::PlacedVolume> tilesPerLayer;
  tilesPerLayer.reserve(layerDepths1.size()+layerDepths2.size());

  // loop over R ("layers")
  double layerR = 0.;
  for (unsigned int idxLayer = 0; idxLayer < layerDepths1.size(); ++idxLayer) {
    // in Module rmin = 0  for first wedge, changed radius to the full radius starting at (0,0,0)
    double rminLayer = sensitiveBarrel1Rmin + layerR;
    double rmaxLayer = sensitiveBarrel1Rmin + layerR + layerDepths1.at(idxLayer);
    layerR += layerDepths1.at(idxLayer);

    //alternate: even layers consist of tile sequence b, odd layer of tile sequence a
    unsigned int sequenceIdx = idxLayer % 2;
    
    dd4hep::Tube tileSequenceShape(rminLayer, rmaxLayer, 0.5*dzSequence);
    Volume tileSequenceVolume("HCalEBTileSequenceVol1", tileSequenceShape, aLcdd.air());

    lLog << MSG::DEBUG << "layer radii:  " << rminLayer << " - " << rmaxLayer << " [cm]" << endmsg;
    

    dd4hep::Tube layerShape(rminLayer, rmaxLayer, dzDetector1 );
    Volume layerVolume("HCalEBLayerVol1", layerShape, aLcdd.air());

  
    layerVolume.setVisAttributes(aLcdd.invisible());
    unsigned int idxSubMod = 0;
    unsigned int idxActMod = 0;    

    dd4hep::Position moduleOffset1 (0,0,sign * extBarrelOffset1);

    dd4hep::PlacedVolume placedLayerVolume = aEnvelope.placeVolume(layerVolume, moduleOffset1);
    placedLayerVolume.addPhysVolID("type", 1 - sign);  // First module type=0,2 in front of second +/-
    placedLayerVolume.addPhysVolID("layer", idxLayer);
    layers.push_back(placedLayerVolume);
   
    double tileZOffset = - 0.5* dzSequence;

    // first Z loop (tiles that make up a sequence)
    for (xml_coll_t xCompColl(sequences[sequenceIdx], _Unicode(module_component)); xCompColl;
	 ++xCompColl, ++idxSubMod) {
      xml_comp_t xComp = xCompColl;
      dd4hep::Tube tileShape(rminLayer, rmaxLayer, 0.5 * xComp.thickness());
      
      Volume tileVol("HCalEBTileVol_"+ xComp.materialStr()
		     , tileShape,
		     aLcdd.material(xComp.materialStr()));
      tileVol.setVisAttributes(aLcdd, xComp.visStr());
      
      dd4hep::Position tileOffset(0, 0, tileZOffset + 0.5 * xComp.thickness() );
      dd4hep::PlacedVolume placedTileVol = tileSequenceVolume.placeVolume(tileVol, tileOffset);
      
      if (xComp.isSensitive()) {
        tileVol.setSensitiveDetector(sensDet);
        tilesPerLayer.push_back(placedTileVol);
	idxActMod++;
      }
      tileZOffset += xComp.thickness();
    }

    // second z loop (place sequences in layer)
    std::vector<dd4hep::PlacedVolume> seqs; 
    double zOffset = - dzDetector1 + dZEndPlate + space/2 + 0.5 * dzSequence;
    
    for (uint numSeq=0; numSeq < numSequencesZ1; numSeq++){
      dd4hep::Position tileSequencePosition(0, 0, zOffset);
      dd4hep::PlacedVolume placedTileSequenceVolume = layerVolume.placeVolume(tileSequenceVolume, tileSequencePosition);
      placedTileSequenceVolume.addPhysVolID("row", numSeq);
      seqs.push_back(placedTileSequenceVolume);
      zOffset += dzSequence;
    }
    seqInLayers.push_back(seqs);
  }

  layerR = 0.;
  // Placement of subWedges in Wedge, 2nd part
  for (unsigned int idxLayer = 0; idxLayer < layerDepths2.size(); ++idxLayer) {
    // in Module rmin = 0  for first wedge, changed radius to the full radius starting at (0,0,0)                                                                      
    double rminLayer = sensitiveBarrel2Rmin + layerR;
    double rmaxLayer = sensitiveBarrel2Rmin + layerR + layerDepths2.at(idxLayer);
    layerR += layerDepths2.at(idxLayer);

    //alternate: even layers consist of tile sequence b, odd layer of tile sequence a                                                                                  
    unsigned int sequenceIdx = idxLayer % 2;

    dd4hep::Tube tileSequenceShape(rminLayer, rmaxLayer, 0.5*dzSequence);
    Volume tileSequenceVolume("HCalEBTileSequenceVol2", tileSequenceShape, aLcdd.air());

    lLog << MSG::DEBUG << "layer radii:  " << rminLayer << " - " << rmaxLayer << " [cm]" << endmsg;


    dd4hep::Tube layerShape(rminLayer, rmaxLayer, dzDetector2);
    Volume layerVolume("HCalEBLayerVol2", layerShape, aLcdd.air());

    layerVolume.setVisAttributes(aLcdd.invisible());
    unsigned int idxSubMod = 0;
    unsigned int idxActMod = 0;

    double tileZOffset = - 0.5* dzSequence;

    // first Z loop (tiles that make up a sequence)                                                                                                                    
    for (xml_coll_t xCompColl(sequences[sequenceIdx], _Unicode(module_component)); xCompColl;
         ++xCompColl, ++idxSubMod) {
      xml_comp_t xComp = xCompColl;
      dd4hep::Tube tileShape(rminLayer, rmaxLayer, 0.5 * xComp.thickness());

      Volume tileVol("HCalEBTileVol_"
		     , tileShape,
                     aLcdd.material(xComp.materialStr()));
      tileVol.setVisAttributes(aLcdd, xComp.visStr());

      dd4hep::Position tileOffset(0, 0, tileZOffset + 0.5 * xComp.thickness() );
      dd4hep::PlacedVolume placedTileVol = tileSequenceVolume.placeVolume(tileVol, tileOffset);

      if (xComp.isSensitive()) {
        tileVol.setSensitiveDetector(sensDet);
        tilesPerLayer.push_back(placedTileVol);
        idxActMod++;
      }
      tileZOffset += xComp.thickness();
    }

    // second z loop (place sequences in layer)                                                                                                                        
    std::vector<dd4hep::PlacedVolume> seqs;
    double zOffset = - dzDetector2 + dZEndPlate + space/2. + (dzSequence * 0.5);

    for (uint numSeq=0; numSeq < numSequencesZ2; numSeq++){
      dd4hep::Position tileSequencePosition(0, 0, zOffset);
      dd4hep::PlacedVolume placedTileSequenceVolume = layerVolume.placeVolume(tileSequenceVolume, tileSequencePosition);
      placedTileSequenceVolume.addPhysVolID("row", numSeq);
      seqs.push_back(placedTileSequenceVolume);
      zOffset += dzSequence;
    }
    seqInLayers.push_back(seqs);

    dd4hep::Position moduleOffset2 (0, 0, sign * extBarrelOffset2);
    dd4hep::PlacedVolume placedLayerVolume = aEnvelope.placeVolume(layerVolume, moduleOffset2);
    placedLayerVolume.addPhysVolID("type", 2 - sign);  // Second module type=1,3 behind the first +/-
    placedLayerVolume.addPhysVolID("layer", idxLayer);
    layers.push_back(placedLayerVolume);

  }

  // Placement of DetElements
  lLog << MSG::DEBUG << "Layers in r :    " << layers.size() << std::endl;
  lLog << MSG::DEBUG << "Tiles in layers :" << tilesPerLayer.size() << std::endl;
  
  for (uint iLayer = 0; iLayer < (layerDepths1.size()+layerDepths2.size()); iLayer++) {
    DetElement layerDet(aHCal, dd4hep::xml::_toString(sign*(iLayer+1), "layer%d"), sign*(iLayer+1));
    layerDet.setPlacement(layers[iLayer]);
    
    for (uint iSeq = 0; iSeq < seqInLayers[iLayer].size(); iSeq++){
      DetElement seqDet(layerDet, dd4hep::xml::_toString(iSeq, "seq%d"), sign*(iSeq+1));
      seqDet.setPlacement(seqInLayers[iLayer][iSeq]);

      DetElement tileDet(seqDet, dd4hep::xml::_toString(iSeq, "tile%d"), sign*(iSeq+1));
      tileDet.setPlacement(tilesPerLayer[iLayer]);
    }
  }  

}

static dd4hep::Ref_t createHCalEB(dd4hep::Detector& lcdd, xml_h xmlElement, dd4hep::SensitiveDetector sensDet) {

  // Get the Gaudi message service and message stream:
  ServiceHandle<IMessageSvc> msgSvc("MessageSvc", "ExtHCalConstruction");
  MsgStream lLog(&(*msgSvc), "ExtHCalConstruction");

  xml_det_t xmlDet = xmlElement;
  std::string detName = xmlDet.nameStr();

  // Make DetElement
  dd4hep::DetElement hCalEB(detName, xmlDet.id());

  // Make volume that envelopes the whole barrel; set material to air
  Dimension dimensions(xmlDet.dimensions());

  dd4hep::Tube envelope(dimensions.rmin2(), dimensions.rmax1(), (dimensions.z_offset() + dimensions.dz()));
  dd4hep::Tube negative(dimensions.rmin2(), dimensions.rmax1(), (dimensions.offset() - dimensions.width()));
  dd4hep::SubtractionSolid envelopeShape(envelope, negative);

  Volume envelopeVolume(detName + "_volume", envelopeShape, lcdd.air());
  envelopeVolume.setVisAttributes(lcdd, dimensions.visStr());

  lLog << MSG::DEBUG << "Placing detector on the positive side: (cm) " << (dimensions.offset() + dimensions.dz())
       << endmsg;
  buildEB(lLog, lcdd, sensDet, envelopeVolume, hCalEB, xmlElement, 1);
  lLog << MSG::DEBUG << "Placing detector on the negative side: (cm) " << -(dimensions.offset() + dimensions.dz())
       << endmsg;
  buildEB(lLog, lcdd, sensDet, envelopeVolume, hCalEB, xmlElement, -1);

  // Place envelope volume
  Volume motherVol = lcdd.pickMotherVolume(hCalEB);

  PlacedVolume placedHCal = motherVol.placeVolume(envelopeVolume);
  placedHCal.addPhysVolID("system", xmlDet.id());
  hCalEB.setPlacement(placedHCal);

  return hCalEB;
}
}  // namespace hcal

DECLARE_DETELEMENT(CaloExtBarrel, det::createHCalEB)
