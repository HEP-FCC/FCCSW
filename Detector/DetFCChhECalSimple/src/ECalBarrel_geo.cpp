// DD4hep includes
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

static DD4hep::Geometry::Ref_t createECal (DD4hep::Geometry::LCDD& lcdd,xml_h xmlElement,
        DD4hep::Geometry::SensitiveDetector sensDet)
{
  ServiceHandle<IMessageSvc> msgSvc("MessageSvc", "ECalConstruction");
  MsgStream lLog(&(*msgSvc), "ECalConstruction");

  xml_det_t xmlDet = xmlElement;
  std::string detName = xmlDet.nameStr();
  //Make DetElement
  DetElement eCal(detName, xmlDet.id());

  // Make volume that envelopes the whole barrel; set material to air
  Dimension dimensions(xmlDet.dimensions());
  DD4hep::Geometry::Tube envelopeShape(dimensions.rmin(), dimensions.rmax(), dimensions.dz());
  Volume envelopeVolume(detName, envelopeShape, lcdd.air());
  // Invisibility seems to be broken in visualisation tags, have to hardcode that
  envelopeVolume.setVisAttributes(lcdd, dimensions.visStr());

  xml_comp_t cryostat = xmlElement.child("cryostat");
  Dimension cryo_dims(cryostat.dimensions());
  double cryo_thickness=cryo_dims.thickness();

  xml_comp_t calo = xmlElement.child("calorimeter");
  Dimension calo_dims(calo.dimensions());
  std::string calo_name=calo.nameStr();

  xml_comp_t active = calo.child("active_layers");
  std::string active_mat=active.materialStr();
  double active_tck=active.thickness();

  xml_comp_t passive = calo.child("passive_layers");
  std::string passive_mat=passive.materialStr();
  double passive_tck=passive.thickness();

  //cryo_thickness < 0 => Run without a cryostat
  Volume bathVol; //volume inside cryostat filled with active medium
  if (cryo_thickness>0) {
    // Step 1 : cryostat
    DetElement cryo(cryostat.nameStr(), 0);
    DD4hep::Geometry::Tube cryoShape(cryo_dims.rmin() , cryo_dims.rmax(), cryo_dims.dz());
    lLog << MSG::DEBUG << "ECAL cryostat: rmin " << cryo_dims.rmin() << " rmax " << cryo_dims.rmax() << endmsg;
    Volume cryoVol(cryostat.nameStr(), cryoShape, lcdd.material(cryostat.materialStr()));
    PlacedVolume placedCryo = envelopeVolume.placeVolume(cryoVol);
    placedCryo.addPhysVolID("ECAL_Cryo", 1);
    cryo.setPlacement(placedCryo);
    // Step 2 : fill cryostat with active medium
    DetElement calo_bath(active_mat+"_notSensitive", 0);
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
  DetElement caloDet(calo_name, 1);
  DD4hep::Geometry::Tube caloShape(calo_dims.rmin() , calo_dims.rmin()+calo_tck, calo_dims.dz());
  lLog << MSG::DEBUG << "ECAL actual calorimeter: rmin " << calo_dims.rmin() << " rmax " <<   calo_dims.rmin()+calo_tck << endmsg;
  Volume caloVol(passive_mat, caloShape, lcdd.material(passive_mat));
  PlacedVolume placedCalo;
  if (cryo_thickness>0) {
    placedCalo = bathVol.placeVolume(caloVol);
  }
  else {
    placedCalo = envelopeVolume.placeVolume(caloVol);
  }
  placedCalo.addPhysVolID("EM_barrel", 1);
  caloDet.setPlacement(placedCalo);

  // set the sensitive detector type to the DD4hep calorimeter
  DD4hep::Geometry::SensitiveDetector sd = sensDet;
  DD4hep::XML::Dimension sd_typ = xmlDet.child(_U(sensitive));
  sd.setType(sd_typ.typeStr());

  // loop on the sensitive layers
  for (int i=0;i<active_samples;i++)
  {
    double layer_r=calo_dims.rmin()+passive_tck+i*(passive_tck+active_tck);
    DetElement caloLayer(active_mat+"_sensitive", i);
    DD4hep::Geometry::Tube layerShape(layer_r , layer_r+active_tck, calo_dims.dz());
    Volume layerVol(active_mat+"_sensitive", layerShape, lcdd.material(active_mat));
    PlacedVolume placedLayer = caloVol.placeVolume(layerVol);
    placedLayer.addPhysVolID("active_layer", i);
    caloLayer.setPlacement(placedLayer);
    layerVol.setSensitiveDetector(sensDet);
  }

  //Place envelope (or barrel) volume
  Volume motherVol = lcdd.pickMotherVolume(eCal);
  PlacedVolume placedECal = motherVol.placeVolume(envelopeVolume);
  placedECal.addPhysVolID("system", eCal.id());
  eCal.setPlacement(placedECal);
  return eCal;

}
} // namespace det

DECLARE_DETELEMENT(EmCaloBarrel, det::createECal)

