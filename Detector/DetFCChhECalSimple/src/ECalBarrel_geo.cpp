// DD4hep includes
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

static DD4hep::Geometry::Ref_t createECal(DD4hep::Geometry::LCDD& lcdd, xml_h xmlElement,
                                          DD4hep::Geometry::SensitiveDetector sensDet) {
  ServiceHandle<IMessageSvc> msgSvc("MessageSvc", "ECalConstruction");
  MsgStream lLog(&(*msgSvc), "ECalConstruction");

  xml_det_t xmlDet = xmlElement;
  std::string detName = xmlDet.nameStr();
  // Make DetElement
  DetElement eCal(detName, xmlDet.id());

  // Make volume that envelopes the whole barrel; set material to air
  Dimension dimensions(xmlDet.dimensions());
  DD4hep::Geometry::Tube envelopeShape(dimensions.rmin(), dimensions.rmax(), dimensions.dz());
  Volume envelopeVolume(detName, envelopeShape, lcdd.air());
  // Invisibility seems to be broken in visualisation tags, have to hardcode that
  envelopeVolume.setVisAttributes(lcdd, dimensions.visStr());

  xml_comp_t cryostat = xmlElement.child(_Unicode(cryostat));
  Dimension cryoDim(cryostat.dimensions());
  double cryoThickness = cryoDim.thickness();

  xml_comp_t calo = xmlElement.child(_Unicode(calorimeter));
  Dimension caloDim(calo.dimensions());

  xml_comp_t active = calo.child(_Unicode(active_layers));
  std::string activeMaterial = active.materialStr();
  double activeThickness = active.thickness();

  xml_comp_t passive = calo.child(_Unicode(passive_layers));
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

  // cryoThickness < 0 => Run without a cryostat
  Volume bathVol;  // volume inside cryostat filled with active medium
  DetElement caloBath(eCal, activeMaterial + "_notSensitive", 0);
  if (cryoThickness > 0) {
    // Step 1 : cryostat
    DD4hep::Geometry::Tube cryoShape(cryoDim.rmin(), cryoDim.rmax(), cryoDim.dz());
    lLog << MSG::DEBUG << "ECAL cryostat: rmin " << cryoDim.rmin() << " rmax " << cryoDim.rmax() << endmsg;
    Volume cryoVol(cryostat.nameStr(), cryoShape, lcdd.material(cryostat.materialStr()));
    PlacedVolume placedCryo = envelopeVolume.placeVolume(cryoVol);
    placedCryo.addPhysVolID("ECAL_Cryo", 1);
    DetElement cryo(eCal, cryostat.nameStr(), 0);
    cryo.setPlacement(placedCryo);
    // Step 2 : fill cryostat with active medium
    DD4hep::Geometry::Tube bathShape(cryoDim.rmin() + cryoThickness, cryoDim.rmax() - cryoThickness,
                                     cryoDim.dz() - cryoThickness);
    lLog << MSG::DEBUG << "ECAL " << activeMaterial << " bath: rmin " << cryoDim.rmin() + cryoThickness << " rmax "
         << cryoDim.rmax() - cryoThickness << endmsg;
    bathVol = Volume(activeMaterial + "_notSensitive", bathShape, lcdd.material(activeMaterial));
    PlacedVolume placedBath = cryoVol.placeVolume(bathVol);
    placedBath.addPhysVolID("bath", 1);
    caloBath.setPlacement(placedBath);
  }

  // Step 3 : create the actual calorimeter
  int activeSamples = (caloDim.rmax() - caloDim.rmin() - passiveThickness) / (passiveThickness + activeThickness);
  double caloThickness = activeSamples * (activeThickness + passiveThickness) + passiveThickness;
  lLog << MSG::DEBUG << "ECAL actual calorimeter: rmin " << caloDim.rmin() << " rmax " << caloDim.rmin() + caloThickness
       << endmsg;

  // set the sensitive detector type to the DD4hep calorimeter
  DD4hep::Geometry::SensitiveDetector sd = sensDet;
  DD4hep::XML::Dimension sdType = xmlDet.child(_U(sensitive));
  sd.setType(sdType.typeStr());

  double glueThickness = passiveGlueThickness / 2.;
  double outerThickness = passiveOuterThickness / 2.;
  double innerThickness = passiveInnerThickness;
  lLog << MSG::DEBUG << "Passive material:  outer = 2x " << outerThickness << ", glue = 2x " << glueThickness
       << ", inner = " << innerThickness << endmsg;
  // loop on the sensitive layers
  for (int i = 0; i < activeSamples; i++) {
    double layerRadius = caloDim.rmin() + passiveThickness + i * (passiveThickness + activeThickness);
    DD4hep::Geometry::Tube outerShape1(layerRadius - passiveThickness, layerRadius - passiveThickness + outerThickness,
                                       caloDim.dz());
    DD4hep::Geometry::Tube glueShape1(layerRadius - passiveThickness + outerThickness,
                                      layerRadius - passiveThickness + outerThickness + glueThickness, caloDim.dz());
    DD4hep::Geometry::Tube innerShape(layerRadius - passiveThickness + outerThickness + glueThickness,
                                      layerRadius - outerThickness - glueThickness, caloDim.dz());
    DD4hep::Geometry::Tube glueShape2(layerRadius - outerThickness - glueThickness, layerRadius - outerThickness,
                                      caloDim.dz());
    DD4hep::Geometry::Tube outerShape2(layerRadius - outerThickness, layerRadius, caloDim.dz());
    DD4hep::Geometry::Tube layerShape(layerRadius, layerRadius + activeThickness, caloDim.dz());
    Volume layerVol(activeMaterial + "_sensitive", layerShape, lcdd.material(activeMaterial));
    Volume outerVol1(passiveOuterMaterial + "_below", outerShape1, lcdd.material(passiveOuterMaterial));
    Volume outerVol2(passiveOuterMaterial + "_above", outerShape2, lcdd.material(passiveOuterMaterial));
    Volume glueVol1(passiveGlueMaterial + "_below", glueShape1, lcdd.material(passiveGlueMaterial));
    Volume glueVol2(passiveGlueMaterial + "_above", glueShape2, lcdd.material(passiveGlueMaterial));
    Volume innerVol(passiveInnerMaterial, innerShape, lcdd.material(passiveInnerMaterial));
    lLog << MSG::DEBUG << "layer borders: " << layerRadius - passiveThickness << "\t"
         << layerRadius - passiveThickness + outerThickness << "\t"
         << layerRadius - passiveThickness + outerThickness + glueThickness << "\t"
         << layerRadius - outerThickness - glueThickness << "\t" << layerRadius - outerThickness << "\t" << layerRadius
         << "\t" << layerRadius + activeThickness << endmsg;
    if (cryoThickness > 0) {
      bathVol.placeVolume(outerVol1);
      bathVol.placeVolume(glueVol1);
      bathVol.placeVolume(innerVol);
      bathVol.placeVolume(glueVol2);
      bathVol.placeVolume(outerVol2);
      PlacedVolume placedLayer = bathVol.placeVolume(layerVol);
      placedLayer.addPhysVolID("active_layer", i);
      DetElement caloLayer(caloBath, activeMaterial + "_sensitive" + std::to_string(i), i);
      caloLayer.setPlacement(placedLayer);
    } else {
      envelopeVolume.placeVolume(outerVol1);
      envelopeVolume.placeVolume(glueVol1);
      envelopeVolume.placeVolume(innerVol);
      envelopeVolume.placeVolume(glueVol2);
      envelopeVolume.placeVolume(outerVol2);
      PlacedVolume placedLayer = envelopeVolume.placeVolume(layerVol);
      placedLayer.addPhysVolID("active_layer", i);
      DetElement caloLayer(eCal, activeMaterial + "_sensitive" + std::to_string(i), i);
      caloLayer.setPlacement(placedLayer);
    }
    layerVol.setSensitiveDetector(sensDet);
  }

  // Place envelope (or barrel) volume
  Volume motherVol = lcdd.pickMotherVolume(eCal);
  PlacedVolume placedECal = motherVol.placeVolume(envelopeVolume);
  placedECal.addPhysVolID("system", eCal.id());
  eCal.setPlacement(placedECal);
  return eCal;
}
}  // namespace det

DECLARE_DETELEMENT(EmCaloBarrel, det::createECal)
