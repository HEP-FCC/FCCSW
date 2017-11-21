// DD4hep includes
#include "DD4hep/DetFactoryHelper.h"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "cmath"

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



  xml_comp_t calo = xmlElement.child(_Unicode(calorimeter));
  Dimension calo_dims(calo.dimensions());
  std::string calo_name=calo.nameStr();
  double calo_id=calo.id();

  xml_comp_t trkr = calo.child("tracker");
  std::string trkr_mat = trkr.materialStr();
  double trkr_tck = trkr.thickness();

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

  // here we take the fabs value of padding as if it is negative it goes before the W volume
  double module_tck = active_tck+substrate_tck+passive_tck+std::fabs(padding_tck);
  double calo_tck=(active_samples*module_tck) + trkr_tck;

  lLog << MSG::INFO << "Tracker Material Thickness = " << trkr_tck << endmsg;

  lLog << MSG::INFO << "ECAL Epitaxial Thickness = " << active_tck << endmsg;
  lLog << MSG::INFO << "ECAL Substrate Thickness = " << substrate_tck << endmsg;
  lLog << MSG::INFO << "ECAL Passive Thickness = " << passive_tck << endmsg;  
  lLog << MSG::INFO << "ECAL Between Layers Thickness = " << padding_tck << endmsg;
  lLog << MSG::INFO << "ECAL Module Thickness = " << module_tck <<  endmsg;

  lLog << MSG::INFO << "nSamplings Layers = " << active_samples << endmsg;
  lLog << MSG::INFO << "Total Calorimeter thickness = " << calo_tck  << endmsg;

   // Make volume that envelopes the whole barrel; set material to air
  Dimension dimensions(xmlDet.dimensions());
  //DD4hep::Geometry::Tube envelopeShape(dimensions.rmin(), dimensions.rmax(), dimensions.dz());
  DD4hep::Geometry::Tube envelopeShape(dimensions.rmin(), dimensions.rmin()+calo_tck, dimensions.dz());
  Volume envelopeVolume(detName, envelopeShape, lcdd.air());
  // Invisibility seems to be broken in visualisation tags, have to hardcode that
  std::cout << "dimensions.visStr() = " << dimensions.visStr()<< std::endl;
  envelopeVolume.setVisAttributes(lcdd, dimensions.visStr());

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

  // if we add material for the tracker then it needs to be placed first
  if(trkr_tck > 0.0) {
    DetElement trkrLayer(trkr_mat, 0);
    DD4hep::Geometry::Tube trkrShape(calo_dims.rmin(), calo_dims.rmin()+trkr_tck, calo_dims.dz());
    lLog << MSG::DEBUG << "TRKR: Tracker from " << calo_dims.rmin() << " to " << calo_dims.rmin()+trkr_tck  << endmsg;
    Volume trkrVolume(trkr_mat, trkrShape, lcdd.material(trkr_mat));
    PlacedVolume placedTrkrLayer = caloVolume.placeVolume(trkrVolume);
    //placedTrkrLayer.addPhysVolID("trkr", 0);
    //placedTrkrLayer.addPhysVolID("layer", 0);
    trkrLayer.setPlacement(placedTrkrLayer);
  }

  // Calorimeter runs
  // 1) epitaxial
  // 2) substrate
  // 3) passive material  (absorber)
  // 4) air gap between layers (this is the caleVolume made of air

  // STANDARD CONFIGURATION WITH PADDING (air) >= 0 mm
  // |    MODULE 1   |    MODULE 2   |  ...  |   MODULE N    |  
  // |   |   |   |   |   |   |   |   |  ...  |   |   |   |   |
  // | E | S | A | A | E | S | A | A |  ...  | E | S | A | A |
  // | P | U | B | I | P | U | B | I |  ...  | P | U | B | I |
  // | I | B | S | R | I | B | S | R |  ...  | I | B | S | R |
  // |   |   |   |   |   |   |   |   |  ...  |   |   |   |   |

  // ALTERNATIVE CONFIGURATION WITH PADDING (air) < 0 mm
  // |    MODULE 1   |    MODULE 2   |  ...  |   MODULE N    |  
  // |   |   |   |   |   |   |   |   |  ...  |   |   |   |   |
  // | E | S | A | A | E | S | A | A |  ...  | E | S | A | A |
  // | P | U | I | B | P | U | I | B |  ...  | P | U | I | B |
  // | I | B | R | S | I | B | R | S |  ...  | I | B | R | S |
  // |   |   |   |   |   |   |   |   |  ...  |   |   |   |   |  

  // loop on the sensitive and substrate layers and place within the caloVolume
  int l=1;  
  for (int i=0;i<active_samples;i++)
  {
    lLog << MSG::DEBUG << "ECAL: Building Module Layer " << i << endmsg;
    // 1) place the epitaxial layer
    double layer_rmin=calo_dims.rmin()+(i*module_tck) + trkr_tck; // thick last part adds on the tracker material infront
    double layer_rmax=layer_rmin+active_tck;
    DetElement epiLayer(active_mat+"_sensitive", i+1);
    DD4hep::Geometry::Tube epiShape(layer_rmin , layer_rmax, calo_dims.dz());
    lLog << MSG::DEBUG << "\tEpitaxial from " << layer_rmin << " to " <<  layer_rmax << endmsg;
    Volume epiVolume(active_mat+"_sensitive", epiShape, lcdd.material(active_mat));
    PlacedVolume placedEpiLayer = caloVolume.placeVolume(epiVolume);
    placedEpiLayer.addPhysVolID("layer", i+1);
    placedEpiLayer.addPhysVolID("digital", 1);
    l++;
    epiLayer.setPlacement(placedEpiLayer);
    epiVolume.setSensitiveDetector(sensDet);

    // 2) place the substrate
    layer_rmin = layer_rmax;
    layer_rmax = layer_rmin+substrate_tck;
    DetElement subLayer(substrate_mat+"_substrate", i+1);
    DD4hep::Geometry::Tube subShape(layer_rmin , layer_rmax, calo_dims.dz());
    lLog << MSG::DEBUG << "\tSubstrate from " << layer_rmin << " to " <<  layer_rmax << endmsg;
    Volume subVolume(substrate_mat+"_substrate", subShape, lcdd.material(active_mat));
    PlacedVolume placedSubLayer = caloVolume.placeVolume(subVolume);
    placedSubLayer.addPhysVolID("layer", 1+i);
    placedSubLayer.addPhysVolID("digital", 0);
    subLayer.setPlacement(placedSubLayer);
    subVolume.setSensitiveDetector(sensDet);
    
    // 3) place the passive material
    // if padding_tck < 0 then the air gap goes infront of the passive material
    if(padding_tck < 0) {
      layer_rmin = layer_rmax-padding_tck;
    }
    else {
      layer_rmin = layer_rmax;
    }
    layer_rmax = layer_rmin+passive_tck;
    DetElement passiveLayer(passive_mat+"_passive", i+1);
    DD4hep::Geometry::Tube passiveShape(layer_rmin , layer_rmax, calo_dims.dz());
    lLog << MSG::DEBUG << "\t" << passive_mat << " from " << layer_rmin << " to " <<  layer_rmax << endmsg;
    Volume passiveVolume(passive_mat, passiveShape, lcdd.material(passive_mat));
    PlacedVolume placedPassiveLayer = caloVolume.placeVolume(passiveVolume);
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

