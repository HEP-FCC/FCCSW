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

static dd4hep::Ref_t createHCal(dd4hep::Detector& lcdd, xml_h xmlElement, dd4hep::SensitiveDetector sensDet) {
  // Get the Gaudi message service and message stream:
  ServiceHandle<IMessageSvc> msgSvc("MessageSvc", "HCalConstruction");
  MsgStream lLog(&(*msgSvc), "HCalConstruction");

  // set the sensitive detector type to the DD4hep calorimeter
  Dimension sensDetType = xmlElement.child(_Unicode(sensitive));
  sensDet.setType(sensDetType.typeStr());

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

  double sensitiveBarrelRmin = dimensions.rmin() + dRhoFacePlate + space;

  // Hard-coded assumption that we have two different sequences for the modules
  std::vector<xml_comp_t> sequences = {xmlElement.child(_Unicode(sequence_a)), xmlElement.child(_Unicode(sequence_b))};
  // NOTE: This assumes that both have the same dimensions!
  Dimension sequenceDimensions(sequences[1].dimensions());
  double dzSequence = sequenceDimensions.dz();
  lLog << MSG::DEBUG << "sequence thickness " << dzSequence << endmsg;

  // calculate the number of modules fitting in phi and Z
  unsigned int numSequencesPhi = sequenceDimensions.phiBins();
  double dphi = 2 * dd4hep::pi / static_cast<double>(numSequencesPhi);
  unsigned int numSequencesZ = static_cast<unsigned>((2 * dimensions.dz() - 2 * dZEndPlate - 2 * space) / dzSequence);

  // Hard-coded assumption that we have three different layer types for the modules
  std::vector<xml_comp_t> Layers = {xmlElement.child(_Unicode(layer_1)), xmlElement.child(_Unicode(layer_2)),
                                    xmlElement.child(_Unicode(layer_3))};
  unsigned int numSequencesR = 0;
  double moduleDepth = 0.;
  std::vector<double> layerDepths = std::vector<double>();
  for (std::vector<xml_comp_t>::iterator it = Layers.begin(); it != Layers.end(); ++it) {
    xml_comp_t layer = *it;
    Dimension layerDimension(layer.dimensions());
    numSequencesR += layerDimension.nModules();
    for (int nLayer = 0; nLayer < layerDimension.nModules(); nLayer++) {
      moduleDepth += layerDimension.dr();
      layerDepths.push_back(layerDimension.dr());
    }
  }
  lLog << MSG::DEBUG << "retrieved number of layers:  " << numSequencesR
       << " , which end up to a full module depth in rho of " << moduleDepth << endmsg;
  lLog << MSG::DEBUG << "retrieved number of layers:  " << layerDepths.size() << endmsg;

  lLog << MSG::INFO << "constructing: " << numSequencesPhi << " modules, within the angle "
       << (dphi * 180. / dd4hep::pi) << " per ring in phi, " << numSequencesZ << " rings in Z, " << numSequencesR
       << " layers in Rho, " << numSequencesR * numSequencesZ * numSequencesPhi << " tiles" << endmsg;

  // Calculate correction along z based on the module size (can only have natural number of modules)
  double dzDetector = (numSequencesZ * dzSequence) / 2 + dZEndPlate + space;
  lLog << MSG::INFO << "correction of dz (negative = size reduced):" << dzDetector - dimensions.dz() << endmsg;

  dd4hep::Tube envelopeShape(dimensions.rmin(), dimensions.rmax(), dzDetector);
  Volume envelopeVolume("envelopeVolume", envelopeShape, lcdd.air());
  envelopeVolume.setVisAttributes(lcdd, dimensions.visStr());

  // Add structural support made of steel inside of HCal
  DetElement facePlate(hCal, "FacePlate", 0);
  dd4hep::Tube facePlateShape(dimensions.rmin(), sensitiveBarrelRmin, (dzDetector - dZEndPlate - space));
  Volume facePlateVol("facePlate", facePlateShape, lcdd.material(xFacePlate.materialStr()));
  facePlateVol.setVisAttributes(lcdd, xFacePlate.visStr());
  PlacedVolume placedFacePlate = envelopeVolume.placeVolume(facePlateVol);
  facePlate.setPlacement(placedFacePlate);

  // Add structural support made of steel at both ends of HCal
  dd4hep::Tube endPlateShape(dimensions.rmin(), (dimensions.rmax() - dSteelSupport), dZEndPlate / 2);
  Volume endPlateVol("endPlate", endPlateShape, lcdd.material(xEndPlate.materialStr()));
  endPlateVol.setVisAttributes(lcdd, xEndPlate.visStr());

  DetElement endPlatePos(hCal, "endPlatePos", 0);
  dd4hep::Position posOffset(0, 0, dzDetector - (dZEndPlate / 2));
  PlacedVolume placedEndPlatePos = envelopeVolume.placeVolume(endPlateVol, posOffset);
  endPlatePos.setPlacement(placedEndPlatePos);

  DetElement endPlateNeg(hCal, "endPlateNeg", 1);
  dd4hep::Position negOffset(0, 0, -dzDetector + (dZEndPlate / 2));
  PlacedVolume placedEndPlateNeg = envelopeVolume.placeVolume(endPlateVol, negOffset);
  endPlateNeg.setPlacement(placedEndPlateNeg);

  // Calculation the dimensions of one whole module:
  double tn = tan(dphi / 2.);
  double spacing = sequenceDimensions.x();
  // the half depth of sequence (placed in z in HCAL coordinated) is y coordinate of trapezoid definition
  double dy0 = dzSequence * 0.5;

  double dx1Module = tn * sensitiveBarrelRmin - spacing;
  double dx2Module = tn * (sensitiveBarrelRmin + moduleDepth) - spacing;
  double dzModule = moduleDepth / 2;

  lLog << MSG::DEBUG << "half height of full module (trapezoid side): " << dzModule << endmsg;
  lLog << MSG::DEBUG << "half width  of full module (trapezoid side): " << dy0 << endmsg;
  lLog << MSG::DEBUG << "half width in phi (rmin) of full module (trapezoid side): " << dx1Module << endmsg;
  lLog << MSG::DEBUG << "half width in phi (rmax) of full module (trapezoid side): " << dx2Module << endmsg;

  double dx1Support = tn * (sensitiveBarrelRmin + moduleDepth) - spacing;
  double dx2Support = tn * (sensitiveBarrelRmin + moduleDepth + dSteelSupport) - spacing;
  double dzSupport = dSteelSupport / 2;
  double rminSupport = sensitiveBarrelRmin + moduleDepth - spacing;
  double rmaxSupport = sensitiveBarrelRmin + moduleDepth + dSteelSupport - spacing;

  // DetElement vectors for placement in loop at the end
  std::vector<dd4hep::PlacedVolume> layers;
  layers.reserve(layerDepths.size());
  std::vector<std::vector<dd4hep::PlacedVolume>> tilesInLayers;
  tilesInLayers.reserve(numSequencesZ*sequences.size());
  
  double layerR = 0.;

  // Placement of subWedges in Wedge
  for (unsigned int idxLayer = 0; idxLayer < layerDepths.size(); ++idxLayer) {
    auto layerName = dd4hep::xml::_toString(idxLayer, "layer%d");
    unsigned int sequenceIdx = idxLayer % 2;

    // get length of layer in rho
    double dz0 = layerDepths.at(idxLayer) * 0.5;

    // in Module rmin = 0  for first wedge, changed radius to the full radius starting at (0,0,0)
    double rminLayer = sensitiveBarrelRmin + layerR;
    double rmaxLayer = sensitiveBarrelRmin + layerR + layerDepths.at(idxLayer);
    double dx1 = tn * rminLayer - spacing;
    double dx2 = tn * rmaxLayer - spacing;
    double rMiddle = (rminLayer - sensitiveBarrelRmin) + dz0 - dzModule;

    layerR += layerDepths.at(idxLayer);

    dd4hep::Tube layerShape(rminLayer, rmaxLayer, (dzDetector - dZEndPlate - space));
    Volume layerVolume("layerVolume", layerShape, lcdd.material("Air"));
    
    layerVolume.setVisAttributes(lcdd.invisible());
    unsigned int idxSubMod = 0;
    unsigned int idxActMod = 0;
    double modCompZOffset = - (dzDetector - dZEndPlate - space) + sequenceDimensions.dz() * 0.5;
    
    // this matches the order of sequences of standalone HCAL geo description
    if (sequenceIdx == 0) {
      sequenceIdx = 1;
    } else {
      sequenceIdx = 0;
    }

    layers.push_back(envelopeVolume.placeVolume(layerVolume));
    layers.back().addPhysVolID("module", 0);
    layers.back().addPhysVolID("layer", idxLayer);
   
    std::vector<dd4hep::PlacedVolume> tiles;
    // Filling of the layer tube with tile tubes in full z
    for (uint numSeq=0; numSeq<numSequencesZ; numSeq++){
      for (xml_coll_t xCompColl(sequences[sequenceIdx], _Unicode(module_component)); xCompColl;
	   ++xCompColl, ++idxSubMod) {
	xml_comp_t xComp = xCompColl;
	double dyComp = xComp.thickness() * 0.5;
	dd4hep::Tube tileShape(rminLayer, rmaxLayer, dz0);
	Volume modCompVol("modCompVolume", tileShape,
			  lcdd.material(xComp.materialStr()));
	modCompVol.setVisAttributes(lcdd, xComp.visStr());
	dd4hep::Position offset(0, modCompZOffset + dyComp + xComp.y_offset() / 2, 0);
	
	if (xComp.isSensitive()) {
	  Volume tileVol("tileVolume", tileShape,
			 lcdd.material(xComp.materialStr()));
	  tileVol.setSensitiveDetector(sensDet);
	  tiles.push_back(layerVolume.placeVolume(tileVol, offset));
	  tiles.back().addPhysVolID("row", numSeq);
	  idxActMod++;
	} else {
	  tiles.push_back(layerVolume.placeVolume(modCompVol, offset));
	}
	modCompZOffset += xComp.thickness() + xComp.y_offset();
      }
    }
    // Fill vector for DetElements
    tilesInLayers.push_back(tiles);
  }

  dd4hep::Tube supportShape(rminSupport, rmaxSupport, (dzDetector - dZEndPlate - space));
  Volume steelSupportVolume("steelSupportVolume", supportShape,
			    lcdd.material(xSteelSupport.materialStr()));
  PlacedVolume placedSupport = envelopeVolume.placeVolume(steelSupportVolume);
  
  // Placement of DetElements
  lLog << MSG::DEBUG << "Layers in r :    " << layers.size() << endmsg;
  lLog << MSG::DEBUG << "Tiles in layers :" << tilesInLayers[1].size() << endmsg;
  
  DetElement support(hCal, "support", 0);
  support.setPlacement(placedSupport);
  
  for (uint iLayer = 0; iLayer < numSequencesR; iLayer++) {
    DetElement layerDet(hCal, dd4hep::xml::_toString(iLayer, "layer%d"), iLayer);
    layerDet.setPlacement(layers[iLayer]);
    
    for (uint iTile = 0; iTile < tilesInLayers[iLayer].size(); iTile++) {
      DetElement tileDet(layerDet, dd4hep::xml::_toString(iTile, "tile%d"), iTile);
      tileDet.setPlacement(tilesInLayers[iLayer][iTile]);
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

DECLARE_DETELEMENT(CaloBarrel, det::createHCal)
