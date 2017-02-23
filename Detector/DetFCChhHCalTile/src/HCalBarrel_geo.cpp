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

static DD4hep::Geometry::Ref_t createHCal (
  DD4hep::Geometry::LCDD& lcdd,
  xml_h xmlElement,
  DD4hep::Geometry::SensitiveDetector sensDet
  ) {
  // Get the Gaudi message service and message stream:
  ServiceHandle<IMessageSvc> msgSvc("MessageSvc", "HCalConstruction");
  MsgStream lLog(&(*msgSvc), "HCalConstruction");

  xml_det_t xmlDet = xmlElement;
  std::string detName = xmlDet.nameStr();
  //Make DetElement
  DetElement hCal(detName, xmlDet.id());

  // Make volume that envelopes the whole barrel; set material to air
  Dimension dimensions(xmlDet.dimensions());
  xml_comp_t xEndPlate = xmlElement.child("end_plate");
  double dZEndPlate = xEndPlate.thickness();
  xml_comp_t xFacePlate = xmlElement.child("face_plate");
  double dRhoFacePlate = xFacePlate.thickness();
  xml_comp_t xSpace = xmlElement.child("plate_space"); //to avoid overlaps                                                                                                                                                                 
  double space = xSpace.thickness();
  
  double sensitiveBarrelRmin = dimensions.rmin() + dRhoFacePlate + space;

  // Hard-coded assumption that we have two different sequences for the modules
  std::vector<xml_comp_t> sequences = {xmlElement.child("sequence_a"), xmlElement.child("sequence_b")};
  // NOTE: This assumes that both have the same dimensions!
  Dimension sequenceDimensions(sequences[0].dimensions());
  double dzSequence = sequenceDimensions.dz();
  lLog << MSG::DEBUG << "sequence thickness " << dzSequence << endmsg;
  
  // calculate the number of modules fitting in phi, Z and Rho
  unsigned int numSequencesPhi = sequenceDimensions.phiBins();
  double dphi = 2 * dd4hep::pi / static_cast<double>(numSequencesPhi);
  //  unsigned int numSequencesdZ = static_cast<unsigned>(dimensions.dz() / dzSequence);
  unsigned int numSequencesZ = static_cast<unsigned>((2*dimensions.dz()-2*dZEndPlate-2*space) / dzSequence);
  unsigned int numSequencesR = static_cast<unsigned>((cos(dphi / 2) * dimensions.rmax() - sensitiveBarrelRmin) / sequenceDimensions.dr()); // needed due to trapezoid shapes inside the available HCAL volume
  lLog << MSG::DEBUG << "constructing " << numSequencesPhi << " modules per ring in phi, "
                     << numSequencesZ << " rings in Z, "
                     << numSequencesR << " layers in Rho, "
                     << numSequencesR*numSequencesZ*numSequencesPhi << " tiles" << endmsg;

  // Calculate correction along z based on the module size (can only have natural number of modules)
  double dzDetector = (numSequencesZ * dzSequence)/2 + dZEndPlate + space;
  lLog << MSG::INFO << "correction of dz (negative = size reduced):" << dzDetector - dimensions.dz() << endmsg;

  DD4hep::Geometry::Tube envelopeShape(dimensions.rmin(), dimensions.rmax(), dzDetector);
  Volume envelopeVolume("envelopeVolume", envelopeShape, lcdd.air());
  envelopeVolume.setVisAttributes(lcdd.invisible());

  // set the sensitive detector type to the DD4hep calorimeter -- including Birks Law
  sensDet.setType("SimpleCalorimeterSDWithBirksLaw");

  // Add structural support made of steel inside of HCal
  DetElement facePlate(hCal, "FacePlate", 0);
  DD4hep::Geometry::Tube facePlateShape(dimensions.rmin(), sensitiveBarrelRmin, (dzDetector - dZEndPlate - space));
  Volume facePlateVol("facePlate", facePlateShape, lcdd.material(xFacePlate.materialStr()));
  facePlateVol.setVisAttributes(lcdd, xFacePlate.visStr());
  PlacedVolume placedFacePlate = envelopeVolume.placeVolume(facePlateVol);
  facePlate.setPlacement(placedFacePlate);


  // Add structural support made of steel at both ends of HCal
  DD4hep::Geometry::Tube endPlateShape(dimensions.rmin(), dimensions.rmax(), dZEndPlate/2);
  Volume endPlateVol("endPlate", endPlateShape, lcdd.material(xEndPlate.materialStr()));
  endPlateVol.setVisAttributes(lcdd, xEndPlate.visStr());

  DetElement endPlatePos("endPlate", 0);
  DD4hep::Geometry::Position posOffset(0, 0, dzDetector -  (dZEndPlate/2));
  PlacedVolume placedEndPlatePos = envelopeVolume.placeVolume(endPlateVol, posOffset);
  endPlatePos.setPlacement(placedEndPlatePos);

  DetElement endPlateNeg("endPlate", 1);
  DD4hep::Geometry::Position negOffset(0, 0, -dzDetector +  (dZEndPlate/2));
  PlacedVolume placedEndPlateNeg = envelopeVolume.placeVolume(endPlateVol, negOffset);
  endPlateNeg.setPlacement(placedEndPlateNeg);

  // Calculation the dimensions of one whole module:
  double tn = tan(dphi / 2.);
  double spacing = sequenceDimensions.x();
  //the half depth of sequence (placed in z in HCAL coordinated) is y coordinate of trapezoid definition
  double dy0 = dzSequence * 0.5;
  //the half minimum depth of the sequence defines the minimum depth of layers
  double dz0 = sequenceDimensions.dr() * 0.5;

  double dx1Module = tn * sensitiveBarrelRmin - spacing;
  double dx2Module = tn * cos(dphi / 2.) * dimensions.rmax() - spacing;         
  double dzModule = (numSequencesR*sequenceDimensions.dr())/2; //( cos(dphi / 2.) * dimensions.rmax() - sensitiveBarrelRmin ) * 0.5;

  lLog << MSG::DEBUG << "half height of full module (trapazoid side): " << dzModule << endmsg;
  //lLog << MSG::DEBUG << "half height of full module (diff way):       " << (numSequencesR*sequenceDimensions.dr())/2 << endmsg;
  lLog << MSG::DEBUG << "half width  of full module (trapazoid side): " << dy0 << endmsg;

  // First we construct one wedge:
  Volume wedgeVolume("wedge", DD4hep::Geometry::Trapezoid(dx1Module, dx2Module, dy0, dy0, dzModule), lcdd.material("Air"));
  // Placement of subWedges in Wedge 
  for (unsigned int idxLayer = 0; idxLayer < numSequencesR; ++idxLayer) {
    auto layerName = DD4hep::XML::_toString(idxLayer, "layer%d");
    unsigned int sequenceIdx = idxLayer % 2;
    DetElement layerDet(hCal, layerName, idxLayer);

    //in Module rmin = 0  for first wedge, changed radius to the full radius starting at (0,0,0)
    double rminLayer = sensitiveBarrelRmin + idxLayer * sequenceDimensions.dr();
    double rmaxLayer = sensitiveBarrelRmin + (idxLayer + 1) * sequenceDimensions.dr();
    double dx1 = tn * rminLayer - spacing;
    double dx2 = tn * rmaxLayer - spacing;
    double rMiddle = (rminLayer-sensitiveBarrelRmin) + 0.5 * sequenceDimensions.dr() - dzModule;
    lLog << MSG::DEBUG << "offset of layer inside the wedge: " << rMiddle << endmsg;
    
    Volume subWedgeVolume(layerName, DD4hep::Geometry::Trapezoid(
        dx1, dx2, dy0, dy0, dz0
      ), lcdd.material("Air")
    );
    subWedgeVolume.setVisAttributes(lcdd.invisible());
    unsigned int idxSubMod = 0;
    unsigned int idxActMod = 0;
    double modCompZOffset = - sequenceDimensions.dz() * 0.5;

    //Filling of the subWedge with coponents (submodules)
    for (xml_coll_t xCompColl(sequences[sequenceIdx], _U(module_component)); xCompColl; ++xCompColl, ++idxSubMod) {
      xml_comp_t xComp = xCompColl;
      auto compName = DD4hep::XML::_toString(idxLayer, "layer%d") + DD4hep::XML::_toString(idxSubMod, "_tile%d");
      std::string subModuleName =  xComp.materialStr() + DD4hep::XML::_toString(idxSubMod, "_tile%d");
      DetElement compDet(hCal, compName, idxSubMod);
      //added * 0.5 for Trapezoid 
      double dyComp = xComp.thickness() * 0.5;
      Volume modCompVol(subModuleName, DD4hep::Geometry::Trapezoid(
	  dx1, dx2, dyComp, dyComp, dz0
        ), lcdd.material(xComp.materialStr())
      );
      modCompVol.setVisAttributes(lcdd, xComp.visStr());
      DD4hep::Geometry::Position offset(0, modCompZOffset + dyComp + xComp.y_offset()/2, 0);
      lLog << MSG::DEBUG << "position of components in z : " << modCompZOffset + dyComp << endmsg;
      PlacedVolume placedModCompVol = subWedgeVolume.placeVolume(modCompVol, offset);

      if (xComp.isSensitive()) {
	//std::cout << xComp.materialStr() << std::endl;
	modCompVol.setSensitiveDetector(sensDet);
	placedModCompVol.addPhysVolID("tile", idxActMod);
	idxActMod++;
      }
      compDet.setPlacement(placedModCompVol);
      modCompZOffset += xComp.thickness() + xComp.y_offset();
    }
    DD4hep::Geometry::Position modOffset(0, 0, rMiddle);
    PlacedVolume placedModuleVol = wedgeVolume.placeVolume(subWedgeVolume, modOffset);
    placedModuleVol.addPhysVolID("layer", idxLayer);
    layerDet.setPlacement(placedModuleVol);
  }

  // Now we place the wedges along z within the module
  Volume moduleVolume("module", DD4hep::Geometry::Trapezoid(
							    dx1Module, dx2Module, (dzDetector - dZEndPlate - space), (dzDetector - dZEndPlate - space), dzModule
							   ), lcdd.material("Air")
		     );
  moduleVolume.setVisAttributes(lcdd.invisible());

  for (unsigned int idxZRow = 0; idxZRow < numSequencesZ; ++idxZRow) {
    auto wedgeName = DD4hep::XML::_toString(idxZRow, "row%d");
    DetElement wedgeDet(hCal, wedgeName, idxZRow);  
    double zOffset = -dzDetector + dZEndPlate + space + (2*idxZRow + 1) * (dzSequence*0.5);
    lLog << MSG::DEBUG << "z offset of wedges = " << zOffset << std::endl;
    
    if ( (-dzDetector + zOffset) >= dzDetector )
      lLog << MSG::DEBUG << " WARNING!!!! Module position outside" << std::endl;
    
    DD4hep::Geometry::Position wedgeOffset(0, zOffset, 0);
    PlacedVolume placedRowVolume = moduleVolume.placeVolume(wedgeVolume, wedgeOffset);
    placedRowVolume.addPhysVolID("row", idxZRow);
    wedgeDet.setPlacement(placedRowVolume);
  }


  // Finally we place all the modules around phi
  for (unsigned int idxPhi = 0; idxPhi < numSequencesPhi; ++idxPhi) {
    auto modName = DD4hep::XML::_toString(idxPhi, "mod%d");
    DetElement moduleDet(hCal, modName, idxPhi);
    // first rotation due to default rotation of trapezoid
    double phi = 0.5 * dphi + idxPhi * dphi; // 0.5*dphi for middle of module
    double yPosModule = (sensitiveBarrelRmin + dzModule) * cos(phi);
    double xPosModule = (sensitiveBarrelRmin + dzModule) * sin(phi);
    DD4hep::Geometry::Position moduleOffset(xPosModule, yPosModule, 0);
    DD4hep::Geometry::Transform3D trans(
 					DD4hep::Geometry::RotationX(-0.5*dd4hep::pi)*
 					DD4hep::Geometry::RotationY(phi),
 					moduleOffset
 					);
    PlacedVolume placedModuleVol = envelopeVolume.placeVolume(moduleVolume, trans);
    placedModuleVol.addPhysVolID("module", idxPhi);
    moduleDet.setPlacement(placedModuleVol);
  }

  //Place envelope (or barrel) volume
  Volume motherVol = lcdd.pickMotherVolume(hCal);
  PlacedVolume placedHCal = motherVol.placeVolume(envelopeVolume);
  placedHCal.addPhysVolID("system", hCal.id());
  hCal.setPlacement(placedHCal);
  return hCal;

}
} // namespace hcal

DECLARE_DETELEMENT(CaloBarrel, det::createHCal)
