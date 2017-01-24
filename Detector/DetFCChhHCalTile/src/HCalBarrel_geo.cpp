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
  // Get status for the RecoGeometry what is status?
  // xml_comp_t xmlStatus = xmlDet.child(_U(status));
  // int status = xmlStatus.id();
  // add Extension to Detlement for the RecoGeometry
  // Let's skip this for now...
  // Det::DetCylinderVolume* detVolume = new Det::DetCylinderVolume(status);
  // hCal.addExtension<Det::IDetExtension>(detVolume);

  // Make volume that envelopes the whole barrel; set material to air
  Dimension dimensions(xmlDet.dimensions());
  xml_comp_t xEndPlate = xmlElement.child("end_plate");
  double dZEndPlate = xEndPlate.thickness();
  xml_comp_t xFacePlate = xmlElement.child("face_plate");
  double dRhoFacePlate = xFacePlate.thickness();
  double sensitiveBarrelRmin = dimensions.rmin() + dRhoFacePlate;
  // Hard-coded assumption that we have two different sequences for the modules
  std::vector<xml_comp_t> sequences = {xmlElement.child("sequence_a"), xmlElement.child("sequence_b")};
  // NOTE: This assumes that both have the same dimensions!
  Dimension moduleDimensions(sequences[0].dimensions());
  double dzModule = moduleDimensions.dz();
  // calculate the number of modules fitting in phi, Z and Rho
  unsigned int numModulesPhi = moduleDimensions.phiBins();
  unsigned int numModulesZ = static_cast<unsigned>(dimensions.dz() / dzModule);
  unsigned int numModulesR = static_cast<unsigned>((dimensions.rmax() - sensitiveBarrelRmin) / moduleDimensions.dr());
  lLog << MSG::DEBUG << "constructing " << numModulesPhi << " modules per ring in phi, "
                     << numModulesZ << " rings in Z, "
                     << numModulesR << " rings (layers) in Rho"
                     << numModulesR*numModulesZ*numModulesPhi << " modules" << endmsg;

  // Calculate correction along z based on the module size (can only have natural number of modules)
  double dzDetector = numModulesZ * dzModule + dZEndPlate;
  lLog << MSG::INFO << "correction of dz (negative = size reduced):" << dzDetector - dimensions.dz() << endmsg;

  DD4hep::Geometry::Tube envelopeShape(dimensions.rmin(), dimensions.rmax(), dzDetector);
  Volume envelopeVolume(detName, envelopeShape, lcdd.air());
  // Invisibility seems to be broken in visualisation tags, have to hardcode that
  // envelopeVolume.setVisAttributes(lcdd, dimensions.visStr());
  envelopeVolume.setVisAttributes(lcdd.invisible());

  // set the sensitive detector type to the DD4hep calorimeter
  sensDet.setType("SimpleCalorimeterSD");

  // Add structural support made of steel inside of HCal
  DetElement facePlate("facePlate", 0);
  DD4hep::Geometry::Tube facePlateShape(dimensions.rmin(), sensitiveBarrelRmin, dzDetector);
  Volume facePlateVol("facePlate", facePlateShape, lcdd.material(xFacePlate.materialStr()));
  facePlateVol.setVisAttributes(lcdd, xFacePlate.visStr());
  PlacedVolume placedFacePlate = envelopeVolume.placeVolume(facePlateVol);
  facePlate.setPlacement(placedFacePlate);


  // Add structural support made of steel at both ends of HCal

  DD4hep::Geometry::Tube endPlateShape(dimensions.rmin(), dimensions.rmax(), dZEndPlate);
  Volume endPlateVol("endPlate", endPlateShape, lcdd.material(xEndPlate.materialStr()));
  endPlateVol.setVisAttributes(lcdd, xEndPlate.visStr());

  DetElement endPlatePos("endPlate", 0);
  DD4hep::Geometry::Position posOffset(0, 0, dzDetector -  dZEndPlate);
  PlacedVolume placedEndPlatePos = envelopeVolume.placeVolume(endPlateVol, posOffset);
  endPlatePos.setPlacement(placedEndPlatePos);

  DetElement endPlateNeg("endPlate", 1);
  DD4hep::Geometry::Position negOffset(0, 0, -dzDetector +  dZEndPlate);
  PlacedVolume placedEndPlateNeg = envelopeVolume.placeVolume(endPlateVol, negOffset);
  endPlateNeg.setPlacement(placedEndPlateNeg);

  // calculate the dimensions of one module:
  double dphi = 2 * dd4hep::pi / static_cast<double>(numModulesPhi);
  double tn = tan(dphi / 2.);
  double spacing = moduleDimensions.x();
  double dy0 = moduleDimensions.dz();
  double dz0 = moduleDimensions.dr() / 2.;

  double drWedge = cos(dphi / 2.) * (dimensions.rmax() - sensitiveBarrelRmin) * 0.5;

  double dxWedge1 = tn * sensitiveBarrelRmin - spacing;
  double dxWedge2 = tn * cos(dphi / 2.) * dimensions.rmax() - spacing;

  // First we construct one wedge with width of one module:
  Volume subWedgeVolume("subWedge", DD4hep::Geometry::Trapezoid(
        dxWedge1, dxWedge2, dzModule, dzModule, drWedge
      ), lcdd.material("Air")
  );
  for (unsigned int idxLayer = 0; idxLayer < numModulesR; ++idxLayer) {
    auto layerName = std::string("wedge") + DD4hep::XML::_toString(idxLayer, "layer%d");
    unsigned int sequenceIdx = idxLayer % 2;
    double rminLayer = idxLayer * moduleDimensions.dr();
    double rmaxLayer = (idxLayer + 1) * cos(dphi / 2.) * moduleDimensions.dr();
    double dx1 = tn * (rminLayer + sensitiveBarrelRmin) - spacing;
    double dx2 = tn * cos(dphi / 2.) * (rmaxLayer + sensitiveBarrelRmin) - spacing;
    // -drWedge to place it in the middle of the wedge-volume
    double rMiddle = rminLayer + 0.5 * moduleDimensions.dr() - drWedge;
    Volume moduleVolume(layerName, DD4hep::Geometry::Trapezoid(
        dx1, dx2, dy0, dy0, dz0
      ), lcdd.material("Air")
    );
    moduleVolume.setVisAttributes(lcdd.invisible());
    unsigned int idxSubMod = 0;
    // DetElement moduleDet(wedgeDet, layerName, idxLayer);
    double modCompZOffset = -moduleDimensions.dz();
    for (xml_coll_t xCompColl(sequences[sequenceIdx], _U(module_component)); xCompColl; ++xCompColl, ++idxSubMod) {
      xml_comp_t xComp = xCompColl;
      std::string subModuleName = layerName+DD4hep::XML::_toString(idxSubMod, "module_component%d");
      double dyComp = xComp.thickness();
      Volume modCompVol(subModuleName, DD4hep::Geometry::Trapezoid(
          dx1, dx2, dyComp, dyComp, dz0
        ), lcdd.material(xComp.materialStr())
      );
      if (xComp.isSensitive()) {
        modCompVol.setSensitiveDetector(sensDet);
      }
      modCompVol.setVisAttributes(lcdd, xComp.visStr());
      // modCompVol.setVisAttributes(lcdd.invisible());
      // DetElement modCompDet(wedgeDet, subModuleName, idxSubMod);
      DD4hep::Geometry::Position offset(0, modCompZOffset + dyComp + xComp.y_offset()*2, 0);
      PlacedVolume placedModCompVol = moduleVolume.placeVolume(modCompVol, offset);
      placedModCompVol.addPhysVolID("sub_module", idxSubMod);
      // modCompDet.setPlacement(placedModCompVol);
      modCompZOffset += xComp.thickness()*2 + xComp.y_offset()*2;
    }
    DD4hep::Geometry::Position modOffset(0, 0, rMiddle);
    PlacedVolume placedModuleVol = subWedgeVolume.placeVolume(moduleVolume, modOffset);
    placedModuleVol.addPhysVolID("layer", idxLayer);
    // moduleDet.setPlacement(placedModuleVol);
  }

  // Now we place the components along z within the wedge
  Volume wedgeVolume("wedge", DD4hep::Geometry::Trapezoid(
        dxWedge1, dxWedge2, dzDetector, dzDetector, drWedge
      ), lcdd.material("Air")
  );
  wedgeVolume.setVisAttributes(lcdd.invisible());

  for (unsigned int idxZRow = 0; idxZRow < numModulesZ; ++idxZRow) {
    double zOffset = -dzDetector + dZEndPlate * 2 + (2*idxZRow + 1) * dzModule;
    auto wedgeRowName = DD4hep::XML::_toString(idxZRow, "row%d");
    DD4hep::Geometry::Position wedgeOffset(0, zOffset, 0);
    PlacedVolume placedRowVolume = wedgeVolume.placeVolume(subWedgeVolume, wedgeOffset);
    placedRowVolume.addPhysVolID("row", idxZRow);
    // wedgeDet.setPlacement(placedWedgeVol);
  }

  // Finally we place all the wedges around phi
  for (unsigned int idxPhi = 0; idxPhi < numModulesPhi; ++idxPhi) {
    auto modName = DD4hep::XML::_toString(idxPhi, "mod%d");
    // Volume and DetElement for one row in Z
    DetElement wedgeDet(hCal, modName, idxPhi);
    // moduleVolume.setVisAttributes(lcdd, sequences[sequenceIdx].visStr());
    // moduleVolume.setVisAttributes(lcdd.invisible());
    // calculate position and rotation of this wedge;
    // first rotation due to default rotation of trapezoid
    double phi = 0.5 * dphi - idxPhi * dphi; // 0.5*dphi for middle of module
    double yPosModule = (sensitiveBarrelRmin + drWedge) * cos(phi);
    double xPosModule = (sensitiveBarrelRmin + drWedge) * sin(phi);
    DD4hep::Geometry::Position moduleOffset(xPosModule, yPosModule, 0);
    DD4hep::Geometry::Transform3D trans(
      DD4hep::Geometry::RotationX(-0.5*dd4hep::pi)*
      DD4hep::Geometry::RotationY(phi),
      moduleOffset
    );
    PlacedVolume placedWedgeVol = envelopeVolume.placeVolume(wedgeVolume, trans);
    placedWedgeVol.addPhysVolID("wedge", idxPhi);
    wedgeDet.setPlacement(placedWedgeVol);
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

