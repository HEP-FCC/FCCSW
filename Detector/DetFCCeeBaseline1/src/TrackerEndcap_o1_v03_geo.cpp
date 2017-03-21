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
//====================================================================

#include "DD4hep/DetFactoryHelper.h"
#include "XML/Utilities.h"
#include <map>
#include "DDRec/DetectorData.h"


using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_detector(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {
    typedef vector<PlacedVolume> Placements;
    xml_det_t   x_det     = e;
    Material    vacuum    = lcdd.vacuum();
    int         det_id    = x_det.id();
    string      det_name  = x_det.nameStr();
    bool        reflect   = x_det.reflect(false);
    DetElement  sdet        (det_name,det_id);
    Volume      motherVol = lcdd.pickMotherVolume(sdet);
    int         m_id=0, c_id=0, n_sensor=0;
    map<string,Volume> modules;
    map<string, Placements>  sensitives;
    PlacedVolume pv;
    
    // --- create an envelope volume and position it into the world ---------------------
    
    Volume envelope = XML::createPlacedEnvelope( lcdd,  e , sdet ) ;
    XML::setDetectorTypeFlag( e, sdet ) ;
    
    if( lcdd.buildType() == BUILD_ENVELOPE ) return sdet ;
    
    //-----------------------------------------------------------------------------------
    
    DDRec::ZDiskPetalsData*  zDiskPetalsData = new DDRec::ZDiskPetalsData ;
    
    
    envelope.setVisAttributes(lcdd.invisible());
    sens.setType("tracker");
    
    for(xml_coll_t mi(x_det,_U(module)); mi; ++mi, ++m_id)  {
        xml_comp_t x_mod   = mi;
        string     m_nam   = x_mod.nameStr();
        xml_comp_t trd     = x_mod.trd();
        double     posY;
        double     x1      = trd.x1();
        double     x2      = trd.x2();
        double     z       = trd.z();
        double     y1, y2, total_thickness=0.;
        xml_coll_t ci(x_mod,_U(module_component));
        for(ci.reset(), total_thickness=0.0; ci; ++ci)
            total_thickness += xml_comp_t(ci).thickness();
        
        y1 = y2 = total_thickness / 2;
        Volume  m_volume(m_nam, Trapezoid(x1, x2, y1, y2, z), vacuum);
        m_volume.setVisAttributes(lcdd.visAttributes(x_mod.visStr()));
        
        
        //Iterate through slices. The first slice (top in the xml) is placed at the "bottom" of the module
        //So when rotated in the final positions the first slice is closest to IP
        for(ci.reset(), n_sensor=1, c_id=0, posY=-y1; ci; ++ci, ++c_id)  {
            xml_comp_t c       = ci;
            double     c_thick = c.thickness();
            Material   c_mat   = lcdd.material(c.materialStr());
            string     c_name  = _toString(c_id,"component%d");
            Volume     c_vol(c_name, Trapezoid(x1,x2,c_thick/2e0,c_thick/2e0,z), c_mat);
            
            c_vol.setVisAttributes(lcdd.visAttributes(c.visStr()));
            pv = m_volume.placeVolume(c_vol,Position(0,posY+c_thick/2,0));
            if ( c.isSensitive() ) {
//                 sdet.check(n_sensor > 1,"TrackerEndcap:fromCompact: "+c_name+" Max of 1 sensitive elemets allowed!");
//                 pv.addPhysVolID("sensor",n_sensor); //Not what we call sensor; see below
                c_vol.setSensitiveDetector(sens);
                sensitives[m_nam].push_back(pv);
                ++n_sensor;
            }
            posY += c_thick;
        }
        modules[m_nam] = m_volume;
    }
    
    for(xml_coll_t li(x_det,_U(layer)); li; ++li)  {
        xml_comp_t  x_layer(li);
        int l_id    = x_layer.id();
        int mod_num = 0;
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
            
            DDRec::ZDiskPetalsData::LayerLayout thisLayer ;
            
            ///FIXME! ALL THIS NEEDS TO BE CHECKED and actually are not needed for CED
            thisLayer.typeFlags[ DDRec::ZDiskPetalsData::SensorType::DoubleSided ] = false ; ///CHECK FIXME
            thisLayer.typeFlags[ DDRec::ZDiskPetalsData::SensorType::Pixel ]     = false ;  ///CHECK
            
            DD4hep::Geometry::Trapezoid mod_shape(m_vol.solid());
            
            
            thisLayer.petalHalfAngle   = 0;
            thisLayer.alphaPetal    = 0. ;  // petals are othogonal to z-axis
            thisLayer.zPosition     = zstart;
            thisLayer.petalNumber   = nmodules ;
            thisLayer.sensorsPerPetal   = 0 ; 
            thisLayer.phi0      = phi  ;
            thisLayer.zOffsetSupport    = 0;
            thisLayer.distanceSupport   = r - mod_shape->GetDZ() ;
            thisLayer.thicknessSupport    = mod_shape->GetDY() ; //FIXME
            thisLayer.widthInnerSupport   = 0;
            thisLayer.widthOuterSupport   = 0;
            thisLayer.lengthSupport   = 0 ;
            thisLayer.zOffsetSensitive    = 0 ;
            thisLayer.distanceSensitive   = r - mod_shape->GetDZ() ; 
            thisLayer.thicknessSensitive  = 0;
            thisLayer.widthInnerSensitive =  0 ;
            thisLayer.widthOuterSensitive = 0 ;
            thisLayer.lengthSensitive   = 2*mod_shape->GetDZ();
            
            zDiskPetalsData->layers.push_back( thisLayer ) ;
            
            
            //NOTE: As in the barrel, what we call "module" in the xml is like a single trapezoidal wafer
            //For reasons of bit conservation in the encoding and to be more similar to the ILD geometry
            //A module has to be something that consists of many "sensors" (wafers) 
            //We chose to do it so one phi-ring is one module. i.e. Modules have constant R
            
            for(int k=0; k<nmodules; ++k) {
                string m_base = _toString(l_id,"layer%d") + _toString(mod_num,"_module%d") + _toString(k,"_sensor%d");
                
                double x = -r*std::cos(phi);
                double y = -r*std::sin(phi);
                DetElement module(sdet,m_base+"_pos",det_id);
                pv = envelope.placeVolume(m_vol,Transform3D(RotationZYX(M_PI,-M_PI/2-phi,-M_PI/2),Position(x,y,zstart+dz)));
                pv.addPhysVolID("barrel",1).addPhysVolID("side",0).addPhysVolID("layer", l_id).addPhysVolID("module",mod_num).addPhysVolID("sensor",k);
                module.setPlacement(pv);
                for(size_t ic=0; ic<sensVols.size(); ++ic)  {
                    PlacedVolume sens_pv = sensVols[ic];
                    DetElement comp_elt(module,sens_pv.volume().name(),mod_num);
                    comp_elt.setPlacement(sens_pv);
                }
                
                if ( reflect ) {
                    pv = envelope.placeVolume(m_vol,Transform3D(RotationZYX(0,-M_PI/2-phi,-M_PI/2),Position(x,y,-zstart-dz)));
                    pv.addPhysVolID("barrel",2).addPhysVolID("side",1).addPhysVolID("layer",l_id).addPhysVolID("module",mod_num).addPhysVolID("sensor",k);
                    DetElement r_module(sdet,m_base+"_neg",det_id);
                    r_module.setPlacement(pv);
                    for(size_t ic=0; ic<sensVols.size(); ++ic)  {
                        PlacedVolume sens_pv = sensVols[ic];
                        DetElement comp_elt(r_module,sens_pv.volume().name(),mod_num);
                        comp_elt.setPlacement(sens_pv);
                    }
                }
                dz   = -dz;
                phi += iphi;
            }
            
            //Increment module numner, i.e. ring
            ++mod_num;
        }
    }

    //FIXME: we don't need these for the event display
    zDiskPetalsData->widthStrip  = 0;
    zDiskPetalsData->lengthStrip =0 ;
    zDiskPetalsData->pitchStrip  = 0 ;
    zDiskPetalsData->angleStrip  = 0;
    
    
    
    sdet.addExtension< DDRec::ZDiskPetalsData >( zDiskPetalsData ) ;
    
    
    return sdet;
}

DECLARE_DETELEMENT(TrackerEndcap_o1_v03,create_detector)
