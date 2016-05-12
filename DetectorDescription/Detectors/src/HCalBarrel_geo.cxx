// DD4hep includes
#include "DD4hep/DetFactoryHelper.h"

// FCCSW includes
// #include "DetExtensions/DetCylinderVolume.h"

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


  //JANA: read-in the parameters and resize the calorimeter
  //All parameters in xml, except the calorimeter size in z (dz), are full lengths!!
  Dimension dimensions(xmlDet.dimensions());
  //Faceplate and Endplate dimensions needed for that, reading it here:
  //Faceplate: in front of the actual calorimeter volume in R
  xml_comp_t xFacePlate = xmlElement.child("face_plate");
  double dRhoFacePlate = xFacePlate.thickness();
  //Endplates: at both sides of the calorimeter in Z
  xml_comp_t xEndPlate = xmlElement.child("end_plate");
  double dZEndPlate = xEndPlate.thickness();

  //JANA: re-define the HCAL volume: active part in R (starts behind the facePlate)
  const double hcal_rmin = dimensions.rmin() + dRhoFacePlate;
  const double hcal_rmax = dimensions.rmax();
  
  // Hard-coded assumption that we have two different sequences for the modules
  std::vector<xml_comp_t> sequences = {xmlElement.child("sequence_a"), xmlElement.child("sequence_b")};
  // NOTE: This assumes that both have the same dimensions! - always true for TileCal
  Dimension sequenceDimensions(sequences[0].dimensions());
  double dzModule = sequenceDimensions.thickness();

  // calculate the number of modules fitting in phi, Z and Rho
  unsigned int numModulesPhi = sequenceDimensions.phiBins();
  unsigned int numModulesZ = static_cast<unsigned>((dimensions.dz()*2-2*dZEndPlate) / dzModule);
  unsigned int numModulesR = static_cast<unsigned>((hcal_rmax - hcal_rmin) / sequenceDimensions.dr());
  std::cout << "constructing " << numModulesPhi << " modules per ring in phi, " << std::endl;
  std::cout << "             " << numModulesZ << " rings in Z, " << std::endl;
  std::cout << "             " << numModulesR << " rings (layers) in Rho" << std::endl;
  std::cout << "             => " << numModulesR*numModulesZ*numModulesPhi << " modules" << std::endl;

  // Calculate correction along z based on the module size (can only have natural number of modules)
  std::cout << "correction of dz:" << dimensions.dz()*2 - ( numModulesZ * dzModule + 2*dZEndPlate ) << std::endl;
  //JANA: re-define the HCAL volume including the endplates
  const double hcal_zhalf = (numModulesZ * dzModule + 2*dZEndPlate)/2;
  //active part of the detector in Z - without endplates
  const double hcal_zhalf_noEP = hcal_zhalf-dZEndPlate;

 // set the sensitive detector type to the DD4hep calorimeter
  sensDet.setType("Geant4Calorimeter");

  // Make volume that envelopes the whole barrel; set material to air
  //JANA: Tube: third parameter is half size!!!
  DD4hep::Geometry::Tube envelopeShape(dimensions.rmin(), dimensions.rmax(), dimensions.dz());
  Volume envelopeVolume(detName, envelopeShape, lcdd.air());
  // Invisibility seems to be broken in visualisation tags, have to hardcode that
  // envelopeVolume.setVisAttributes(lcdd, dimensions.visStr());
  envelopeVolume.setVisAttributes(lcdd.invisible());

  // Add structural support made of steel inside of HCal
  //JANA: Place facePlate inside the HCAL volume
  DetElement facePlate("facePlate", 0);
  DD4hep::Geometry::Tube facePlateShape(dimensions.rmin(), dimensions.rmin()+dRhoFacePlate, hcal_zhalf);
  Volume facePlateVol("facePlate", facePlateShape, lcdd.material(xFacePlate.materialStr()));
  facePlateVol.setVisAttributes(lcdd, xFacePlate.visStr());
  PlacedVolume placedFacePlate = envelopeVolume.placeVolume(facePlateVol);
  placedFacePlate.addPhysVolID("facePlate", facePlate.id());
  facePlate.setPlacement(placedFacePlate);

  //Add structural support made of steel at both ends of HCal
  //JANA: Tube: third parameter is half size!!!
  DD4hep::Geometry::Tube endPlateShape(hcal_rmin, hcal_rmax, dZEndPlate*0.5);
  Volume endPlateVol("endPlate", endPlateShape, lcdd.material(xEndPlate.materialStr()));
  endPlateVol.setVisAttributes(lcdd, xEndPlate.visStr());

  DetElement endPlatePos("endPlate", 0);
  //JANA: place endPlates inside the HCAL volume
  DD4hep::Geometry::Position posOffset(0, 0, hcal_zhalf-dZEndPlate*0.5);
  PlacedVolume placedEndPlatePos = envelopeVolume.placeVolume(endPlateVol, posOffset);
  placedEndPlatePos.addPhysVolID("endPlatePos", endPlatePos.id());
  endPlatePos.setPlacement(placedEndPlatePos);

  DetElement endPlateNeg("endPlate", 1);
  DD4hep::Geometry::Position negOffset(0, 0, -hcal_zhalf+dZEndPlate*0.5);
  PlacedVolume placedEndPlateNeg = envelopeVolume.placeVolume(endPlateVol, negOffset);
  placedEndPlateNeg.addPhysVolID("endPlateNeg", endPlateNeg.id());
  endPlateNeg.setPlacement(placedEndPlateNeg);
 

  // calculate the dimensions of one module:
  double dphi = 2 * dd4hep::pi / static_cast<double>(numModulesPhi);
  double tn = tan(dphi / 2.);
  double spacing = sequenceDimensions.x();
  double dy0 = sequenceDimensions.thickness()*0.5;
  double dz0 = sequenceDimensions.dr()*0.5;

  double drWedge = (hcal_rmax - hcal_rmin) * 0.5;

  double dxWedge1 = tn * hcal_rmin - spacing;
  double dxWedge2 = tn * hcal_rmax - spacing;

  //Trapezoid(dx1,dx2,dy1,dy2,dz)
  // dx1 Half-length along x at the surface positioned at -dz
  // dx2 Half-length along x at the surface positioned at +dz
  // dy1 Half-length along y at the surface positioned at -dz
  // dy2 Half-length along y at the surface positioned at +dz
  // dz  Half-length along z axis


  // First we construct one wedge with width of one module:
  Volume subWedgeVolume("subWedge", DD4hep::Geometry::Trapezoid(
        dxWedge1, dxWedge2, dzModule*0.5, dzModule*0.5, drWedge
      ), lcdd.material("Air")
  );
  //numModulesR = 1;
  for (unsigned int idxLayer = 0; idxLayer < numModulesR; ++idxLayer) {
    auto layerName = std::string("wedge") + DD4hep::XML::_toString(idxLayer, "layer%d");
    unsigned int sequenceIdx = idxLayer % 2;
    double rminLayer = idxLayer * sequenceDimensions.dr();
    double rmaxLayer = (idxLayer + 1) * sequenceDimensions.dr();
    double dx1 = tn * (rminLayer + hcal_rmin) - spacing;
    double dx2 = tn * (rmaxLayer + hcal_rmin) - spacing;
    // -drWedge to place it in the middle of the wedge-volume
    double rMiddle = rminLayer + 0.5 * sequenceDimensions.dr() - drWedge;
    Volume moduleVolume(layerName, DD4hep::Geometry::Trapezoid(
        dx1, dx2, dy0, dy0, dz0
    ), lcdd.material("Air")
    );
    //std::cout <<"wedgelayer" << idxLayer << " " << dx1 << " "<<dx2<<" "<<dy0<<" "<<dz0 <<std::endl;
    moduleVolume.setVisAttributes(lcdd.invisible());
    unsigned int idxSubMod = 0;
    // DetElement moduleDet(wedgeDet, layerName, idxLayer);
    double modCompOffset = -sequenceDimensions.thickness()*0.5;
    for (xml_coll_t xCompColl(sequences[sequenceIdx], _U(module_component)); xCompColl; ++xCompColl, ++idxSubMod) {
      xml_comp_t xComp = xCompColl;
      std::string subModuleName = layerName+DD4hep::XML::_toString(idxSubMod, "module_component%d");
      double modCompThickness = xComp.thickness();
      Volume modCompVol(subModuleName, DD4hep::Geometry::Trapezoid(
          dx1, dx2, modCompThickness*0.5, modCompThickness*0.5, dz0
      ), lcdd.material(xComp.materialStr())
      );
      // std::cout <<"wedgelayer" << idxLayer << "_component" << idxSubMod << " " << dx1 << " "<<dx2<<" "<<modCompThickness*0.5<<" "<<dz0 <<std::endl;
      if (xComp.isSensitive()) {
        modCompVol.setSensitiveDetector(sensDet);
      }
      modCompVol.setVisAttributes(lcdd, xComp.visStr());
      // modCompVol.setVisAttributes(lcdd.invisible());
      // DetElement modCompDet(wedgeDet, subModuleName, idxSubMod);
      //JANA: Offsets fixed
      DD4hep::Geometry::Position offset(0, modCompOffset + xComp.offset() + modCompThickness*0.5, 0);
      PlacedVolume placedModCompVol = moduleVolume.placeVolume(modCompVol, offset);
      placedModCompVol.addPhysVolID("sub_module", idxSubMod);
      // modCompDet.setPlacement(placedModCompVol);
      
      modCompOffset += modCompThickness + xComp.offset();
    }
    DD4hep::Geometry::Position modOffset(0, 0, rMiddle);
    PlacedVolume placedModuleVol = subWedgeVolume.placeVolume(moduleVolume, modOffset);
    placedModuleVol.addPhysVolID("layer", idxLayer);
    // moduleDet.setPlacement(placedModuleVol);
  }

  // Now we place the components along z within the wedge
  Volume wedgeVolume("wedge", DD4hep::Geometry::Trapezoid(
        dxWedge1, dxWedge2, hcal_zhalf_noEP, hcal_zhalf_noEP, drWedge
      ), lcdd.material("Air")
  );
  wedgeVolume.setVisAttributes(lcdd.invisible());

  //numModulesZ = 1;
  for (unsigned int idxZRow = 0; idxZRow < numModulesZ; ++idxZRow) {
    //JANA: dZEndPlate*2 removed
    double zOffset = -hcal_zhalf_noEP + (idxZRow + 0.5) * dzModule;
    auto wedgeRowName = DD4hep::XML::_toString(idxZRow, "row%d");
    DD4hep::Geometry::Position wedgeOffset(0, zOffset, 0);
    PlacedVolume placedRowVolume = wedgeVolume.placeVolume(subWedgeVolume, wedgeOffset);
    placedRowVolume.addPhysVolID("row", idxZRow);
    // wedgeDet.setPlacement(placedWedgeVol);
  }
  //numModulesPhi = 1;
  // Finally we place all the wedges around phi
  for (unsigned int idxPhi = 0; idxPhi < numModulesPhi; ++idxPhi) {
    auto modName = DD4hep::XML::_toString(idxPhi, "mod%d");
    // Volume and DetElement for one row in Z
    DetElement wedgeDet(hCal, modName, idxPhi);
    // moduleVolume.setVisAttributes(lcdd, sequences[sequenceIdx].visStr());
    // moduleVolume.setVisAttributes(lcdd.invisible());
    // calculate position and rotation of this wedge;
    // first rotation due to default rotation of trapezoid
    //double phi = 0.5 * dphi - idxPhi * dphi; // 0.5*dphi for middle of module
    double phi = (-(double)idxPhi -0.5)*dphi; // 0.5*dphi for middle of module
    double yPosModule = (hcal_rmin + drWedge) * cos(phi);
    double xPosModule = (hcal_rmin + drWedge) * sin(phi);
    DD4hep::Geometry::Position moduleOffset(xPosModule, yPosModule, 0);
    
    
    DD4hep::Geometry::Transform3D trans(
      DD4hep::Geometry::RotationX(-0.5*dd4hep::pi)*
      DD4hep::Geometry::RotationY(phi),
      moduleOffset
    );
    
    
    //    std::cout << "idxPhi " << idxPhi << " dphi " << dphi << " Phi " << phi << " yPos " << yPosModule << " xPos " << xPosModule << " Rotation: X " << -0.5*dd4hep::pi << " Y " << phi << std::endl;
    PlacedVolume placedWedgeVol = envelopeVolume.placeVolume(wedgeVolume, trans);
    //PlacedVolume placedWedgeVol = envelopeVolume.placeVolume(wedgeVolume);
    
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

