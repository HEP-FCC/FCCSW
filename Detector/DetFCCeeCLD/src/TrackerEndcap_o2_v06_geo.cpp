//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : N. Nikiforou, modified from SiTrackerEndcap by M.Frank
//
//  Comments: 
//  Originally Forked from SiTrackerEndcap2 by M. Frank
//  N. Nikiforou: This version is equivalent to VertexEndcap_o1_v03
//  and is compatible with the convention that modules are built so
//  the top-most slice in the xml is the inner-most slice (closest to IP)
//  when placed. This convention allows for the use of the TrackerEndcap_surfaces
//  plugin with n defined as (0,1,0), i.e parallel to the y axis when stacking slices
//  
//  Note: v04 differs from v03 only in the ZDiskPetalsData by putting all rings in 
//  one layer which is meaningful for the reconstruction. 
//  The other way was only to accommodate "dish" shaped layers
//  for the CED event display. In V03, more "layers" are inserted in the structure 
//  at different z positions. The caveat in this version is that we create one
//  large "petal" at an average z position, without gaps along r.
//
// Comment: You have to use <include ref=... to define a module stack
//
//====================================================================

#include "DD4hep/DetFactoryHelper.h"
#include "XML/Utilities.h"
#include "XML/DocumentHandler.h"
#include <map>
#include "DDRec/DetectorData.h"

/*
#include <UTIL/BitField64.h>
#include <UTIL/BitSet32.h>
#include "UTIL/LCTrackerConf.h"
#include <UTIL/ILDConf.h>

*/

using namespace std;

using dd4hep::Assembly;
using dd4hep::BUILD_ENVELOPE;
using dd4hep::Box;
using dd4hep::DetElement;
using dd4hep::Detector;
using dd4hep::Material;
using dd4hep::PlacedVolume;
using dd4hep::Position;
using dd4hep::Ref_t;
using dd4hep::RotationX;
using dd4hep::RotationZ;
using dd4hep::RotationZYX;
using dd4hep::SensitiveDetector;
using dd4hep::Transform3D;
using dd4hep::Trapezoid;
using dd4hep::Tube;
using dd4hep::Volume;
using dd4hep::_toString;
using dd4hep::rec::ZDiskPetalsData;
using dd4hep::rec::NeighbourSurfacesData;

namespace det {

static Ref_t TrackerEndcap_o2_v06_geo(Detector& theDetector, xml_h e, SensitiveDetector sens)  {
    typedef vector<PlacedVolume> Placements;
    xml_det_t   x_det     = e;
    Material    vacuum    = theDetector.vacuum();
    int         det_id    = x_det.id();
    string      det_name  = x_det.nameStr();
    bool        reflect   = x_det.reflect(false);
    DetElement  sdet        (det_name,det_id);
    int         m_id=0, c_id=0, n_sensor=0;
    map<string,Volume> modules;
    map<string, Placements>  sensitives;
    PlacedVolume pv;
    

    // for encoding
       /*
    std::string cellIDEncoding = sens.readout().idSpec().fieldDescription();
    UTIL::BitField64 encoder( cellIDEncoding );
    encoder.reset();
    encoder[lcio::LCTrackerCellID::subdet()] = det_id;
*/



    // --- create an envelope volume and position it into the world ---------------------
    
    Volume envelope = dd4hep::xml::createPlacedEnvelope( theDetector,  e , sdet ) ;
    dd4hep::xml::setDetectorTypeFlag( e, sdet ) ;
    
    if( theDetector.buildType() == BUILD_ENVELOPE ) return sdet ;
    
    //-----------------------------------------------------------------------------------
    
    ZDiskPetalsData*  zDiskPetalsData = new ZDiskPetalsData ;
    NeighbourSurfacesData*  neighbourSurfacesData = new NeighbourSurfacesData() ;
    std::map< std::string, double > moduleSensThickness;
    
    
    envelope.setVisAttributes(theDetector.invisible());
    // sens.setType("tracker");	 // MANU
       sens.setType("SimpleTrackerSD"); 
    
    for(xml_coll_t mi(x_det,_U(module)); mi; ++mi, ++m_id)  {
        xml_comp_t x_mod   = mi;
        string     m_nam   = x_mod.nameStr();
        xml_comp_t trd     = x_mod.trd();
        double     posZ;
        double     dx      = trd.x()/2.;
        double     dy      = trd.y()/2.;
        double     total_thickness=0.;

        for(xml_coll_t incl(x_mod,_U(include)); incl; ++incl)  {
            dd4hep::xml::DocumentHolder doc(dd4hep::xml::DocumentHandler().load(incl, incl.attr_value(_U(ref))));
            xml_h includes = doc.root();
            xml_det_t incl_stack = includes;
            for(xml_coll_t ci(incl_stack,_U(module_component)); ci; ++ci)  {
                total_thickness += xml_comp_t(ci).thickness();
            }
        }

        double dz = total_thickness / 2.;
        Volume  m_volume(m_nam, Box(dx, dy, dz), vacuum);
        m_volume.setVisAttributes(theDetector.visAttributes(x_mod.visStr()));
        
        
        //Iterate through slices. The first slice (top in the xml) is placed at the "bottom" of the module
        //So when rotated in the final positions the first slice is closest to IP
        for(xml_coll_t incl(x_mod,_U(include)); incl; ++incl) {
            dd4hep::xml::DocumentHolder doc(dd4hep::xml::DocumentHandler().load(incl, incl.attr_value(_U(ref))));
            xml_h includes = doc.root();
            xml_det_t incl_stack = includes;
            xml_coll_t ci(incl_stack, _U(module_component));
            for (ci.reset(), n_sensor = 1, c_id = 0, posZ = -dz; ci; ++ci, ++c_id) {
                xml_comp_t c = ci;
                double c_thick = c.thickness();
                Material c_mat = theDetector.material(c.materialStr());
                string c_name = _toString(c_id, "component%d");
                Volume c_vol(c_name, Box(dx, dy, c_thick / 2.0), c_mat);

                c_vol.setVisAttributes(theDetector.visAttributes(c.visStr()));
                pv = m_volume.placeVolume(c_vol, Position(0, 0, posZ + c_thick / 2.0));
                if (c.isSensitive()) {
//                 sdet.check(n_sensor > 1,"TrackerEndcap:fromCompact: "+c_name+" Max of 1 sensitive elemets allowed!");
//                 pv.addPhysVolID("sensor",n_sensor); //Not what we call sensor; see below
                    c_vol.setSensitiveDetector(sens);
                    sensitives[m_nam].push_back(pv);
                    //Assuming one sensitive slice per module
                    moduleSensThickness[m_nam] = c_thick;
                    ++n_sensor;
                }
                posZ += c_thick;
            }
            modules[m_nam] = m_volume;
        }
    }
    
    for(xml_coll_t li(x_det,_U(layer)); li; ++li)  {
        xml_comp_t  x_layer(li);
        int l_id    = x_layer.id();
        int mod_num = 0;
        int ring_no=0;
        //Cannot handle rings in ZDiskPetalsData, calculate approximate petal quantities
        double sumZ(0.), innerR(1e100),outerR(0.);
        double sensitiveThickness(0.0);

	//loop only to count the number of rings in a disk - it is then needed for looking for neighborous when you are in a "border" cell
	int nrings = 0;
	for(xml_coll_t ri(x_layer,_U(ring)); ri; ++ri) { 
	  nrings++;
	}

        for(xml_coll_t ri(x_layer,_U(ring)); ri; ++ri)  {
            xml_comp_t x_ring = ri;
            double r        = x_ring.r();
            double phi0     = x_ring.phi0(0);
            double zstart   = x_ring.zstart();
            double dz       = x_ring.dz(0);
            int    nmodules = x_ring.nmodules();
            string m_nam    = x_ring.moduleStr();
            Volume m_vol    = modules[m_nam];
            double iphi     = 2*M_PI/nmodules;
            double phi      = phi0;
            Placements& sensVols = sensitives[m_nam];
            sensitiveThickness = moduleSensThickness[m_nam];
            
            
            Box mod_shape(m_vol.solid());
            
             if(r - mod_shape->GetDZ()<innerR)
                 innerR= r- mod_shape->GetDZ();
             
             if(r + mod_shape->GetDZ()>outerR)
                 outerR = r + mod_shape->GetDZ();
                 
            sumZ+=zstart;
            
            r=r+mod_shape->GetDY();
            
            //NOTE: As in the barrel, what we call "module" in the xml is like a single trapezoidal wafer
            //For reasons of bit conservation in the encoding and to be more similar to the ILD geometry
            //A module has to be something that consists of many "sensors" (wafers) 
            //We chose to do it so one phi-ring is one module. i.e. Modules have constant R
            
            for(int k=0; k<nmodules; ++k) {
                string m_base = _toString(l_id,"layer%d") + _toString(mod_num,"_module%d") + _toString(k,"_sensor%d");
                
                double x = -r*std::cos(phi);
                double y = -r*std::sin(phi);
                DetElement module(sdet,m_base+"_pos",det_id);
                pv = envelope.placeVolume(m_vol,Transform3D(RotationZ(phi+M_PI/2),Position(x,y,zstart+dz)));
                pv.addPhysVolID("side",1).addPhysVolID("layer", l_id).addPhysVolID("module",mod_num).addPhysVolID("sensor",k);
                module.setPlacement(pv);


                for(size_t ic=0; ic<sensVols.size(); ++ic)  {
                    PlacedVolume sens_pv = sensVols[ic];
                    DetElement comp_elt(module,sens_pv.volume().name(),mod_num);
                    comp_elt.setPlacement(sens_pv);
                }
                
                if ( reflect ) {
                    pv = envelope.placeVolume(m_vol,Transform3D(RotationZ(phi+M_PI/2)*RotationX(M_PI),Position(x,y,-zstart-dz)));
                    pv.addPhysVolID("side",-1).addPhysVolID("layer",l_id).addPhysVolID("module",mod_num).addPhysVolID("sensor",k);
                    DetElement r_module(sdet,m_base+"_neg",det_id);
                    r_module.setPlacement(pv);
                    for(size_t ic=0; ic<sensVols.size(); ++ic)  {
                        PlacedVolume sens_pv = sensVols[ic];
                        DetElement comp_elt(r_module,sens_pv.volume().name(),mod_num);
                        comp_elt.setPlacement(sens_pv);
                    }
                }

		///////////////////

		//get cellID and fill map< cellID of surface, vector of cellID of neighbouring surfaces >

		//encoding

		// dd4hep::long64 cellID_reflect;	// unused
		if (reflect) {
/*
		  encoder[lcio::LCTrackerCellID::side()] = lcio::ILDDetID::bwd;
		  encoder[lcio::LCTrackerCellID::layer()] = l_id;
		  encoder[lcio::LCTrackerCellID::module()] = mod_num;
		  encoder[lcio::LCTrackerCellID::sensor()] = k;

		  cellID_reflect = encoder.lowWord(); // 32 bits
*/
		}

/*
		encoder[lcio::LCTrackerCellID::side()] = lcio::ILDDetID::fwd;
		encoder[lcio::LCTrackerCellID::layer()] = l_id;
		encoder[lcio::LCTrackerCellID::module()] = mod_num;
		encoder[lcio::LCTrackerCellID::sensor()] = k;

		dd4hep::long64 cellID = encoder.lowWord(); // 32 bits
*/

		//compute neighbours 

		int n_neighbours_module = 1; // 1 gives the adjacent modules (i do not think we would like to change this)
		int n_neighbours_sensor = 1;

		int newmodule=0, newsensor=0;

		for(int imodule=-n_neighbours_module; imodule<=n_neighbours_module; imodule++){ // neighbouring modules
		  for(int isensor=-n_neighbours_sensor; isensor<=n_neighbours_sensor; isensor++){ // neighbouring sensors
		    
		    if (imodule==0 && isensor==0) continue; // cellID we started with
		    newmodule = mod_num + imodule;
		    newsensor = k + isensor;
		    
		    //compute special case at the boundary  
		    //general computation to allow (if necessary) more then adiacent neighbours (ie: +-2)
		    if (newsensor < 0) newsensor = nmodules + newsensor;
		    if (newsensor >= nmodules) newsensor = newsensor - nmodules;

		    if (newmodule < 0 || newmodule >= nrings) continue; //out of disk		

		    //encoding
		      /*
		    encoder[lcio::LCTrackerCellID::module()] = newmodule;
		    encoder[lcio::LCTrackerCellID::sensor()] = newsensor;

		    neighbourSurfacesData->sameLayer[cellID].push_back(encoder.lowWord());
*/


		    if (reflect){
/*
		      encoder[lcio::LCTrackerCellID::side()] = lcio::ILDDetID::bwd;
		      encoder[lcio::LCTrackerCellID::layer()] = l_id;
		      encoder[lcio::LCTrackerCellID::module()] = newmodule;
		      encoder[lcio::LCTrackerCellID::sensor()] = newsensor;

		      neighbourSurfacesData->sameLayer[cellID_reflect].push_back(encoder.lowWord());
*/
		    }
 
		  }
		}

		///////////////////


                dz   = -dz;
                phi += iphi;
            }
            
            //Increment module numner, i.e. ring
            ++mod_num;
            ++ring_no;
        }
        
            ZDiskPetalsData::LayerLayout thisLayer ;
            
            ///NOTE: Only filling what needed for CED/DDMarlinPandora
            thisLayer.zPosition= sumZ/ring_no; //calc average z
            thisLayer.distanceSensitive   = innerR ; 
            thisLayer.lengthSensitive   = outerR-innerR;
            thisLayer.petalNumber   = ring_no ; //Store the number of rings in petalNumber needed for tracking

            //this assumes there is a constant sensitive thickness even though
            //the layer could have different modules with different thicknesses
            thisLayer.thicknessSensitive = sensitiveThickness;

            zDiskPetalsData->layers.push_back( thisLayer ) ;
    }

    sdet.addExtension< ZDiskPetalsData >( zDiskPetalsData ) ;
    sdet.addExtension< NeighbourSurfacesData >( neighbourSurfacesData ) ;

    
    return sdet;
}
}

DECLARE_DETELEMENT(TrackerEndcap_o2_v06,det::TrackerEndcap_o2_v06_geo)
