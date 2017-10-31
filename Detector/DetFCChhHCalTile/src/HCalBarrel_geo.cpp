// DD4hep
#include "DD4hep/DetFactoryHelper.h"

// Gaudi
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

using DD4hep::Geometry::Volume;
using DD4hep::Geometry::DetElement;
using DD4hep::XML::Dimension;
using DD4hep::Geometry::PlacedVolume;

namespace det {

static DD4hep::Geometry::Ref_t createHCal(DD4hep::Geometry::LCDD& lcdd, xml_h xmlElement,
                                          DD4hep::Geometry::SensitiveDetector sensDet) {
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

  DD4hep::Geometry::Tube envelopeShape(dimensions.rmin(), dimensions.rmax(), dzDetector);
  Volume envelopeVolume("envelopeVolume", envelopeShape, lcdd.air());
  envelopeVolume.setVisAttributes(lcdd, dimensions.visStr());

  // Add structural support made of steel inside of HCal
  DetElement facePlate(hCal, "FacePlate", 0);
  DD4hep::Geometry::Tube facePlateShape(dimensions.rmin(), sensitiveBarrelRmin, (dzDetector - dZEndPlate - space));
  Volume facePlateVol("facePlate", facePlateShape, lcdd.material(xFacePlate.materialStr()));
  facePlateVol.setVisAttributes(lcdd, xFacePlate.visStr());
  PlacedVolume placedFacePlate = envelopeVolume.placeVolume(facePlateVol);
  facePlate.setPlacement(placedFacePlate);

  // Add structural support made of steel at both ends of HCal
  DD4hep::Geometry::Tube endPlateShape(dimensions.rmin(), (dimensions.rmax() - dSteelSupport), dZEndPlate / 2);
  Volume endPlateVol("endPlate", endPlateShape, lcdd.material(xEndPlate.materialStr()));
  endPlateVol.setVisAttributes(lcdd, xEndPlate.visStr());

  DetElement endPlatePos(hCal, "endPlatePos", 0);
  DD4hep::Geometry::Position posOffset(0, 0, dzDetector - (dZEndPlate / 2));
  PlacedVolume placedEndPlatePos = envelopeVolume.placeVolume(endPlateVol, posOffset);
  endPlatePos.setPlacement(placedEndPlatePos);

  DetElement endPlateNeg(hCal, "endPlateNeg", 1);
  DD4hep::Geometry::Position negOffset(0, 0, -dzDetector + (dZEndPlate / 2));
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

  // DetElement vectors for placement in loop at the end
  std::vector<DD4hep::Geometry::PlacedVolume> supports;
  supports.reserve(numSequencesPhi);
  std::vector<DD4hep::Geometry::PlacedVolume> modules;
  modules.reserve(numSequencesPhi);
  std::vector<DD4hep::Geometry::PlacedVolume> rows;
  rows.reserve(numSequencesZ);
  std::vector<DD4hep::Geometry::PlacedVolume> layers;
  layers.reserve(layerDepths.size());
  std::vector<std::vector<DD4hep::Geometry::PlacedVolume>> tilesInLayers;
  tilesInLayers.reserve(layerDepths.size());

  // First we construct one wedge:
  Volume wedgeVolume("wedgeVolume", DD4hep::Geometry::Trapezoid(dx1Module, dx2Module, dy0, dy0, dzModule),
                     lcdd.material("Air"));
  double layerR = 0.;

  // Placement of subWedges in Wedge
  for (unsigned int idxLayer = 0; idxLayer < layerDepths.size(); ++idxLayer) {
    auto layerName = DD4hep::XML::_toString(idxLayer, "layer%d");
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

    Volume layerVolume("layerVolume", DD4hep::Geometry::Trapezoid(dx1, dx2, dy0, dy0, dz0), lcdd.material("Air"));
    layerVolume.setVisAttributes(lcdd.invisible());
    unsigned int idxSubMod = 0;
    unsigned int idxActMod = 0;
    double modCompZOffset = -sequenceDimensions.dz() * 0.5;

    // this matches the order of sequences of standalone HCAL geo description
    if (sequenceIdx == 0) {
      sequenceIdx = 1;
    } else {
      sequenceIdx = 0;
    }
    // layer middle offset
    DD4hep::Geometry::Position modOffset(0, 0, rMiddle);
    layers.push_back(wedgeVolume.placeVolume(layerVolume, modOffset));
    layers.back().addPhysVolID("layer", idxLayer);

    std::vector<DD4hep::Geometry::PlacedVolume> tiles;
    // Filling of the subWedge with coponents (submodules)
    for (xml_coll_t xCompColl(sequences[sequenceIdx], _Unicode(module_component)); xCompColl;
         ++xCompColl, ++idxSubMod) {
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
        tiles.push_back(layerVolume.placeVolume(tileVol, offset));
        tiles.back().addPhysVolID("tile", idxActMod);
        idxActMod++;
      } else {
        tiles.push_back(layerVolume.placeVolume(modCompVol, offset));
      }
      modCompZOffset += xComp.thickness() + xComp.y_offset();
    }
    // Fill vector for DetElements
    tilesInLayers.push_back(tiles);
  }

  Volume moduleVolume("moduleVolume",
                      DD4hep::Geometry::Trapezoid(dx1Module, dx2Module, (dzDetector - dZEndPlate - space),
                                                  (dzDetector - dZEndPlate - space), dzModule),
                      lcdd.material("Air"));
  moduleVolume.setVisAttributes(lcdd.invisible());

  Volume steelSupportVolume("steelSupportVolume",
                            DD4hep::Geometry::Trapezoid(dx1Support, dx2Support, (dzDetector - dZEndPlate - space),
                                                        (dzDetector - dZEndPlate - space), dzSupport),
                            lcdd.material(xSteelSupport.materialStr()));

  for (unsigned int idxZRow = 0; idxZRow < numSequencesZ; ++idxZRow) {
    double zOffset = -dzDetector + dZEndPlate + space + (2 * idxZRow + 1) * (dzSequence * 0.5);
    lLog << MSG::DEBUG << "z offset of wedges = " << zOffset << std::endl;

    if ((-dzDetector + zOffset) >= dzDetector) {
      lLog << MSG::WARNING << " WARNING!!!! Module position outside of detector envelope" << std::endl;
    }
    DD4hep::Geometry::Position wedgeOffset(0, zOffset, 0);
    // Fill vector for DetElements
    rows.push_back(moduleVolume.placeVolume(wedgeVolume, wedgeOffset));
    rows.back().addPhysVolID("row", idxZRow);
  }

  for (unsigned int idxPhi = 0; idxPhi < numSequencesPhi; ++idxPhi) {
    double phi = 0.5 * dphi + idxPhi * dphi;  // 0.5*dphi for middle of module
    double yPosModule = (sensitiveBarrelRmin + dzModule) * cos(phi);
    double xPosModule = (sensitiveBarrelRmin + dzModule) * sin(phi);
    double yPosSupport = (sensitiveBarrelRmin + 2 * dzModule + dzSupport) * cos(phi);
    double xPosSupport = (sensitiveBarrelRmin + 2 * dzModule + dzSupport) * sin(phi);

    DD4hep::Geometry::Position moduleOffset(xPosModule, yPosModule, 0);
    DD4hep::Geometry::Position supportOffset(xPosSupport, yPosSupport, 0);

    DD4hep::Geometry::Transform3D trans(
        DD4hep::Geometry::RotationX(-0.5 * dd4hep::pi) * DD4hep::Geometry::RotationY(phi), moduleOffset);

    DD4hep::Geometry::Transform3D transS(
        DD4hep::Geometry::RotationX(-0.5 * dd4hep::pi) * DD4hep::Geometry::RotationY(phi), supportOffset);
    // Fill the vectors of DetElements
    modules.push_back(envelopeVolume.placeVolume(moduleVolume, trans));
    modules.back().addPhysVolID("module", idxPhi);
    supports.push_back(envelopeVolume.placeVolume(steelSupportVolume, transS));
    //    }
  }

  // Placement of DetElements
  lLog << MSG::DEBUG << "Modules in phi : " << modules.size() << std::endl;
  lLog << MSG::DEBUG << "Rows in z :      " << rows.size() << std::endl;
  lLog << MSG::DEBUG << "Layers in r :    " << layers.size() << std::endl;
  lLog << MSG::DEBUG << "Tiles in layers :" << tilesInLayers[1].size() << std::endl;

  for (uint iPhi = 0; iPhi < numSequencesPhi; iPhi++) {
    DetElement moduleDet(hCal, DD4hep::XML::_toString(iPhi, "module%d"), iPhi);
    moduleDet.setPlacement(modules[iPhi]);
    DetElement support(hCal, DD4hep::XML::_toString(iPhi, "support%d"), iPhi);
    support.setPlacement(supports[iPhi]);

    for (uint iZ = 0; iZ < numSequencesZ; iZ++) {
      DetElement wedgeDet(moduleDet, DD4hep::XML::_toString(iZ, "row%d"), iZ);
      wedgeDet.setPlacement(rows[iZ]);

      for (uint iLayer = 0; iLayer < numSequencesR; iLayer++) {
        DetElement layerDet(wedgeDet, DD4hep::XML::_toString(iLayer, "layer%d"), iLayer);
        layerDet.setPlacement(layers[iLayer]);

        for (uint iTile = 0; iTile < tilesInLayers[iLayer].size(); iTile++) {
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

DECLARE_DETELEMENT(CaloBarrel, det::createHCal)
