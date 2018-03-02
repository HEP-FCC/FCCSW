// DD4hep includes
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Shapes.h"
#include "DD4hep/Objects.h"

#include "XML/XMLElements.h"

// Gaudi
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "cmath"

/*ROBBIE
using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;
using dd4hep::Geometry::Volume;
using dd4hep::Geometry::DetElement;
using dd4hep::XML::Dimension;
using dd4hep::Geometry::PlacedVolume;
using dd4hep::Geometry::Trapezoid;
using dd4hep::_toString;
using dd4hep::Geometry::Position;
using dd4hep::Geometry::Box;
using dd4hep::Geometry::Transform3D;
using dd4hep::Geometry::RotationZYX;
using dd4hep::Geometry::Translation3D;
*/


namespace det {

static dd4hep::detail::Ref_t createECal (dd4hep::Detector& lcdd,dd4hep::xml::Handle_t xmlElement,
        dd4hep::SensitiveDetector sensDet)
{

  ServiceHandle<IMessageSvc> msgSvc("MessageSvc", "DECalConstruction");
  MsgStream lLog(&(*msgSvc), "DECalConstruction");

  lLog << MSG::DEBUG << "++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endmsg;
  lLog << MSG::DEBUG << "\t\t\t Building DECAL " << endmsg;
  lLog << MSG::DEBUG << " +++++++++++++++++++++++++++++++++++++++++++++++++++++" << endmsg;
  
  // Getting the detectors various values from the xml file
  
  dd4hep::xml::DetElement xmlDet = xmlElement;
  std::string detName = xmlDet.nameStr();
  //Make DetElement
  dd4hep::DetElement eCal(detName, xmlDet.id());


  
  dd4hep::xml::DetElement calo = xmlElement.child(_Unicode(calorimeter));
  dd4hep::xml::Dimension calo_dims(calo.dimensions());
  std::string calo_name=calo.nameStr();
  double calo_id=calo.id();

  dd4hep::xml::DetElement trkr = calo.child("tracker");
  std::string trkr_mat = trkr.materialStr();
  double trkr_tck = trkr.thickness();

  dd4hep::xml::DetElement active = calo.child("active_layers");
  std::string active_mat=active.materialStr();
  double active_tck=active.thickness();
  //int active_samples=active.attr<int>("nSamplings");
  // need to fix line above as no longer supported
  int active_samples = 50;

  dd4hep::xml::DetElement substrate = calo.child("substrate_layers");
  std::string substrate_mat=substrate.materialStr();
  double substrate_tck=substrate.thickness();

  dd4hep::xml::DetElement passive = calo.child("passive_layers");
  std::string passive_mat=passive.materialStr();
  double passive_tck=passive.thickness();

  dd4hep::xml::DetElement padding = calo.child("between_layers");
  std::string padding_mat=padding.materialStr();
  double padding_tck=padding.thickness();

  // here we take the fabs value of padding as if it is negative it goes before the W volume
  double module_tck = active_tck+substrate_tck+passive_tck+std::fabs(padding_tck);
  double calo_tck=(active_samples*module_tck) + trkr_tck;
  
  //ROBBIE introduce number of staves, modules from the xml file:
  int nStaves = calo_dims.numsides();
  int nModules = calo_dims.nmodules();
  int testing = 1;
  
  lLog << MSG::INFO << "Tracker Material Thickness = " << trkr_tck << endmsg;

  lLog << MSG::INFO << "ECAL Epitaxial Thickness = " << active_tck << endmsg;
  lLog << MSG::INFO << "ECAL Substrate Thickness = " << substrate_tck << endmsg;
  lLog << MSG::INFO << "ECAL Passive Thickness = " << passive_tck << endmsg;  
  lLog << MSG::INFO << "ECAL Between Layers Thickness = " << padding_tck << endmsg;
  lLog << MSG::INFO << "ECAL Module Thickness = " << module_tck <<  endmsg;

  lLog << MSG::INFO << "nSamplings Layers = " << active_samples << endmsg;
  lLog << MSG::INFO << "Total Calorimeter thickness = " << calo_tck  << endmsg;

  //ROBBIE log number of staves/modules in the log file:
  lLog << MSG::INFO << "nStaves = " << nStaves << endmsg;
  lLog << MSG::INFO << "nModules = " << nModules << endmsg;

   // Make volume that envelopes the whole barrel; set material to air
  dd4hep::xml::Dimension dimensions(xmlDet.dimensions());
  dd4hep::Tube envelopeShape(dimensions.rmin(), dimensions.rmax(), dimensions.dz());
  //DD4hep::Geometry::Tube envelopeShape(dimensions.rmin(), dimensions.rmin()+calo_tck, dimensions.dz());
  dd4hep::Volume envelopeVolume(detName, envelopeShape, lcdd.air());
  // Invisibility seems to be broken in visualisation tags, have to hardcode that
  std::cout << "dimensions.visStr() = " << dimensions.visStr()<< std::endl;
  envelopeVolume.setVisAttributes(lcdd, dimensions.visStr());

  // create a lump of air the entire size of the barrel ECAL (Cylindrical)
  // place it within the ECAL envelope
  dd4hep::DetElement caloDet(calo_name, calo_id);
  dd4hep::Tube caloShape(calo_dims.rmin() , /*calo_dims.rmin()+calo_tck*/calo_dims.rmax(), calo_dims.dz());
  lLog << MSG::INFO/*DEBUG*/ << "ECAL: Building the calorimeter from " << calo_dims.rmin() << " to " <<   /*calo_dims.rmin()+calo_tck*/calo_dims.rmax() << endmsg;
  dd4hep::Volume caloVolume(padding_mat, caloShape, lcdd.material(padding_mat));
  lLog << MSG::DEBUG << "ECAL: Filling the calorimeter volume with " << padding_mat << endmsg;  
  dd4hep::PlacedVolume placedCalo = envelopeVolume.placeVolume(caloVolume);
  placedCalo.addPhysVolID("EM_barrel", calo_id);
  caloDet.setPlacement(placedCalo);

  // set the sensitive detector type to the DD4hep calorimeter
  sensDet.setType("DigitalCalorimeterSD");

  // if we add material for the tracker then it needs to be placed first
  if(trkr_tck > 0.0) {
    dd4hep::DetElement trkrLayer(trkr_mat, 0);
    dd4hep::Tube trkrShape(calo_dims.rmin(), calo_dims.rmin()+trkr_tck, calo_dims.dz());
    lLog << MSG::DEBUG << "TRKR: Tracker from " << calo_dims.rmin() << " to " << calo_dims.rmin()+trkr_tck  << endmsg;
    dd4hep::Volume trkrVolume(trkr_mat, trkrShape, lcdd.material(trkr_mat));
    dd4hep::PlacedVolume placedTrkrLayer = caloVolume.placeVolume(trkrVolume);
    //placedTrkrLayer.addPhysVolID("trkr", 0);
    //placedTrkrLayer.addPhysVolID("layer", 0);
    trkrLayer.setPlacement(placedTrkrLayer);
  }


  // START OF TONY's STUFF
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

  //ROBBIE Leave option for circular geometry if nStaves == 1:
  if (nStaves == 1) 
    {
      // loop on the sensitive and substrate layers and place within the caloVolume
      int l=1;  
      for (int i=0;i<active_samples;i++)
	{
	  lLog << MSG::DEBUG << "ECAL: Building Module Layer " << i << endmsg;
	  // 1) place the epitaxial layer
	  double layer_rmin=calo_dims.rmin()+(i*module_tck) + trkr_tck; // thick last part adds on the tracker material infront
	  double layer_rmax=layer_rmin+active_tck;
	  dd4hep::DetElement epiLayer(active_mat+"_sensitive", i+1);
	  dd4hep::Tube epiShape(layer_rmin , layer_rmax, calo_dims.dz());
	  lLog << MSG::DEBUG << "\tEpitaxial from " << layer_rmin << " to " <<  layer_rmax << endmsg;
	  dd4hep::Volume epiVolume(active_mat+"_sensitive", epiShape, lcdd.material(active_mat));
	  dd4hep::PlacedVolume placedEpiLayer = caloVolume.placeVolume(epiVolume);
	  placedEpiLayer.addPhysVolID("layer", i+1);
	  placedEpiLayer.addPhysVolID("digital", 1);
	  l++;
	  epiLayer.setPlacement(placedEpiLayer);
	  epiVolume.setSensitiveDetector(sensDet);
	  
	  // 2) place the substrate
	  layer_rmin = layer_rmax;
	  layer_rmax = layer_rmin+substrate_tck;
	  dd4hep::DetElement subLayer(substrate_mat+"_substrate", i+1);
	  dd4hep::Tube subShape(layer_rmin , layer_rmax, calo_dims.dz());
	  lLog << MSG::DEBUG << "\tSubstrate from " << layer_rmin << " to " <<  layer_rmax << endmsg;
	  dd4hep::Volume subVolume(substrate_mat+"_substrate", subShape, lcdd.material(active_mat));
	  dd4hep::PlacedVolume placedSubLayer = caloVolume.placeVolume(subVolume);
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
	  dd4hep::DetElement passiveLayer(passive_mat+"_passive", i+1);
	  dd4hep::Tube passiveShape(layer_rmin , layer_rmax, calo_dims.dz());
	  lLog << MSG::DEBUG << "\t" << passive_mat << " from " << layer_rmin << " to " <<  layer_rmax << endmsg;
	  dd4hep::Volume passiveVolume(passive_mat, passiveShape, lcdd.material(passive_mat));
	  dd4hep::PlacedVolume placedPassiveLayer = caloVolume.placeVolume(passiveVolume);
	  passiveLayer.setPlacement(placedPassiveLayer);   
	  
	  // 4) the air volume alread exists so don't do anything
	  
	  
	}
    }
  //END OF TONY'S STUFF
  
  else if (testing == 1) {
    double hphi = M_PI/nStaves;
    double rmin = calo_dims.rmin();
    double rmax = calo_dims.rmax();
    double dz = calo_dims.dz();
    double s_pos_r = rmin;
    double trd_z = dz/2;
    
    lLog << MSG::INFO << "HERE WE WILL MEASURE RMIN AND RMAX, TO SEE IF IT LIES BETWEEN 196 AND 197" << endmsg;
    lLog << MSG::INFO << "RMIN IS " << rmin << "    RMAX IS " << rmax << "     DZ IS " << dz << endmsg;

    double phi = 0.0;
    double dphi = 2*hphi;

    double nudge_gap = 0.000001;
    double l_thickness = active_tck + passive_tck + substrate_tck + padding_tck;

    double module_dz = dz/nModules;
    double module_z = -trd_z + (module_dz/2);

    for (int stavenum=0; stavenum<nStaves/**2*/; stavenum++)
      {
	phi -= dphi;
	
	module_z = -trd_z + (module_dz/2);	
	
    for (int modulenum=0; modulenum<nModules; modulenum++)
      {
	s_pos_r = rmin;

	for (int layernum=0; layernum  < active_samples; ++layernum)
	  {
	    double l_dim_x = (s_pos_r * (std::tan(hphi))) - nudge_gap;//72.0;

	    // EPI
	    std::string epi_name = dd4hep::xml::_toString(layernum, "epi%d");
	    dd4hep::Box epi_box(l_dim_x, module_dz/2, active_tck/2);
	    lLog << MSG::INFO << "This means that the epitaxial layer exists between " << epi_box.x() << " and " << -epi_box.x() << " in the x direction" << endmsg;
	    dd4hep::Volume epi_vol(epi_name, epi_box, lcdd.material(active_mat));
	    dd4hep::DetElement epi_slice(epi_name, layernum);
	    lLog << MSG::INFO << "Epitaxial thickness is " << active_tck << endmsg;
	    
	    double mod_y_off = s_pos_r + (0.5*active_tck);
	    double m_pos_x = -mod_y_off*std::sin(phi);
	    double m_pos_y = mod_y_off*std::cos(phi);

	
	    dd4hep::Transform3D epi_tr(dd4hep::RotationZYX(0, phi, M_PI*0.5), dd4hep::Translation3D(-m_pos_x, -m_pos_y, module_z));
	    dd4hep::PlacedVolume epi_phv = caloVolume.placeVolume(epi_vol, epi_tr); 
	
	    lLog << MSG::INFO << "The EPI has been placed at (" << -m_pos_x << ", " << -m_pos_y << ", " << module_z << ")" << endmsg;
	    lLog << MSG::INFO << "That means its ends are at (" << -m_pos_x+((epi_box.x())*(std::cos(phi))) << ", " << -m_pos_y+((epi_box.x())*(std::sin(phi))) << ")  and  (" << -m_pos_x-((epi_box.x())*(std::cos(phi))) << ", " << -m_pos_y-((epi_box.x())*(std::sin(phi))) << ")" << endmsg;
	    lLog << MSG::INFO << "In z, the EPI lies between " << module_z - (epi_box.y()) << " and " << module_z + (epi_box.y()) << endmsg;

	    epi_phv.addPhysVolID("digital", 1);
	    epi_phv.addPhysVolID("layer", layernum);
	    epi_phv.addPhysVolID("stave", stavenum);
	    epi_phv.addPhysVolID("module", modulenum);
	    
	    epi_slice.setPlacement(epi_phv);
	    epi_vol.setSensitiveDetector(sensDet);

	    s_pos_r += active_tck + nudge_gap;
	    //s_pos_r += passive_tck + substrate_tck + padding_tck + (3*nudge_gap);
	    lLog << MSG::INFO << "Radial position is " << s_pos_r << endmsg;


	    //SUBSTRATE
	    dd4hep::Box sub_box(l_dim_x, module_dz/2, substrate_tck/2);
	    std::string sub_name = dd4hep::xml::_toString(layernum, "sub%d");
	    lLog << MSG::INFO << "SUB BOX LENGTH" << 2*sub_box.x() << endmsg;
	    lLog << MSG::INFO << "This means that the substrate layer exists between " << sub_box.x() << " and " << -sub_box.x() << " in the x direction" << endmsg;
	    dd4hep::Volume sub_vol(sub_name, sub_box, lcdd.material(active_mat));
	    dd4hep::DetElement sub_slice(sub_name, layernum);
	    lLog << MSG::INFO << "Substrate thickness is " << substrate_tck << endmsg;
	    
	    mod_y_off = s_pos_r + (0.5*substrate_tck);
	    m_pos_x = -mod_y_off*std::sin(phi);
	    m_pos_y = mod_y_off*std::cos(phi);

	    dd4hep::Transform3D sub_tr(dd4hep::RotationZYX(0, phi, M_PI*0.5), dd4hep::Translation3D(-m_pos_x, -m_pos_y, module_z));
	    dd4hep::PlacedVolume sub_phv = caloVolume.placeVolume(sub_vol, sub_tr);
	    
	    sub_phv.addPhysVolID("digital", 0);
	    sub_phv.addPhysVolID("layer", layernum);
	    sub_phv.addPhysVolID("stave", stavenum);
	    sub_phv.addPhysVolID("module", modulenum);
	    
	    sub_slice.setPlacement(sub_phv);
	    sub_vol.setSensitiveDetector(sensDet);

	    s_pos_r += substrate_tck + nudge_gap;
	    lLog << MSG::INFO << "Radial position is " << s_pos_r << endmsg;

	    
	    //If padding value negative, air padding needs to go before the passive material
	    if(padding_tck < 0) {
	      s_pos_r -= (padding_tck - nudge_gap);
	    }
	    
	    
	    //PASSIVE
	    std::string pas_name = dd4hep::xml::_toString(layernum, "passive%d");
	    dd4hep::Box pas_box(l_dim_x, module_dz/2, passive_tck/2);
	    lLog << MSG::INFO << "PAS BOX LENGTH" << 2*pas_box.x() << endmsg;
	    lLog << MSG::INFO << "This means that the passive layer exists between " << pas_box.x() << " and " << -pas_box.x() << " in the x direction" << endmsg;
	    dd4hep::Volume pas_vol(pas_name, pas_box, lcdd.material(passive_mat));
	    dd4hep::DetElement pas_slice(pas_name, layernum);
	    lLog << MSG::INFO << "Passive thickness is " << passive_tck << endmsg;

	    mod_y_off = s_pos_r + (0.5*passive_tck);
	    m_pos_x = -mod_y_off*std::sin(phi);
	    m_pos_y = mod_y_off*std::cos(phi);
	    
	    dd4hep::Transform3D pas_tr(dd4hep::RotationZYX(0, phi, M_PI*0.5), dd4hep::Translation3D(-m_pos_x, -m_pos_y, module_z));
	    dd4hep::PlacedVolume pas_phv = caloVolume.placeVolume(pas_vol, pas_tr);
	    
	    pas_slice.setPlacement(pas_phv);
	    s_pos_r += passive_tck + nudge_gap;
	    lLog << MSG::INFO << "Radial position is " << s_pos_r << endmsg;

	    
	    if(!(padding_tck < 0)) {
	      s_pos_r += (padding_tck + nudge_gap);
	    }

	  }
	module_z += module_dz;
      }
      }
  }
  

  //ROBBIE If not circular, need to construct trapezoidal geometry:
  else 
    {
      //ROBBIE Create the radii, half-angle, long half-length, short half-length, half-thickness and thickness of trapezium:
      double hphi = M_PI/nStaves;
      lLog << MSG::DEBUG << "hphi = " << hphi << endmsg;
      
      //ROBBIE Get the dimensions of the cylindrical volume our ECal has to fit inside:
      double rmin = calo_dims.rmin();
      double rmax = calo_dims.rmax() /* std::cos(hphi)*/;
      double dz = calo_dims.dz();
      lLog << MSG::INFO << "rmin = " << rmin << " and rmax = " << rmax << " and dz = " << dz << endmsg;
      
      //ROBBIE we alter rmin and rmax since trapezia have corners, and we need to fit the trapezia into the cylindrical volume.
      double trd_x_short = rmin * std::tan(hphi)/*40.0*/;
      double trd_x_long = rmax * std::sin(hphi)/*50.0*/;
      double trd_y = 0.5*((rmax * std::cos(hphi)) - rmin);
      double trd_z1 = dz/2;
      double trd_z2 = trd_z1;

      lLog << MSG::INFO << "Trapezoid has side lengths " <<  2*trd_x_short << " and " << 2*trd_x_long << " respectively." << endmsg;
      lLog << MSG::INFO << "Trapezoid has thickness " <<  2*trd_y << " and depth " << 2*trd_z1 << " which should be the same as " << 2*trd_z2 << endmsg;
      
      //ROBBIE Create the trapezium volume using the variables above
      dd4hep::Trapezoid trd(trd_x_short,
		    trd_x_long,
		    trd_z1,
		    trd_z2,
		    trd_y);

      //lLog << MSG::INFO << "Trapezoid half-dimensions: X=" << trd.GetDX() << " Y=" << trd.GetDY() << " Z=" << trd.GetDZ() << endmsg;



      //      DetElement stave_det("stave0", xmlDet.id());
      //ROBBIE here we make some parameters that will be useful for when we loop over the layers in each stave.
      //double stave_z = trd_z1/2;
      double l_dim_x = trd_x_long/2;      
      //double adj = (l_dim_x-trd_x_long/2)/2;
      //double hyp = std::sqrt(trd_y*trd_y/4 + adj*adj);
      //double beta = std::acos(adj / hyp);
      //double tan_beta = std::tan(beta);
      //double tan_hphi = std::tan(hphi);
      double l_thickness = active_tck + passive_tck + substrate_tck + padding_tck;
      double xcut = l_thickness*(std::tan(hphi));
      lLog << MSG::INFO << "xcut is " << xcut << " = " << l_thickness << "tan(" << hphi << ")" << endmsg;
      lLog << MSG::INFO << "hphi = " << hphi << endmsg;
      //lLog << MSG::INFO << "beta = " << beta << endmsg;
      

      //ROBBIE If number of layers means they won't all fit in the trapezium, log a message saying how many layers will fit:

      if (2*trd_y < active_samples*module_tck)
	{
	  lLog << MSG::INFO << "Can't fit all " << active_samples << " layers of thickness " << module_tck << " into a trapezoid of thickness " << 2*trd_y << endmsg;
	  active_samples = (2*trd_y)/module_tck;
	  lLog << MSG::INFO << "Filling the trapezoid with " << active_samples << " layers. (max. that will fit)" << endmsg;
	}

      // Calculate angles for nStaves
      double phi = M_PI / nStaves;
      double dphi = 2*phi;
      phi = 0.0;
      double mod_y_off = rmin + trd_y;
      lLog << MSG::INFO << "mod_y_off is " << mod_y_off << endmsg;

      //int stavenum = 0;
      //nStaves = 2;
      //active_samples=10;

      //ROBBIE This value is a small number, used to ensure no overlap between different volume elements:
      double nudge_gap = 0.00001;


      //ROBBIE Here we loop over all the staves in the detector, as defined by the stavenum variable

      for (int stavenum=0; stavenum<nStaves/**2*/; stavenum++)
	{
	  phi -= dphi/*/2*/;
	  double s_pos_z = -trd_y;
	  
	  // recalculate per stave...
	  std::string stave_name = dd4hep::xml::_toString(stavenum, "stave%d");
	  dd4hep::Volume trap_vol(stave_name, trd, lcdd.air());
	  dd4hep::DetElement stave_det(stave_name, stavenum);
	  l_dim_x = trd_x_long/2;
	  lLog << MSG::INFO << "l_dim_x is starting at" << l_dim_x << " which should be similar to either " << rmin * std::tan(hphi) << " or " << rmax * std::tan(hphi) << endmsg;
	  //ROBBIE Ideally we would like to be able to add the stave number volume ID directly to the stave. For now, it's added to each slice in turn, inside the loop below.

	  // stave_det.addPhysVolID("stave", stavenum);
	  // int l_num = 0;


	  //ROBBIE Now we loop over every layer in the stave, adding the slices below:

      for (int layernum=0; layernum  < active_samples; ++layernum)
	{
	  //std::string l_name = _toString(layernum, "layer%d");
	  //lLog << MSG::INFO << "l_name is " << l_name << endmsg;
	  l_dim_x -= xcut/2;
	  //Position l_pos(0, 0, l_pos_z + l_thickness/2);
	  //Box l_box(l_dim_x*2, stave_z*2, l_thickness);
	  //Volume l_vol(l_name, l_box, lcdd.air());
	  //DetElement layer(stave_det, l_name, layernum);
	  

	  //ROBBIE 1) Place the epitaxial slice

       	  //double s_pos_z = -(l_thickness/2);
	  std::string epi_name = dd4hep::xml::_toString(layernum, "epi%d");
	  dd4hep::Box epi_box(l_dim_x*2, trd_z1, active_tck/2);
	  lLog << MSG::INFO << "EPI BOX LENGTH" << 2*epi_box.x() << endmsg;
	  lLog << MSG::INFO << "This means that the epitaxial layer exists between " << epi_box.x() << " and " << -epi_box.x() << " in the x direction" << endmsg;
	  dd4hep::Volume epi_vol(epi_name, epi_box, lcdd.material(active_mat));
	  dd4hep::DetElement epi_slice(stave_det, epi_name, layernum);
	  //lLog << MSG::INFO << "epi_name is " << epi_name << endmsg;
	  lLog << MSG::INFO << "Epitaxial thickness is " << active_tck << endmsg;
	  dd4hep::PlacedVolume epi_phv = trap_vol.placeVolume(epi_vol, dd4hep::Position(0, 0, /*l_pos_z + l_thickness/2 +*/ s_pos_z + active_tck/2));
	  lLog << MSG::INFO << "Placing Epitaxial slice at 0, 0, " << /*l_pos_z + l_thickness/2 +*/ s_pos_z + active_tck/2 << endmsg;
	  lLog << MSG::INFO << "This EPI exists between " << /*l_pos_z + l_thickness/2 +*/ s_pos_z << " and " << /*l_pos_z + l_thickness/2 +*/ s_pos_z + active_tck << " in the z direction." << endmsg;
	  epi_phv.addPhysVolID("digital", 1);
	  epi_phv.addPhysVolID("layer", layernum);
	  epi_phv.addPhysVolID("stave", stavenum + 1);
	  //lLog << MSG::INFO << "EPI information dump : " << epi_phv.toString() << endmsg;
	  //lLog << MSG::INFO << "Stave = " << stavenum << endmsg;
	  epi_slice.setPlacement(epi_phv);
	  epi_vol.setSensitiveDetector(sensDet);
	  
	  //ROBBIE Here we add a tiny gap between each slice/layer. This is to ensure that our volumes don't overlap and cause us to miss hits. 
	  //ROBBIE We may be able to find a more elegant solution to this problem later.
	  s_pos_z += active_tck + nudge_gap;
	  //l_pos_z += active_tck + nudge_gap;
	  
	  	  
	  //ROBBIE 2) Place the substrate

	  std::string sub_name = dd4hep::xml::_toString(layernum, "sub%d");
	  dd4hep::Box sub_box(l_dim_x*2, trd_z1, substrate_tck/2);
	  dd4hep::Volume sub_vol(sub_name, sub_box, lcdd.material(active_mat));
	  dd4hep::DetElement sub_slice(stave_det, sub_name, layernum);
	  //lLog << MSG::INFO << "sub_name is " << sub_name << endmsg;
	  lLog << MSG::INFO << "Substrate thickness is " << substrate_tck << endmsg;
	  dd4hep::PlacedVolume sub_phv = trap_vol.placeVolume(sub_vol, dd4hep::Position(0, 0, /*l_pos_z + l_thickness/2 +*/ s_pos_z + substrate_tck/2));
	  lLog << MSG::INFO << "Placing Substrate slice at 0, 0, " << /*l_pos_z + l_thickness/2 +*/ s_pos_z + substrate_tck/2 << endmsg;
	  lLog << MSG::INFO << "This SUB exists between " << /*l_pos_z + l_thickness/2 +*/ s_pos_z << " and " << /*l_pos_z + l_thickness/2 +*/ s_pos_z + substrate_tck << " in the z direction." << endmsg;
	  sub_phv.addPhysVolID("digital", 0);
	  sub_phv.addPhysVolID("layer", layernum);
	  sub_phv.addPhysVolID("stave", stavenum + 1);
	  //lLog << MSG::INFO << "SUB information dump : " << sub_phv.toString() << endmsg;
	  sub_slice.setPlacement(sub_phv);
	  sub_vol.setSensitiveDetector(sensDet);
	  s_pos_z += substrate_tck + nudge_gap;
	  //l_pos_z += substrate_tck + nudge_gap;
	  

	  //ROBBIE 3) Place the passive layer

	  // if padding_tck < 0 then the air gap goes infront of the passive material
	  if(padding_tck < 0) {
	    s_pos_z -= padding_tck;
	    }
	  
	  std::string pas_name = dd4hep::xml::_toString(layernum, "passive%d");
	  dd4hep::Box pas_box(l_dim_x*2, trd_z1, passive_tck/2);
	  dd4hep::Volume pas_vol(pas_name, pas_box, lcdd.material(passive_mat));
	  dd4hep::DetElement pas_slice(stave_det, pas_name, layernum);
	  lLog << MSG::INFO << "Passive thickness is " << passive_tck << endmsg;
	  dd4hep::PlacedVolume pas_phv = trap_vol.placeVolume(pas_vol, dd4hep::Position(0, 0, s_pos_z + passive_tck/2));
	  lLog << MSG::INFO << "Placing Passive slice at 0, 0, " << /*l_pos_z + l_thickness/2 +*/ s_pos_z + passive_tck/2 << endmsg;
	  lLog << MSG::INFO << "This PAS exists between " << /*l_pos_z + l_thickness/2 +*/ s_pos_z << " and " << /*l_pos_z + l_thickness/2 +*/ s_pos_z + passive_tck << " in the z direction." << endmsg;
	  pas_slice.setPlacement(pas_phv);
	  s_pos_z += passive_tck + nudge_gap;
	  //l_pos_z += active_tck + substrate_tck + passive_tck + (3*nudge_gap);
	  	  

	  //ROBBIE 4) Remainder of layer is already filled with air, so we can simply skip this bit

	  // if padding_tck >= 0 then need to add in the thickness of the air gap:
	  if(!(padding_tck < 0))
	    {
	      s_pos_z -= padding_tck;
		} 
	  
	  /*PlacedVolume layer_phv = trap_vol.placeVolume(l_vol, l_pos);
	  lLog << MSG::INFO << "Placing Layer at " << "(0, 0, " << l_pos_z + l_thickness/2 << ")" << endmsg;
	  lLog << MSG::INFO << "Layer = " << layernum << endmsg;
	  if (layernum == 30) {
	    lLog << MSG::INFO << "PROBLEM! layernum IS TOO DAMN HIGH! IT SHOULD NEVER REACH " << layernum << "!" << endmsg;
	    }*/
	  //lLog << MSG::INFO << "LAYER information dump : " << layer_phv.toString() << endmsg;
	  //layer_phv.addPhysVolID("layer", layernum);
	  //layer.setPlacement(layer_phv);
	  //	  l_pos_z += l_thickness;
	  // ++l_num;
	  
	}


      //ROBBIE Create an overall volume by repeating the trapezium nStaves times

      // double phi = M_PI / nStaves;
      // double dphi = 2*phi;
      // double mod_y_off = rmin + trd_y;
      // lLog << MSG::INFO << "mod_y_off is " << mod_y_off << endmsg;

      
      // for (int i=0; i<nStaves; i++, phi -= dphi)
      // 	{


	  //ROBBIE Compute the stave position
      
	  double m_pos_x = -mod_y_off*std::sin(phi);
	  double m_pos_y = mod_y_off*std::cos(phi);
	  lLog << MSG::INFO << "Position of stave:" << endmsg;
	  lLog << MSG::INFO << "mod_y_off is rmin + half-thickness = " << rmin << " + " << trd_y << " = " << rmin + trd_y << " = " << mod_y_off << endmsg;
	  
	  lLog << MSG::INFO << "m_pos_x is -mod_y_off*sin(phi) = -" << mod_y_off << "sin(" << phi << ") = " << m_pos_x << endmsg;
	  lLog << MSG::INFO << "m_pos_y is mod_y_off*cos(phi) = " << mod_y_off << "cos(" << phi << ") = " << m_pos_y << endmsg;
	  dd4hep::Transform3D tr(dd4hep::RotationZYX(0, /*3.0*M_PI + */phi, M_PI*0.5), dd4hep::Translation3D(-m_pos_x, -m_pos_y, 0));
	  lLog << MSG::INFO << "Placing stave with rotation " << phi << " and position " << -m_pos_x << ", " << -m_pos_y << ", 0" << endmsg;
	  
	  //Place the staves:
	  dd4hep::PlacedVolume pv = caloVolume.placeVolume(trap_vol, tr);
	  //pv.addPhysVolID("stave", stavenum);
	  double not_phi = (M_PI/2) - phi;
	  double not_rmax = mod_y_off + (2*trd_y);
	  double coord_x_a = -(rmin*std::sin(phi)) + (trd_x_long*std::sin(not_phi));
	  double coord_y_a = (rmin*std::cos(phi)) + (trd_x_long*std::cos(not_phi));
	  double coord_x_b = -(rmin*std::sin(phi)) - (trd_x_long*std::sin(not_phi));
	  double coord_y_b = (rmin*std::cos(phi)) - (trd_x_long*std::cos(not_phi));
	  double coord_x_c = -(not_rmax*std::sin(phi)) + (trd_x_short*std::sin(not_phi));
	  double coord_y_c = (not_rmax*std::cos(phi)) + (trd_x_short*std::cos(not_phi));
	  double coord_x_d = -(not_rmax*std::sin(phi)) - (trd_x_short*std::sin(not_phi));
	  double coord_y_d = (not_rmax*std::cos(phi)) - (trd_x_short*std::cos(not_phi));
	  lLog << MSG::INFO << "STAVE information dump : " << pv.toString() << endmsg;
	  lLog << MSG::INFO << "STAVE coordinates dump A: (" << -coord_x_a << ", " << -coord_y_a << ")" << endmsg;
	  lLog << MSG::INFO << "STAVE coordinates dump B: (" << -coord_x_b << ", " << -coord_y_b << ")" << endmsg;
	  lLog << MSG::INFO << "STAVE coordinates dump C: (" << -coord_x_c << ", " << -coord_y_c << ")" << endmsg;
	  lLog << MSG::INFO << "STAVE coordinates dump D: (" << -coord_x_d << ", " << -coord_y_d << ")" << endmsg;
	  stave_det.setPlacement(pv);
	}
    }
  

  
  //Place envelope (or barrel) volume
  dd4hep::Volume motherVol = lcdd.pickMotherVolume(eCal);
  dd4hep::PlacedVolume placedECal = motherVol.placeVolume(envelopeVolume);
  placedECal.addPhysVolID("system", eCal.id());
  eCal.setPlacement(placedECal);
  return eCal;

}
} // namespace det

DECLARE_DETELEMENT(EmCaloDigiBarrel, det::createECal)

