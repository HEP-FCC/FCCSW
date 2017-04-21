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
  Dimension cryo_dims(cryostat.dimensions());
  double cryo_thickness = cryo_dims.thickness();

  xml_comp_t calo = xmlElement.child(_Unicode(calorimeter));
  Dimension calo_dims(calo.dimensions());
  std::string calo_name = calo.nameStr();

  xml_comp_t active = calo.child(_Unicode(active_layers));
  std::string active_mat = active.materialStr();
  double active_tck = active.thickness();

  xml_comp_t passive = calo.child(_Unicode(passive_layers));
  DD4hep::XML::DetElement passive_inner = passive.child(_Unicode(inner));
  DD4hep::XML::DetElement passive_outer = passive.child(_Unicode(outer));
  DD4hep::XML::DetElement passive_glue = passive.child(_Unicode(glue));
  std::string passive_inner_mat = passive_inner.materialStr();
  std::string passive_outer_mat = passive_outer.materialStr();
  std::string passive_glue_mat = passive_glue.materialStr();
  double passive_inner_thickness = passive_inner.thickness();
  double passive_outer_thickness = passive_outer.thickness();
  double passive_glue_thickness = passive_glue.thickness();
  double passive_tck = passive_inner_thickness + passive_outer_thickness + passive_glue_thickness;

  //cryo_thickness < 0 => Run without a cryostat
  Volume bathVol; //volume inside cryostat filled with active medium
  DetElement calo_bath(eCal, active_mat+"_notSensitive", 0);
  if (cryo_thickness>0) {
    // Step 1 : cryostat
    DD4hep::Geometry::Tube cryoShape(cryo_dims.rmin() , cryo_dims.rmax(), cryo_dims.dz());
    lLog << MSG::DEBUG << "ECAL cryostat: rmin " << cryo_dims.rmin() << " rmax " << cryo_dims.rmax() << endmsg;
    Volume cryoVol(cryostat.nameStr(), cryoShape, lcdd.material(cryostat.materialStr()));
    PlacedVolume placedCryo = envelopeVolume.placeVolume(cryoVol);
    placedCryo.addPhysVolID("ECAL_Cryo", 1);
    DetElement cryo(eCal, cryostat.nameStr(), 0);
    cryo.setPlacement(placedCryo);
    // Step 2 : fill cryostat with active medium
    DD4hep::Geometry::Tube bathShape(cryo_dims.rmin()+cryo_thickness , cryo_dims.rmax()-cryo_thickness, cryo_dims.dz()-cryo_thickness);
    lLog << MSG::DEBUG << "ECAL " << active_mat << " bath: rmin " << cryo_dims.rmin()+cryo_thickness << " rmax " << cryo_dims.rmax()-cryo_thickness << endmsg;
    bathVol = Volume(active_mat+"_notSensitive", bathShape, lcdd.material(active_mat));
    PlacedVolume placedBath = cryoVol.placeVolume(bathVol);
    placedBath.addPhysVolID("bath", 1);
    calo_bath.setPlacement(placedBath);
  }

  // Step 3 : create the actual calorimeter
  int active_samples= (calo_dims.rmax()-  calo_dims.rmin() - passive_tck)/(passive_tck+active_tck);
  double calo_tck=active_samples*(active_tck+passive_tck)+passive_tck;
  lLog << MSG::DEBUG << "ECAL actual calorimeter: rmin " << calo_dims.rmin() << " rmax " <<   calo_dims.rmin()+calo_tck << endmsg;

  // set the sensitive detector type to the DD4hep calorimeter
  DD4hep::Geometry::SensitiveDetector sd = sensDet;
  DD4hep::XML::Dimension sd_typ = xmlDet.child(_U(sensitive));
  sd.setType(sd_typ.typeStr());

  double glue_thck = passive_glue_thickness / 2.;
  double steel_thck = passive_outer_thickness / 2.;
  double pb_thck = passive_inner_thickness;
  lLog << MSG::DEBUG << "Passive material:  steel = 2x " << steel_thck << ", glue = 2x " <<  glue_thck  << ", lead = " <<  pb_thck << endmsg;
  // loop on the sensitive layers
  for (int i=0;i<active_samples;i++)
  {
    double layer_r=calo_dims.rmin()+passive_tck+i*(passive_tck+active_tck);
    DD4hep::Geometry::Tube steelShape1(layer_r-passive_tck , layer_r-passive_tck+steel_thck, calo_dims.dz());
    DD4hep::Geometry::Tube glueShape1(layer_r-passive_tck+steel_thck , layer_r-passive_tck+steel_thck+glue_thck, calo_dims.dz());
    DD4hep::Geometry::Tube passiveShape(layer_r-passive_tck+steel_thck+glue_thck , layer_r-steel_thck-glue_thck, calo_dims.dz());
    DD4hep::Geometry::Tube glueShape2(layer_r-steel_thck-glue_thck, layer_r-steel_thck , calo_dims.dz());
    DD4hep::Geometry::Tube steelShape2(layer_r-steel_thck, layer_r , calo_dims.dz());
    DD4hep::Geometry::Tube layerShape(layer_r , layer_r+active_tck, calo_dims.dz());
    Volume layerVol(active_mat+"_sensitive", layerShape, lcdd.material(active_mat));
    Volume steelVol1(passive_outer_mat+"_below", steelShape1, lcdd.material(passive_outer_mat));
    Volume steelVol2(passive_outer_mat+"_above", steelShape2, lcdd.material(passive_outer_mat));
    Volume glueVol1(passive_glue_mat+"_below", glueShape1, lcdd.material(passive_glue_mat));
    Volume glueVol2(passive_glue_mat+"_above", glueShape2, lcdd.material(passive_glue_mat));
    Volume passiveVol(passive_inner_mat, passiveShape, lcdd.material(passive_inner_mat));
    lLog << MSG::DEBUG << "layer borders: " <<layer_r-passive_tck
         << "\t" <<layer_r-passive_tck+steel_thck
         << "\t" <<layer_r-passive_tck+steel_thck+glue_thck
         << "\t" <<layer_r-steel_thck-glue_thck
         << "\t" << layer_r-steel_thck
         << "\t" << layer_r
         << "\t" << layer_r+active_tck <<endmsg;
    if (cryo_thickness>0) {
      bathVol.placeVolume(steelVol1);
      bathVol.placeVolume(glueVol1);
      bathVol.placeVolume(passiveVol);
      bathVol.placeVolume(glueVol2);
      bathVol.placeVolume(steelVol2);
      PlacedVolume placedLayer = bathVol.placeVolume(layerVol);
      placedLayer.addPhysVolID("active_layer", i);
      DetElement caloLayer(calo_bath, active_mat+"_sensitive"+std::to_string(i), i);
      caloLayer.setPlacement(placedLayer);
    } else {
      envelopeVolume.placeVolume(steelVol1);
      envelopeVolume.placeVolume(glueVol1);
      envelopeVolume.placeVolume(passiveVol);
      envelopeVolume.placeVolume(glueVol2);
      envelopeVolume.placeVolume(steelVol2);
      PlacedVolume placedLayer = envelopeVolume.placeVolume(layerVol);
      placedLayer.addPhysVolID("active_layer", i);
      DetElement caloLayer(eCal, active_mat+"_sensitive"+std::to_string(i), i);
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
