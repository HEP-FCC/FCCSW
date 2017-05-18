// Silicon Tracker Barrel implementation for the CLIC detector
//====================================================================
//--------------------------------------------------------------------
//
//  Author     : N. Nikiforou (forked from SiTrackerBarrel_geo.cpp
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"
#include "XML/Utilities.h"
#include "DDRec/DetectorData.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_detector(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
    typedef vector<PlacedVolume> Placements;
    xml_det_t   x_det     = e;
    Material    air       = lcdd.air();
    int         det_id    = x_det.id();
    string      det_name  = x_det.nameStr();
    DetElement  sdet       (det_name,det_id);
    // Assembly    assembly   (det_name);
    map<string, Volume>    volumes;
    map<string, Placements>  sensitives;
    PlacedVolume pv;
    
    
    // --- create an envelope volume and position it into the world ---------------------
    
    Volume envelope = XML::createPlacedEnvelope( lcdd,  e , sdet ) ;
    XML::setDetectorTypeFlag( e, sdet ) ;
    
    if( lcdd.buildType() == BUILD_ENVELOPE ) return sdet ;
    
    //-----------------------------------------------------------------------------------
    DDRec::ZPlanarData*  zPlanarData = new DDRec::ZPlanarData() ;
    
    sens.setType("tracker");
    
    //NOTE modules are what is defined in compact. Later we call a "module" as a "sensor".
    for(xml_coll_t mi(x_det,_U(module)); mi; ++mi)  {
        xml_comp_t x_mod  = mi;
        xml_comp_t m_env  = x_mod.child(_U(module_envelope));
        string     m_nam  = x_mod.nameStr();

        
        if ( volumes.find(m_nam) != volumes.end() )   {
            printout(ERROR,"TrackerBarrel","Logics error in building modules.");
            throw runtime_error("Logic error in building modules.");
        }

        double module_thickness = 0;
        for(xml_coll_t ci(x_mod,_U(module_component)); ci; ++ci)  {
            xml_comp_t x_comp = ci;
            module_thickness=module_thickness+x_comp.thickness();
        }

        Volume     m_vol(m_nam,Box(m_env.width()/2.,m_env.length()/2.,module_thickness/2.),air);
        volumes[m_nam] = m_vol;
        m_vol.setVisAttributes(lcdd.visAttributes(x_mod.visStr()));
        
        
        int        ncomponents = 0; 
        
        //First component on top of the list is the innermost one. 
        double position_z= -module_thickness/2.;
        for(xml_coll_t ci(x_mod,_U(module_component)); ci; ++ci, ++ncomponents)  {
            xml_comp_t x_comp = ci;
            string     c_nam  = _toString(ncomponents,"component%d");
            Box        c_box(x_comp.width()/2.0,x_comp.length()/2.0,x_comp.thickness()/2.0);
            Volume     c_vol(c_nam,c_box,lcdd.material(x_comp.materialStr()));
            

            pv = m_vol.placeVolume(c_vol,Position(0,0,position_z+x_comp.thickness()/2.0));

            c_vol.setRegion(lcdd, x_comp.regionStr());
            c_vol.setLimitSet(lcdd, x_comp.limitsStr());
            c_vol.setVisAttributes(lcdd, x_comp.visStr());
            if ( x_comp.isSensitive() ) {
                //         pv.addPhysVolID("wafer",wafer_number++);
                c_vol.setSensitiveDetector(sens);
                sensitives[m_nam].push_back(pv);
            }
            
            position_z += x_comp.thickness();
        }
    }
    for(xml_coll_t li(x_det,_U(layer)); li; ++li)  {
        xml_comp_t x_layer  = li;
        xml_comp_t x_layout = x_layer.child(_U(rphi_layout));
        xml_comp_t z_layout = x_layer.child(_U(z_layout));      // Get the <z_layout> element.
        int        lay_id   = x_layer.id();
        string     m_nam    = x_layer.moduleStr();
        string     lay_nam  = _toString(x_layer.id(),"layer%d");
        Assembly   lay_vol   (lay_nam);         // Create the layer envelope volume.
        double     phi0     = x_layout.phi0();              // Starting phi of first sensor.
        double     phi_tilt = x_layout.phi_tilt();          // Phi tilt of a sensor.
        double     rc       = x_layout.rc();                // Radius of the sensor center.
        int        nphi     = x_layout.nphi();              // Number of sensors in phi.
        double     rphi_dr  = x_layout.dr();                // The delta radius of every other sensor.
        
        double     phi_incr = (M_PI * 2) / nphi;            // Phi increment for one sensor.
        double     phic     = phi0;                         // Phi of the sensor center.
        double     z0       = z_layout.z0();                // Z position of first sensor in phi.
        double     nz       = z_layout.nz();                // Number of sensors to place in z.
        double     z_dr     = z_layout.dr();                // Radial displacement parameter, of every other sensor.
        Volume     m_env    = volumes[m_nam];
        DetElement lay_elt(sdet,_toString(x_layer.id(),"layer%d"),lay_id);
        Placements& waferVols = sensitives[m_nam];
        
        // Z increment for sensor placement along Z axis.
        // Adjust for z0 at center of sensor rather than
        // the end of cylindrical envelope.
        double z_incr   = nz > 1 ? (2.0 * z0) / (nz - 1) : 0.0;
        // Starting z for sensor placement along Z axis.
        double sensor_z = -z0;
        int module_idx =0;
        
        
        DDRec::ZPlanarData::LayerLayout thisLayer ;
        
       
        // Loop over the number of sensors in phi.
        for (int ii = 0; ii < nphi; ii++)        {
            double dx = z_dr * std::cos(phic + phi_tilt);        // Delta x of sensor position.
            double dy = z_dr * std::sin(phic + phi_tilt);        // Delta y of sensor position.
            double  x = rc * std::cos(phic);                     // Basic x sensor position.
            double  y = rc * std::sin(phic);                     // Basic y sensor position.
            
            // Loop over the number of sensors in z.
            //Create stave FIXME disable for now
            string module_name = _toString(module_idx,"module%d");
            //       DetElement module_elt(lay_elt,module_name,module_idx);
            int sensor_idx = 0;
            
            for (int j = 0; j < nz; j++)          {
                string sensor_name = _toString(sensor_idx,"sensor%d");
                
                //FIXME
                sensor_name = module_name + sensor_name;
                
                DetElement sens_elt(lay_elt,sensor_name,sensor_idx);
                // Module PhysicalVolume.
                Transform3D tr(RotationZYX(0,((M_PI/2)-phic-phi_tilt),-M_PI/2),Position(x,y,sensor_z));
                
                //FIXME
                pv = lay_vol.placeVolume(m_env,tr);
                pv.addPhysVolID(_U(module), module_idx);
                pv.addPhysVolID(_U(sensor), sensor_idx);
                sens_elt.setPlacement(pv);
                for(size_t ic=0; ic<waferVols.size(); ++ic)  {
//                     std::cout<<"Layer: "<<lay_id<<" phiIdx: "<<ii<<" zidx: "<<j<<" wafer idx: "<<ic<<std::endl;
                    PlacedVolume wafer_pv = waferVols[ic];
                    DetElement comp_elt(sens_elt,wafer_pv.volume().name(),sensor_idx);
                    comp_elt.setPlacement(wafer_pv);
                    
                    ///GET GEAR INFORMATION FROM FIRST "MODULE" IN Z AND phi
                    ///NOTE WORKS ONLY FOR ONE WAFER
                    if (ii==0 && j==0 && ic==0){
                      
                      DD4hep::Geometry::Box mod_shape(m_env.solid()), comp_shape(wafer_pv.volume().solid());
                      
                      const double* trans = comp_elt.placement()->GetMatrix()->GetTranslation();
                      double half_module_thickness = mod_shape->GetDZ();
                      double half_silicon_thickness = comp_shape->GetDZ();
                      
                      double sensitive_z_position  = trans[2];
                      
                      double inner_thickness = half_module_thickness - sensitive_z_position;
                      
                      thisLayer.distanceSupport  = rc  ;
                     
                      thisLayer.offsetSupport    =  0; 
                      thisLayer.thicknessSupport = inner_thickness- half_silicon_thickness;
                      thisLayer.zHalfSupport    = z0 + mod_shape->GetDY();
                      thisLayer.widthSupport     = 2*mod_shape->GetDX(); 
                      
                      thisLayer.distanceSensitive = rc+sensitive_z_position; 
                      thisLayer.offsetSensitive  = 0. ;
                      thisLayer.thicknessSensitive = 2*half_silicon_thickness;//Assembled along Z
                      //Changed by Thorben Quast (same applies to zHalfSupport)
                      //z0 = center of most right sensor, comp_shape-GetDY() = half length of one sensitive are of the module
                      thisLayer.zHalfSensitive    = z0 + comp_shape->GetDY();
                      thisLayer.widthSensitive = 2*comp_shape->GetDX();
                      thisLayer.ladderNumber = (int) nphi  ;
                      thisLayer.phi0 =  phic;
                    }
                    
                }
                

                /// Increase counters etc.
                sensor_idx++;
                // Adjust the x and y coordinates of the sensor.
                x += dx;
                y += dy;
                // Flip sign of x and y adjustments.
                dx *= -1;
                dy *= -1;
                // Add z increment to get next z placement pos.
                sensor_z += z_incr;
            }
            module_idx++;
            phic     += phi_incr;      // Increment the phi placement of sensor.
            rc       += rphi_dr;       // Increment the center radius according to dr parameter.
            rphi_dr  *= -1;            // Flip sign of dr parameter.
            sensor_z  = -z0;           // Reset the Z placement parameter for sensor.
        }
        // Create the PhysicalVolume for the layer.
        pv = envelope.placeVolume(lay_vol); // Place layer in mother
        pv.addPhysVolID("layer", lay_id);       // Set the layer ID.
        lay_elt.setAttributes(lcdd,lay_vol,x_layer.regionStr(),x_layer.limitsStr(),x_layer.visStr());
        lay_elt.setPlacement(pv);
        
        zPlanarData->layers.push_back( thisLayer ) ;
        
    }
    sdet.setAttributes(lcdd,envelope,x_det.regionStr(),x_det.limitsStr(),x_det.visStr());
    sdet.addExtension< DDRec::ZPlanarData >( zPlanarData ) ;
    
    //envelope.setVisAttributes(lcdd.invisible());
    /*pv = lcdd.pickMotherVolume(sdet).placeVolume(assembly);
     pv.addPhysVolID("system", det_id);      // Set the subdetector system ID.
     pv.addPhysVolID("barrel", 0);           // Flag this as a barrel subdetector.
     sdet.setPlacement(pv);*/
    return sdet;
}

DECLARE_DETELEMENT(TrackerBarrel_o1_v03,create_detector)
