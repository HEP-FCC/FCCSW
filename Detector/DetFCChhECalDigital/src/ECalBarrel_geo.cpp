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

  ServiceHandle<IMessageSvc> msgSvc("MessageSvc", "DECalConstruction");
  MsgStream lLog(&(*msgSvc), "DECalConstruction");

  lLog << MSG::DEBUG << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endmsg;
  lLog << MSG::DEBUG << "\t\t\t Building DECAL " << endmsg;
  lLog << MSG::DEBUG << " +++++++++++++++++++++++++++++++++++++++++++++++++++++" << endmsg;

  xml_det_t xmlDet = xmlElement;
  std::string detName = xmlDet.nameStr();
  //Make DetElement
  DetElement eCal(detName, xmlDet.id());

  // Make volume that envelopes the whole barrel; set material to air
  Dimension dimensions(xmlDet.dimensions());
  DD4hep::Geometry::Tube envelopeShape(dimensions.rmin(), dimensions.rmax(), dimensions.dz());
  Volume envelopeVolume(detName, envelopeShape, lcdd.air());
  // Invisibility seems to be broken in visualisation tags, have to hardcode that
  std::cout << "dimensions.visStr() = " << dimensions.visStr()<< std::endl;
  envelopeVolume.setVisAttributes(lcdd, dimensions.visStr());

  xml_comp_t calo = xmlElement.child("calorimeter");
  Dimension calo_dims(calo.dimensions());
  std::string calo_name=calo.nameStr();
  double calo_id=calo.id();

  xml_comp_t active = calo.child("active_layers");
  std::string active_mat=active.materialStr();
  double active_tck=active.thickness();
  int active_samples=active.attr<int>("nSamplings");
  
  xml_comp_t substrate = calo.child("substrate_layers");
  std::string substrate_mat=substrate.materialStr();
  double substrate_tck=substrate.thickness();

  xml_comp_t passive = calo.child("passive_layers");
  std::string passive_mat=passive.materialStr();
  double passive_tck=passive.thickness();

  xml_comp_t padding = calo.child("between_layers");
  std::string padding_mat=padding.materialStr();
  double padding_tck=padding.thickness();

  double module_tck = active_tck+substrate_tck+passive_tck+padding_tck;
  double calo_tck=active_samples*module_tck;

  lLog << MSG::DEBUG << "ECAL Epitaxial Thickness = " << active_tck << endmsg;
  lLog << MSG::DEBUG << "ECAL Substrate Thickness = " << substrate_tck << endmsg;
  lLog << MSG::DEBUG << "ECAL Passive Thickness = " << passive_tck << endmsg;  
  lLog << MSG::DEBUG << "ECAL Between Layers Thickness = " << padding_tck << endmsg;
  lLog << MSG::DEBUG << "ECAL Module Thickness = " << module_tck <<  endmsg;

  lLog << MSG::DEBUG << "nSamplings Layers = " << active_samples << endmsg;
  lLog << MSG::DEBUG << "Total Calorimeter thickness = " << calo_tck  << endmsg;

  // create a lump of air the entire size of the barrel ECAL
  // place it within the ECAL envelope
  DetElement caloDet(calo_name, calo_id);
  DD4hep::Geometry::Tube caloShape(calo_dims.rmin() , calo_dims.rmin()+calo_tck, calo_dims.dz());
  lLog << MSG::DEBUG << "ECAL: Building the calorimeter from " << calo_dims.rmin() << " to " <<   calo_dims.rmin()+calo_tck << endmsg;
  Volume caloVolume(padding_mat, caloShape, lcdd.material(padding_mat));
  lLog << MSG::DEBUG << "ECAL: Filling the calorimeter volume with " << padding_mat << endmsg;  
  PlacedVolume placedCalo = envelopeVolume.placeVolume(caloVolume);
  placedCalo.addPhysVolID("EM_barrel", calo_id);
  caloDet.setPlacement(placedCalo);

  // set the sensitive detector type to the DD4hep calorimeter
  sensDet.setType("DigitalCalorimeterSD");


  // Calorimeter runs
  // 1) epitaxial
  // 2) substrate
  // 3) passive material 
  // 4) air gap between layers (this is the caleVolume made of air

  // loop on the sensitive and substrate layers and place within the caloVolume
  for (int i=0;i<active_samples;i++)
  {
    lLog << MSG::DEBUG << "ECAL: Building Module Layer " << i << endmsg;

    // 1) place the epitaxial layer
    double layer_rmin=calo_dims.rmin()+(i*module_tck);
    double layer_rmax=layer_rmin+active_tck;
    DetElement epiLayer(active_mat+"_sensitive", i+1);
    DD4hep::Geometry::Tube epiShape(layer_rmin , layer_rmax, calo_dims.dz());
    lLog << MSG::DEBUG << "\tEpitaxial from " << layer_rmin << " to " <<  layer_rmax << endmsg;
    Volume epiVolume(active_mat, epiShape, lcdd.material(active_mat));
    PlacedVolume placedEpiLayer = caloVolume.placeVolume(epiVolume);
    placedEpiLayer.addPhysVolID("layer", i+1);
    epiLayer.setPlacement(placedEpiLayer);
    epiVolume.setSensitiveDetector(sensDet);

    // 2) place the substrate
    layer_rmin = layer_rmax;
    layer_rmax = layer_rmin+substrate_tck;
    DetElement subLayer(substrate_mat+"_substrate", i+1);
    DD4hep::Geometry::Tube subShape(layer_rmin , layer_rmax, calo_dims.dz());
    lLog << MSG::DEBUG << "\tSubstrate from " << layer_rmin << " to " <<  layer_rmax << endmsg;
    Volume subVolume(active_mat, subShape, lcdd.material(active_mat));
    PlacedVolume placedSubLayer = caloVolume.placeVolume(subVolume);
    placedSubLayer.addPhysVolID("sub", i+1);
    subLayer.setPlacement(placedSubLayer);

    // 3) place the passive material
    layer_rmin = layer_rmax;
    layer_rmax = layer_rmin+passive_tck;
    DetElement passiveLayer(passive_mat+"_passive", i+1);
    DD4hep::Geometry::Tube passiveShape(layer_rmin , layer_rmax, calo_dims.dz());
    lLog << MSG::DEBUG << "\t" << passive_mat << " from " << layer_rmin << " to " <<  layer_rmax << endmsg;
    Volume passiveVolume(passive_mat, passiveShape, lcdd.material(passive_mat));
    PlacedVolume placedPassiveLayer = caloVolume.placeVolume(passiveVolume);
    placedPassiveLayer.addPhysVolID("passive", i+1);
    passiveLayer.setPlacement(placedPassiveLayer);   

    // 4) the air volume alread exists so don't do anything
    

  }

  //Place envelope (or barrel) volume
  Volume motherVol = lcdd.pickMotherVolume(eCal);
  PlacedVolume placedECal = motherVol.placeVolume(envelopeVolume);
  placedECal.addPhysVolID("system", eCal.id());
  eCal.setPlacement(placedECal);
  return eCal;

}
} // namespace det

DECLARE_DETELEMENT(EmCaloDigiBarrel, det::createECal)

