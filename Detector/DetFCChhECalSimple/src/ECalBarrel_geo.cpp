// DD4hep includes
#include "DD4hep/DetFactoryHelper.h"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

// FCCSW includes
// #include "DetExtensions/DetCylinderVolume.h"

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
    // envelopeVolume.setVisAttributes(lcdd, dimensions.visStr());
    envelopeVolume.setVisAttributes(lcdd.invisible());

    xml_comp_t cryostat = xmlElement.child("cryostat");
    Dimension cryo_dims(cryostat.dimensions());
    double cryo_thickness=cryo_dims.thickness();
  
    xml_comp_t calo = xmlElement.child("calorimeter");
    Dimension calo_dims(calo.dimensions());
    std::string calo_name=calo.nameStr();
    double calo_id=calo.id();
  
    xml_comp_t active = calo.child("active_layers");
    std::string active_mat=active.materialStr();
    double active_tck=active.thickness();
  
    xml_comp_t passive = calo.child("passive_layers");
    std::string passive_mat=passive.materialStr();
    double passive_tck=passive.thickness();  

    //segmentation in Z direction
    xml_comp_t caloZmodule = calo.child("caloZmodule");
    double halfz_cell = caloZmodule.thickness();

    // set the sensitive detector type to the DD4hep calorimeter
    DD4hep::XML::Dimension sdTyp = xmlDet.child(_U(sensitive));
    sensDet.setType(sdTyp.typeStr());

    //calibration hits?
    //xml_comp_t calibHits = xmlElement.child("calibration_hits");
    //std::string do_calibHits=calibHits.labelStr();
    //if (do_calibHits=="true")  {
    //  lLog << MSG::INFO <<"Running simulations with calibration hits" << endmsg;
    //}

    bool do_calibHits=false;
    if (do_calibHits)  {
      lLog << MSG::INFO <<"Running simulations with calibration hits" << endmsg;
    }


    if (halfz_cell>calo_dims.dz()) {
      lLog << MSG::WARNING <<"Overlay in ECal construction!!!!! Redefining the boundaries" << endmsg;
      lLog << MSG::WARNING <<" Problem in Z segmentation: calorimeter half_z "<< calo_dims.dz() << " cm, required calorimeter half_z " << caloZmodule.thickness()<<" cm" <<endmsg;
      halfz_cell = calo_dims.dz();
    }

    // Step 1 : cryostat
  
    DetElement cryo(cryostat.nameStr(), 0);
    DD4hep::Geometry::Tube cryoShape(cryo_dims.rmin() , cryo_dims.rmax(), cryo_dims.dz());
    lLog << MSG::INFO << "ECAL Building cryostat from " << cryo_dims.rmin() << " cm to " << cryo_dims.rmax() << " cm"<< endmsg;
    Volume cryoVol(cryostat.nameStr(), cryoShape, lcdd.material(cryostat.materialStr()));
    if (do_calibHits) cryoVol.setSensitiveDetector(sensDet);
    PlacedVolume placedCryo = envelopeVolume.placeVolume(cryoVol);
    placedCryo.addPhysVolID("cryo", cryostat.id());
    cryo.setPlacement(placedCryo);

    // Step 2 : fill cryostat with active medium

    DetElement calo_bath(active_mat, 0);
    DD4hep::Geometry::Tube bathShape(cryo_dims.rmin()+cryo_thickness , cryo_dims.rmax()-cryo_thickness, cryo_dims.dz()-cryo_thickness);
    lLog << MSG::INFO << "ECAL: Filling cryostat with active medium from " << cryo_dims.rmin()+cryo_thickness << " cm to " << cryo_dims.rmax()-cryo_thickness << " cm" << endmsg;
    Volume bathVol(active_mat, bathShape, lcdd.material(active_mat));
    if (do_calibHits) bathVol.setSensitiveDetector(sensDet);
    PlacedVolume placedBath = cryoVol.placeVolume(bathVol);
    placedBath.addPhysVolID("bath", 1);
    calo_bath.setPlacement(placedBath);
  
    // Step 3 : create the actual calorimeter

    int n_samples_r= (calo_dims.rmax()-  calo_dims.rmin() - passive_tck)/(passive_tck+active_tck);
    lLog << MSG::INFO <<"++++++++++++++++++++++++++ nSamplings in r: "<<n_samples_r<<" , passive thickness " << passive_tck << " cm, active thickness " << active_tck << " cm" << endmsg;

    int n_samples_z = calo_dims.dz()/halfz_cell;
    lLog << MSG::INFO <<"++++++++++++++++++++++++++ nSamplings in z: "<<n_samples_z<< " , cell size in z " <<  2*halfz_cell << " cm" << endmsg;

    int n_samples_phi = calo_dims.nphi();
    double delta_phi=2*dd4hep::pi/n_samples_phi;
    lLog << MSG::INFO <<"++++++++++++++++++++++++++ nSamplings in phi: "<<n_samples_phi << " , deltaphi " << delta_phi << " rad" <<endmsg;

    //Real dimensions defined by the size of the cells/layers  
    double calo_tck=n_samples_r*(active_tck+passive_tck)+passive_tck;
    double calo_halfz=n_samples_z*halfz_cell;

    double rmin_calo = calo_dims.rmin();
    double rmax_calo = calo_dims.rmin()+calo_tck;

    //Check the calorimeter dimensions
    if ( (rmin_calo<(cryo_dims.rmin()+cryo_thickness)) || 
	 (rmax_calo>(cryo_dims.rmax()-cryo_thickness)) ) {
      lLog << MSG::WARNING <<"Overlay in ECal construction!!!!! Redefining the boundaries" << endmsg;
      lLog << MSG::WARNING <<" Cryo boundaries "<< cryo_dims.rmin()+cryo_thickness << " cm , " << cryo_dims.rmax()-cryo_thickness <<" cm"<<endmsg;
      lLog << MSG::WARNING <<" Calo volume required from " << rmin_calo << " cm to " << rmax_calo << " cm" <<endmsg;
      rmin_calo = cryo_dims.rmin()+cryo_thickness;
      rmax_calo = cryo_dims.rmax()-cryo_thickness;
    }

    //Prepare one segment in z (halfz_cell size)
    DD4hep::Geometry::Tube caloZShape(rmin_calo, rmax_calo, halfz_cell);
    lLog << MSG::INFO << "ECAL: Building the actual calorimeter from " << rmin_calo << " cm to " << rmax_calo << " cm" << endmsg;
    Volume caloZVol(passive_mat, caloZShape, lcdd.air());
    
    //Prepare one module in phi - tube shape (start_phi and delta_phi, z - cell size )
    double start_phi = 0.0;
    DD4hep::Geometry::Tube caloPhiShape(rmin_calo, rmax_calo, halfz_cell, start_phi, delta_phi);
    Volume caloPhiVol(passive_mat, caloPhiShape, lcdd.air());
  
    double layer_r_passive=rmin_calo;
    double layer_r_active=rmin_calo+passive_tck;

    // loop on the sensitive layers
    for (int i=0;i<n_samples_r;i++)
      {
	// place passive layers
	DetElement caloPassiveLayer(passive_mat+"_passive", i+1);
	DD4hep::Geometry::Tube layerPassiveShape(layer_r_passive, layer_r_passive+passive_tck, calo_halfz, start_phi, delta_phi);
	Volume layerPassiveVol(passive_mat, layerPassiveShape, lcdd.material(passive_mat));
	if (do_calibHits) layerPassiveVol.setSensitiveDetector(sensDet);
	PlacedVolume placedPassiveLayer = caloPhiVol.placeVolume(layerPassiveVol);
	placedPassiveLayer.addPhysVolID("r_layer_passive", i+1);
	caloPassiveLayer.setPlacement(placedPassiveLayer);

	layer_r_passive += passive_tck+active_tck;

	// place active layers
	DetElement caloActiveLayer(active_mat+"_active", i+1);
	DD4hep::Geometry::Tube layerActiveShape(layer_r_active, layer_r_active+active_tck, calo_halfz, start_phi, delta_phi);
	// lLog << MSG::DEBUG << "ECAL senst. layers :  #" << i << " from " << layer_r << " to " <<  layer_r+active_tck << endmsg;
	Volume layerActiveVol(active_mat, layerActiveShape, lcdd.material(active_mat));
	layerActiveVol.setSensitiveDetector(sensDet);
	PlacedVolume placedActiveLayer = caloPhiVol.placeVolume(layerActiveVol);
	placedActiveLayer.addPhysVolID("r_layer_active", i+1);
	caloActiveLayer.setPlacement(placedActiveLayer);
	//layerActiveVol.setVisAttributes(lcdd,active.visStr());
	layer_r_active+=passive_tck+active_tck;
      }
  
    // add last passive layer
    DetElement caloPassiveLayer(passive_mat+"_passive", n_samples_r+1);
    DD4hep::Geometry::Tube layerPassiveShape(layer_r_passive, layer_r_passive+passive_tck, calo_halfz, start_phi, delta_phi);
    Volume layerPassiveVol(passive_mat, layerPassiveShape, lcdd.material(passive_mat));
    PlacedVolume placedPassiveLayer = caloPhiVol.placeVolume(layerPassiveVol);
    placedPassiveLayer.addPhysVolID("r_layer_passive", n_samples_r+1);
    caloPassiveLayer.setPlacement(placedPassiveLayer);

    
    // place caloPhiVol in phi
    for (int i=0;i<n_samples_phi;i++)
      {
	double phi = -delta_phi/2.+i*delta_phi;
	lLog << MSG::DEBUG << "i  #" << i << " deltaphi " << delta_phi << " phi " << phi << endmsg;
	DD4hep::Geometry::Position offset_phi(0, 0, 0);
	DD4hep::Geometry::Transform3D trans( DD4hep::Geometry::RotationZ(phi), 
					     offset_phi );
	DetElement caloPhiMod("calo_mod_phi", i+1);
	PlacedVolume placedCaloPhiVolume = caloZVol.placeVolume(caloPhiVol, trans);
	placedCaloPhiVolume.addPhysVolID("phi_layer", i+1);
	caloPhiMod.setPlacement(placedCaloPhiVolume);
	caloPhiMod.setVisAttributes(lcdd,caloZmodule.visStr(),caloPhiVol);  
      }
    

    // place caloZVol along z axis
    for (int i=0;i<n_samples_z;i++)
      {
	double zOffset = -calo_halfz+(2*i+1)*halfz_cell;
	DD4hep::Geometry::Position offset(0, 0, zOffset);
	DetElement caloZMod("calo_cells_inz", i+1);
	PlacedVolume placedCaloZVolume = bathVol.placeVolume(caloZVol, offset);
	placedCaloZVolume.addPhysVolID("z_layer", i+1);
	caloZMod.setPlacement(placedCaloZVolume);
	//caloZMod.setVisAttributes(lcdd,caloZmodule.visStr(),caloZVol);  
      }
  
 
    // place envelope (or barrel) volume
    Volume motherVol = lcdd.pickMotherVolume(eCal);
    PlacedVolume placedECal = motherVol.placeVolume(envelopeVolume);
    placedECal.addPhysVolID("system", eCal.id());
    eCal.setPlacement(placedECal);
 
    return eCal;

  }
} // namespace det

DECLARE_DETELEMENT(EmCaloBarrel, det::createECal)
