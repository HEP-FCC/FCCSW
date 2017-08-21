#include "DD4hep/DetFactoryHelper.h"

// Gaudi
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

namespace det {
static DD4hep::Geometry::Ref_t createECalBarrelInclined(DD4hep::Geometry::LCDD& aLcdd,
                                                        DD4hep::XML::Handle_t aXmlElement,
                                                        DD4hep::Geometry::SensitiveDetector aSensDet) {
  ServiceHandle<IMessageSvc> msgSvc("MessageSvc", "ECalConstruction");
  MsgStream lLog(&(*msgSvc), "ECalConstruction");

  DD4hep::XML::DetElement xmlDetElem = aXmlElement;
  std::string nameDet = xmlDetElem.nameStr();
  DD4hep::XML::Dimension dim(xmlDetElem.dimensions());
  DD4hep::Geometry::DetElement caloDetElem(nameDet, xmlDetElem.id());

  // Create air envelope for the whole barrel
  DD4hep::Geometry::Volume envelopeVol(nameDet + "_vol", DD4hep::Geometry::Tube(dim.rmin(), dim.rmax(), dim.dz()),
                                       aLcdd.material("Air"));
  envelopeVol.setVisAttributes(aLcdd, dim.visStr());

  // Retrieve cryostat data
  DD4hep::XML::DetElement cryostat = aXmlElement.child(_Unicode(cryostat));
  DD4hep::XML::Dimension cryoDim(cryostat.dimensions());
  double cryoThicknessFront = cryoDim.rmin2() - cryoDim.rmin1();
  double cryoThicknessBack = cryoDim.rmax2() - cryoDim.rmax1();

  // Retrieve active and passive material data
  DD4hep::XML::DetElement calo = aXmlElement.child(_Unicode(calorimeter));
  DD4hep::XML::Dimension caloDim(calo.dimensions());
  DD4hep::XML::DetElement active = calo.child(_Unicode(active));
  std::string activeMaterial = active.materialStr();
  double activeThickness = active.thickness();

  DD4hep::XML::DetElement overlap = active.child(_Unicode(overlap));
  double activePassiveOverlap = overlap.offset();
  if (activePassiveOverlap < 0 || activePassiveOverlap > 0.5) {
    ServiceHandle<IIncidentSvc> incidentSvc("IncidentSvc", "ECalConstruction");
    lLog << MSG::ERROR << "Overlap between active and passive cannot be more than half of passive plane!" << endmsg;
    incidentSvc->fireIncident(Incident("ECalConstruction", "GeometryFailure"));
  }
  DD4hep::XML::DetElement layers = calo.child(_Unicode(layers));
  uint numLayers = 0;
  std::vector<double> layerHeight;
  double layersTotalHeight = 0;
  for (DD4hep::XML::Collection_t layer_coll(layers, _Unicode(layer)); layer_coll; ++layer_coll) {
    DD4hep::XML::Component layer = layer_coll;
    numLayers += layer.repeat();
    for (int iLay = 0; iLay < layer.repeat(); iLay++) {
      layerHeight.push_back(layer.thickness());
    }
    layersTotalHeight += layer.repeat() * layer.thickness();
  }
  lLog << MSG::DEBUG << "Number of layers: " << numLayers << " total thickness " << layersTotalHeight << endmsg;

  DD4hep::XML::DetElement readout = calo.child(_Unicode(readout));
  std::string readoutMaterial = readout.materialStr();
  double readoutThickness = readout.thickness();

  DD4hep::XML::DetElement passive = calo.child(_Unicode(passive));
  DD4hep::XML::DetElement passiveInner = passive.child(_Unicode(inner));
  DD4hep::XML::DetElement passiveOuter = passive.child(_Unicode(outer));
  DD4hep::XML::DetElement passiveGlue = passive.child(_Unicode(glue));
  std::string passiveInnerMaterial = passiveInner.materialStr();
  std::string passiveOuterMaterial = passiveOuter.materialStr();
  std::string passiveGlueMaterial = passiveGlue.materialStr();
  double passiveInnerThickness = passiveInner.thickness();
  double passiveOuterThickness = passiveOuter.thickness();
  double passiveGlueThickness = passiveGlue.thickness();
  double passiveThickness = passiveInnerThickness + passiveOuterThickness + passiveGlueThickness;
  double angle = passive.rotation().angle();

  // 1. Create the tubes for the outer shapes of the volumes
  double bathRmin, bathRmax;
  if (cryoThicknessFront > 0) {
    bathRmin = cryoDim.rmin2();
    bathRmax = cryoDim.rmax1();
  } else {
    bathRmin = caloDim.rmin();
    bathRmax = caloDim.rmax();
  }
  DD4hep::Geometry::Tube bathOuterShape(bathRmin, bathRmax, caloDim.dz());
  if (cryoThicknessFront > 0) {
    DD4hep::Geometry::Tube cryoOuterShape(cryoDim.rmin1(), cryoDim.rmax2(), cryoDim.dz());
    // Subtract volumes to get the actual shape of cryo
    DD4hep::Geometry::SubtractionSolid cryoShape(cryoOuterShape, bathOuterShape);

    // 1. Create cryostat
    lLog << MSG::INFO << "ECAL cryostat: rmin (cm) = " << cryoDim.rmin1() << " rmax (cm) = " << cryoDim.rmax2()
         << " thickness in front of ECal (cm) = " << cryoThicknessFront
         << " thickness behind ECal (cm) = " << cryoThicknessBack << endmsg;
    DD4hep::Geometry::Volume cryoVol(cryostat.nameStr(), cryoShape, aLcdd.material(cryostat.materialStr()));
    DD4hep::Geometry::PlacedVolume cryoPhysVol = envelopeVol.placeVolume(cryoVol);
    if (cryostat.isSensitive()) {
      cryoVol.setSensitiveDetector(aSensDet);
      cryoPhysVol.addPhysVolID("cryo", 1);
      lLog << MSG::INFO << "Cryostat volume set as sensitive" << endmsg;
    }
    DD4hep::Geometry::DetElement cryoDetElem(caloDetElem, "cryo", 0);
    cryoDetElem.setPlacement(cryoPhysVol);
  }
  // 2. Create bath that is inside the cryostat and surrounds the calorimeter
  //    Bath is filled with active material -> but not sensitive
  DD4hep::Geometry::Volume bathVol(activeMaterial + "_bath", bathOuterShape, aLcdd.material(activeMaterial));
  lLog << MSG::INFO << "ECAL bath: material = " << activeMaterial << " rmin (cm) =  " << bathRmin
       << " rmax (cm) = " << bathRmax << " thickness in front of ECal (cm) = " << caloDim.rmin() - cryoDim.rmin2()
       << " thickness behind ECal (cm) = " << cryoDim.rmax1() - caloDim.rmax() << endmsg;

  // 3. Create the calorimeter by placing the passive material, trapezoid active layers, readout and again trapezoid
  // active layers in the bath.
  // sensitive detector for the layers
  DD4hep::Geometry::SensitiveDetector sd = aSensDet;
  DD4hep::XML::Dimension sdType = xmlDetElem.child(_U(sensitive));
  sd.setType(sdType.typeStr());
  lLog << MSG::INFO << "ECAL calorimeter volume rmin (cm) =  " << caloDim.rmin() << " rmax (cm) = " << caloDim.rmax()
       << endmsg;

  // 3.a. Create the passive planes, readout in between of 2 passive planes and the remaining space fill with active
  // material
  //////////////////////////////
  // PASSIVE PLANES
  //////////////////////////////
  lLog << MSG::INFO << "passive inner material = " << passiveInnerMaterial
       << " and outer material = " << passiveOuterMaterial
       << " thickness of inner part (cm) =  " << passiveInnerThickness
       << " thickness of outer part (cm) =  " << passiveOuterThickness
       << " thickness of total (cm) =  " << passiveThickness << " rotation angle = " << angle << endmsg;
  uint numPlanes =
      M_PI / asin((passiveThickness + activeThickness + readoutThickness) / (2. * caloDim.rmin() * cos(angle)));
  double dPhi = 2. * M_PI / numPlanes;
  lLog << MSG::INFO << "number of passive plates = " << numPlanes << " azim. angle difference =  " << dPhi << endmsg;
  lLog << MSG::INFO << " distance at inner radius (cm) = " << 2. * M_PI * caloDim.rmin() / numPlanes
       << " distance at outer radius (cm) = " << 2. * M_PI * caloDim.rmax() / numPlanes << endmsg;
  // Readout is in the middle between two passive planes
  double offsetPassivePhi = caloDim.offset() + dPhi / 2.;
  double offsetReadoutPhi = caloDim.offset() + 0;
  lLog << MSG::INFO << "readout material = " << readoutMaterial
       << " thickness of readout planes (cm) =  " << readoutThickness << " number of readout layers = " << numLayers
       << endmsg;
  double Rmin = caloDim.rmin();
  double Rmax = caloDim.rmax();
  double dR = Rmax - Rmin;
  double planeLength = -Rmin * cos(angle) + sqrt(pow(Rmax, 2) - pow(Rmin * sin(angle), 2));
  lLog << MSG::INFO << "thickness of calorimeter (cm) = " << dR
       << " length of passive or readout planes (cm) =  " << planeLength << endmsg;
  DD4hep::Geometry::Box passiveShape(passiveThickness / 2., caloDim.dz(), planeLength / 2.);
  DD4hep::Geometry::Box passiveInnerShape(passiveInnerThickness / 2., caloDim.dz(), planeLength / 2.);
  DD4hep::Geometry::Box passiveOuterShape(passiveOuterThickness / 4., caloDim.dz(), planeLength / 2.);
  DD4hep::Geometry::Box passiveGlueShape(passiveGlueThickness / 4., caloDim.dz(), planeLength / 2.);
  // passive volume consists of inner part and two outer, joind by glue
  DD4hep::Geometry::Volume passiveVol("passive", passiveShape, aLcdd.material("Air"));
  DD4hep::Geometry::Volume passiveInnerVol(passiveInnerMaterial + "_passive", passiveInnerShape,
                                           aLcdd.material(passiveInnerMaterial));
  DD4hep::Geometry::Volume passiveOuterVol(passiveOuterMaterial + "_passive", passiveOuterShape,
                                           aLcdd.material(passiveOuterMaterial));
  DD4hep::Geometry::Volume passiveGlueVol(passiveGlueMaterial + "_passive", passiveGlueShape,
                                          aLcdd.material(passiveGlueMaterial));

  // rescale layer thicknesses
  double scaleLayerThickness = planeLength / layersTotalHeight;
  layersTotalHeight = 0;
  for (uint iLay = 0; iLay < numLayers; iLay++) {
    layerHeight[iLay] *= scaleLayerThickness;
    layersTotalHeight += layerHeight[iLay];
    lLog << MSG::DEBUG << "Thickness of layer " << iLay << " : " << layerHeight[iLay] << endmsg;
  }
  double layerFirstOffset = -planeLength / 2. + layerHeight[0] / 2.;
  if (passiveInner.isSensitive()) {
    lLog << MSG::DEBUG << "Passive inner volume set as sensitive" << endmsg;
    double layerOffset = layerFirstOffset;
    for (uint iLayer = 0; iLayer < numLayers; iLayer++) {
      DD4hep::Geometry::Box layerPassiveInnerShape(passiveInnerThickness / 2., caloDim.dz(), layerHeight[iLayer] / 2.);
      DD4hep::Geometry::Volume layerPassiveInnerVol(passiveInnerMaterial, layerPassiveInnerShape,
                                                    aLcdd.material(passiveInnerMaterial));
      layerPassiveInnerVol.setSensitiveDetector(aSensDet);
      DD4hep::Geometry::PlacedVolume layerPassiveInnerPhysVol =
          passiveInnerVol.placeVolume(layerPassiveInnerVol, DD4hep::Geometry::Position(0, 0, layerOffset));
      layerPassiveInnerPhysVol.addPhysVolID("layer", iLayer);
      DD4hep::Geometry::DetElement layerPassiveInnerDetElem("layer", iLayer);
      layerPassiveInnerDetElem.setPlacement(layerPassiveInnerPhysVol);
      if (iLayer != numLayers - 1) {
        layerOffset += layerHeight[iLayer] / 2. + layerHeight[iLayer + 1] / 2.;
      }
    }
  }
  if (passiveOuter.isSensitive()) {
    lLog << MSG::DEBUG << "Passive outer volume set as sensitive" << endmsg;
    double layerOffset = layerFirstOffset;
    for (uint iLayer = 0; iLayer < numLayers; iLayer++) {
      DD4hep::Geometry::Box layerPassiveOuterShape(passiveOuterThickness / 4., caloDim.dz(), layerHeight[iLayer] / 2.);
      DD4hep::Geometry::Volume layerPassiveOuterVol(passiveOuterMaterial, layerPassiveOuterShape,
                                                    aLcdd.material(passiveOuterMaterial));
      layerPassiveOuterVol.setSensitiveDetector(aSensDet);
      DD4hep::Geometry::PlacedVolume layerPassiveOuterPhysVol =
          passiveOuterVol.placeVolume(layerPassiveOuterVol, DD4hep::Geometry::Position(0, 0, layerOffset));
      layerPassiveOuterPhysVol.addPhysVolID("layer", iLayer);
      DD4hep::Geometry::DetElement layerPassiveOuterDetElem("layer", iLayer);
      layerPassiveOuterDetElem.setPlacement(layerPassiveOuterPhysVol);
      if (iLayer != numLayers - 1) {
        layerOffset += layerHeight[iLayer] / 2. + layerHeight[iLayer + 1] / 2.;
      }
    }
  }
  if (passiveGlue.isSensitive()) {
    lLog << MSG::DEBUG << "Passive glue volume set as sensitive" << endmsg;
    double layerOffset = layerFirstOffset;
    for (uint iLayer = 0; iLayer < numLayers; iLayer++) {
      DD4hep::Geometry::Box layerPassiveGlueShape(passiveGlueThickness / 4., caloDim.dz(), layerHeight[iLayer] / 2.);
      DD4hep::Geometry::Volume layerPassiveGlueVol(passiveGlueMaterial, layerPassiveGlueShape,
                                                   aLcdd.material(passiveGlueMaterial));
      layerPassiveGlueVol.setSensitiveDetector(aSensDet);
      DD4hep::Geometry::PlacedVolume layerPassiveGluePhysVol =
          passiveGlueVol.placeVolume(layerPassiveGlueVol, DD4hep::Geometry::Position(0, 0, layerOffset));
      layerPassiveGluePhysVol.addPhysVolID("layer", iLayer);
      DD4hep::Geometry::DetElement layerPassiveGlueDetElem("layer", iLayer);
      layerPassiveGlueDetElem.setPlacement(layerPassiveGluePhysVol);
      if (iLayer != numLayers - 1) {
        layerOffset += layerHeight[iLayer] / 2. + layerHeight[iLayer + 1] / 2.;
      }
    }
  }

  DD4hep::Geometry::PlacedVolume passiveInnerPhysVol =
      passiveVol.placeVolume(passiveInnerVol, DD4hep::Geometry::Position(0, 0, 0));
  DD4hep::Geometry::PlacedVolume passiveOuterPhysVolBelow = passiveVol.placeVolume(
      passiveOuterVol,
      DD4hep::Geometry::Position(passiveInnerThickness / 2. + passiveGlueThickness / 2. + passiveOuterThickness / 4., 0,
                                 0));
  DD4hep::Geometry::PlacedVolume passiveOuterPhysVolAbove = passiveVol.placeVolume(
      passiveOuterVol,
      DD4hep::Geometry::Position(-passiveInnerThickness / 2. - passiveGlueThickness / 2. - passiveOuterThickness / 4.,
                                 0, 0));
  DD4hep::Geometry::PlacedVolume passiveGluePhysVolBelow = passiveVol.placeVolume(
      passiveGlueVol, DD4hep::Geometry::Position(-passiveInnerThickness / 2. - passiveGlueThickness / 4., 0, 0));
  DD4hep::Geometry::PlacedVolume passiveGluePhysVolAbove = passiveVol.placeVolume(
      passiveGlueVol, DD4hep::Geometry::Position(passiveInnerThickness / 2. + passiveGlueThickness / 4., 0, 0));
  passiveInnerPhysVol.addPhysVolID("subtype", 0);
  passiveOuterPhysVolBelow.addPhysVolID("subtype", 1);
  passiveOuterPhysVolAbove.addPhysVolID("subtype", 2);
  passiveGluePhysVolBelow.addPhysVolID("subtype", 3);
  passiveGluePhysVolAbove.addPhysVolID("subtype", 4);

  //////////////////////////////
  // READOUT PLANES
  //////////////////////////////
  DD4hep::Geometry::Box readoutShape(readoutThickness / 2., caloDim.dz(), planeLength / 2.);
  DD4hep::Geometry::Volume readoutVol(readoutMaterial, readoutShape, aLcdd.material(readoutMaterial));
  if (readout.isSensitive()) {
    lLog << MSG::INFO << "Readout volume set as sensitive" << endmsg;
    double layerOffset = layerFirstOffset;
    for (uint iLayer = 0; iLayer < numLayers; iLayer++) {
      DD4hep::Geometry::Box layerReadoutShape(readoutThickness / 2., caloDim.dz(), layerHeight[iLayer] / 2.);
      DD4hep::Geometry::Volume layerReadoutVol(readoutMaterial, layerReadoutShape, aLcdd.material(readoutMaterial));
      layerReadoutVol.setSensitiveDetector(aSensDet);
      DD4hep::Geometry::PlacedVolume layerReadoutPhysVol =
          readoutVol.placeVolume(layerReadoutVol, DD4hep::Geometry::Position(0, 0, layerOffset));
      layerReadoutPhysVol.addPhysVolID("layer", iLayer);
      DD4hep::Geometry::DetElement layerReadoutDetElem("layer", iLayer);
      layerReadoutDetElem.setPlacement(layerReadoutPhysVol);
      if (iLayer != numLayers - 1) {
        layerOffset += layerHeight[iLayer] / 2. + layerHeight[iLayer + 1] / 2.;
      }
    }
  }

  //////////////////////////////
  // ACTIVE
  //////////////////////////////
  // thickness of active layers at inner radius and outer ( = distance between passive plane and readout plane)
  // at inner radius: distance projected at plane perpendicular to readout plane
  double activeInThickness = Rmin * sin(dPhi / 2.) * cos(angle);
  activeInThickness -= passiveThickness * (0.5 - activePassiveOverlap);
  // at outer radius: distance projected at plane perpendicular to readout plane
  double activeOutThickness = (Rmin + planeLength) * sin(dPhi / 2.) * cos(angle);
  // make correction for outer readius caused by inclination angle
  // first calculate intersection of readout plane and plane parallel to shifted passive plane
  double xIntersect = (Rmin * (tan(angle) - cos(dPhi / 2.) * tan(angle + dPhi / 2.)) - planeLength * sin(dPhi / 2.)) /
      (tan(angle) - tan(angle + dPhi / 2.));
  double yIntersect = tan(angle) * xIntersect + Rmin * (sin(dPhi / 2.) - tan(angle)) + planeLength * sin(dPhi / 2.);
  // distance from inner radius to intersection
  double correction =
      planeLength - sqrt(pow(xIntersect - Rmin * cos(dPhi / 2), 2) + pow(yIntersect - Rmin * sin(dPhi / 2), 2));
  // correction to the active thickness
  activeOutThickness += 2. * correction * sin(dPhi / 4.);
  activeOutThickness -= passiveThickness * (0.5 - activePassiveOverlap);
  // print the active layer dimensions
  double activeInThicknessAfterSubtraction =
      2. * activeInThickness - readoutThickness - 2. * activePassiveOverlap * passiveThickness;
  double activeOutThicknessAfterSubtraction =
      2. * activeOutThickness - readoutThickness - 2. * activePassiveOverlap * passiveThickness;
  lLog << MSG::INFO << "active material = " << activeMaterial
       << " active layers thickness at inner radius (cm) = " << activeInThicknessAfterSubtraction
       << " thickness at outer radious (cm) = " << activeOutThicknessAfterSubtraction << " making "
       << (activeOutThicknessAfterSubtraction - activeInThicknessAfterSubtraction) * 100 /
          activeInThicknessAfterSubtraction
       << " % increase." << endmsg;
  lLog << MSG::INFO
       << "active passive initial overlap (before subtraction) (cm) = " << passiveThickness * activePassiveOverlap
       << " = " << activePassiveOverlap * 100 << " %" << endmsg;

  // creating shape for rows of layers (active material between two passive planes, with readout in the middle)
  // first define area between two passive planes, area can reach up to the symmetry axis of passive plane
  DD4hep::Geometry::Trapezoid activeOuterShape(activeInThickness, activeOutThickness, caloDim.dz(), caloDim.dz(),
                                               planeLength / 2.);
  // subtract readout shape from the middle
  DD4hep::Geometry::SubtractionSolid activeShapeNoReadout(activeOuterShape, readoutShape);

  // make calculation for active plane that is inclined with 0 deg (= offset + angle)
  double Cx = Rmin * cos(-angle) + planeLength / 2.;
  double Cy = Rmin * sin(-angle);
  double Ax = Rmin * cos(-angle + dPhi / 2.) + planeLength / 2. * cos(dPhi / 2.);
  double Ay = Rmin * sin(-angle + dPhi / 2.) + planeLength / 2. * sin(dPhi / 2.);
  double CAx = fabs(Ax - Cx);
  double CAy = fabs(Ay - Cy);
  double zprim, xprim;
  zprim = CAx;
  xprim = CAy;

  double Bx = Rmin * cos(-angle - dPhi / 2.) + planeLength / 2. * cos(-dPhi / 2.);
  double By = Rmin * sin(-angle - dPhi / 2.) + planeLength / 2. * sin(-dPhi / 2.);
  double CBx = fabs(Bx - Cx);
  double CBy = fabs(By - Cy);
  double zprimB, xprimB;
  zprimB = CBx;
  xprimB = CBy;

  // subtract passive volume above
  DD4hep::Geometry::SubtractionSolid activeShapeNoPassiveAbove(
      activeShapeNoReadout, passiveShape,
      DD4hep::Geometry::Transform3D(DD4hep::Geometry::RotationY(-dPhi / 2.),
                                    DD4hep::Geometry::Position(-fabs(xprim), 0, fabs(zprim))));
  // subtract passive volume below
  DD4hep::Geometry::SubtractionSolid activeShape(
      activeShapeNoPassiveAbove, passiveShape,
      DD4hep::Geometry::Transform3D(DD4hep::Geometry::RotationY(dPhi / 2.),
                                    DD4hep::Geometry::Position(fabs(xprimB), 0, -fabs(zprimB))));
  DD4hep::Geometry::Volume activeVol("active", activeShape, aLcdd.material("Air"));

  std::vector<DD4hep::Geometry::PlacedVolume> layerPhysVols;
  // place layers within active volume
  std::vector<double> layerInThickness;
  std::vector<double> layerOutThickness;
  double layerIncreasePerUnitThickness = (activeOutThickness - activeInThickness) / layersTotalHeight;
  for (uint iLay = 0; iLay < numLayers; iLay++) {
    if (iLay == 0) {
      layerInThickness.push_back(activeInThickness);
    } else {
      layerInThickness.push_back(layerOutThickness[iLay - 1]);
    }
    layerOutThickness.push_back(layerInThickness[iLay] + layerIncreasePerUnitThickness * layerHeight[iLay]);
  }
  double layerOffset = layerFirstOffset;
  for (uint iLayer = 0; iLayer < numLayers; iLayer++) {
    DD4hep::Geometry::Trapezoid layerOuterShape(layerInThickness[iLayer], layerOutThickness[iLayer], caloDim.dz(),
                                                caloDim.dz(), layerHeight[iLayer] / 2.);
    DD4hep::Geometry::SubtractionSolid layerShapeNoReadout(layerOuterShape, readoutShape);
    DD4hep::Geometry::SubtractionSolid layerShapeNoPassiveAbove(
        layerShapeNoReadout, passiveShape,
        DD4hep::Geometry::Transform3D(DD4hep::Geometry::RotationY(-dPhi / 2.),
                                      DD4hep::Geometry::Position(-fabs(xprim), 0, fabs(zprim) - layerOffset)));
    // subtract passive volume below
    DD4hep::Geometry::SubtractionSolid layerShape(
        layerShapeNoPassiveAbove, passiveShape,
        DD4hep::Geometry::Transform3D(DD4hep::Geometry::RotationY(dPhi / 2.),
                                      DD4hep::Geometry::Position(fabs(xprimB), 0, -fabs(zprimB) - layerOffset)));
    DD4hep::Geometry::Volume layerVol("layer", layerShape, aLcdd.material(activeMaterial));
    layerVol.setSensitiveDetector(aSensDet);
    layerPhysVols.push_back(activeVol.placeVolume(layerVol, DD4hep::Geometry::Position(0, 0, layerOffset)));
    layerPhysVols.back().addPhysVolID("layer", iLayer);
    if (iLayer != numLayers - 1) {
      layerOffset += layerHeight[iLayer] / 2. + layerHeight[iLayer + 1] / 2.;
    }
  }

  DD4hep::Geometry::DetElement bathDetElem(caloDetElem, "bath", 1);
  std::vector<DD4hep::Geometry::PlacedVolume> activePhysVols;
  // Next place elements: passive planes, readout planes and rows of layers
  for (uint iPlane = 0; iPlane < numPlanes; iPlane++) {
    // first calculate positions of passive and readout planes
    // PASSIVE
    // calculate centre position of the plane without plane rotation
    double phi = offsetPassivePhi + iPlane * dPhi;
    double xRadial = (Rmin + planeLength / 2.) * cos(phi);
    double yRadial = (Rmin + planeLength / 2.) * sin(phi);
    // calculate position of the beginning of plane
    double xRmin = Rmin * cos(phi);
    double yRmin = Rmin * sin(phi);
    // rotate centre by angle wrt beginning of plane
    double xRotated = xRmin + (xRadial - xRmin) * cos(angle) - (yRadial - yRmin) * sin(angle);
    double yRotated = yRmin + (xRadial - xRmin) * sin(angle) + (yRadial - yRmin) * cos(angle);
    DD4hep::Geometry::Transform3D transform(DD4hep::Geometry::RotationX(-M_PI / 2.)  // to get in XY plane
                                                *
                                                DD4hep::Geometry::RotationY(M_PI / 2.  // to get pointed towards centre
                                                                            -
                                                                            phi - angle),
                                            DD4hep::Geometry::Position(xRotated, yRotated, 0));
    DD4hep::Geometry::PlacedVolume passivePhysVol = bathVol.placeVolume(passiveVol, transform);
    passivePhysVol.addPhysVolID("module", iPlane);
    passivePhysVol.addPhysVolID("type", 1);  // 0 = active, 1 = passive, 2 = readout
    DD4hep::Geometry::DetElement passiveDetElem(bathDetElem, "passive" + std::to_string(iPlane), iPlane);
    passiveDetElem.setPlacement(passivePhysVol);

    // READOUT
    // calculate centre position of the plane without plane rotation
    double phiRead = offsetReadoutPhi + iPlane * dPhi;
    double xRadialRead = (Rmin + planeLength / 2.) * cos(phiRead);
    double yRadialRead = (Rmin + planeLength / 2.) * sin(phiRead);
    // calculate position of the beginning of plane
    double xRminRead = Rmin * cos(phiRead);
    double yRminRead = Rmin * sin(phiRead);
    // rotate centre by angle wrt beginning of plane
    double xRotatedRead = xRminRead + (xRadialRead - xRminRead) * cos(angle) - (yRadialRead - yRminRead) * sin(angle);
    double yRotatedRead = yRminRead + (xRadialRead - xRminRead) * sin(angle) + (yRadialRead - yRminRead) * cos(angle);
    DD4hep::Geometry::Transform3D transformRead(
        DD4hep::Geometry::RotationX(-M_PI / 2.)  // to get in XY plane
            *
            DD4hep::Geometry::RotationY(M_PI / 2.  // to get pointed towards centre
                                        -
                                        phiRead - angle),
        DD4hep::Geometry::Position(xRotatedRead, yRotatedRead, 0));
    DD4hep::Geometry::PlacedVolume readoutPhysVol = bathVol.placeVolume(readoutVol, transformRead);
    readoutPhysVol.addPhysVolID("module", iPlane);
    readoutPhysVol.addPhysVolID("type", 2);  // 0 = active, 1 = passive, 2 = readout
    DD4hep::Geometry::DetElement readoutDetElem(bathDetElem, "readout" + std::to_string(iPlane), iPlane);
    readoutDetElem.setPlacement(readoutPhysVol);

    // ACTIVE
    DD4hep::Geometry::Rotation3D rotationActive(DD4hep::Geometry::RotationX(-M_PI / 2) *
                                                DD4hep::Geometry::RotationY(M_PI / 2 - phiRead - angle));
    activePhysVols.push_back(bathVol.placeVolume(
        activeVol,
        DD4hep::Geometry::Transform3D(rotationActive, DD4hep::Geometry::Position(xRotatedRead, yRotatedRead, 0))));
    activePhysVols.back().addPhysVolID("module", iPlane);
    activePhysVols.back().addPhysVolID("type", 0);  // 0 = active, 1 = passive, 2 = readout
  }
  DD4hep::Geometry::PlacedVolume bathPhysVol = envelopeVol.placeVolume(bathVol);
  if (cryostat.isSensitive()) {
    bathVol.setSensitiveDetector(aSensDet);
    bathPhysVol.addPhysVolID("cryo", 0);
    lLog << MSG::INFO << "Bath volume set as sensitive" << endmsg;
  }
  bathDetElem.setPlacement(bathPhysVol);

  for (uint iPlane = 0; iPlane < numPlanes; iPlane++) {
    DD4hep::Geometry::DetElement activeDetElem(bathDetElem, "active" + std::to_string(iPlane), iPlane);
    activeDetElem.setPlacement(activePhysVols[iPlane]);
    for (uint iLayer = 0; iLayer < numLayers; iLayer++) {
      DD4hep::Geometry::DetElement layerDetElem(activeDetElem, "layer" + std::to_string(iLayer), iLayer);
      layerDetElem.setPlacement(layerPhysVols[iLayer]);
    }
  }

  // Place the envelope
  DD4hep::Geometry::Volume motherVol = aLcdd.pickMotherVolume(caloDetElem);
  DD4hep::Geometry::PlacedVolume envelopePhysVol = motherVol.placeVolume(envelopeVol);
  envelopePhysVol.addPhysVolID("system", xmlDetElem.id());
  caloDetElem.setPlacement(envelopePhysVol);
  return caloDetElem;
}
}  // namespace det

DECLARE_DETELEMENT(EmCaloBarrelInclined, det::createECalBarrelInclined)
