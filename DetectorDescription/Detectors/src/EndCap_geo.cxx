//
//  EndCap_geo.cxx
//
//
//  Created by Julia Hrdinka on 07/01/15.
//
//

#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/TGeoUnits.h"
#include "DetExtensions/DetDiscLayer.h"
#include "DetExtensions/DetModule.h"
#include "DetExtensions/Extension.h"
#include "DetExtensions/DetDiscVolume.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens)
{
    xml_det_t x_det = e;
    string det_name = x_det.nameStr();
    Material air    = lcdd.air();
    
    //Detector envelope of subdetector
    DetElement endcap(det_name, x_det.id());
    //get status for the RecoGeometry
    xml_comp_t x_status = x_det.child(_U(status));
    int status     = x_status.id();
    //add Extension to Detlement for the RecoGeometry
    Det::DetDiscVolume* detvolume = new Det::DetDiscVolume(status);
    endcap.addExtension<Det::IExtension>(detvolume);
    //Volume for the envelope
    DD4hep::XML::Dimension x_det_dim(x_det.dimensions());
    Tube endcap_shape(x_det_dim.rmin(),x_det_dim.rmax(),x_det_dim.dz());
    Volume endcap_vol(det_name+"_envelope",endcap_shape,air);
    double endcap_pos = x_det_dim.z();
    double sign       = 1.;
    if (endcap_pos<0.) sign = -1.;
    //Visualization
    endcap_vol.setVisAttributes(lcdd.invisible());
    //Set sensitive Type
    sens.setType("Geant4Tracker");
    
    int layer_num = 0;
    for (xml_coll_t j(e,_U(layer)); j; j++) {
        
        xml_comp_t x_layer = j;
        double rmin     = x_layer.inner_r();
        double rmax     = x_layer.outer_r();
        double layer_z  = x_layer.dz();
        int repeat      = x_layer.repeat();
        double deltaphi = 2.*M_PI/repeat;
        
        //Layer
        string layer_name = det_name + _toString(layer_num,"layer%d");
        //Create Volume and DetELement for Layer
        Volume layer_vol(layer_name, Tube(rmin,rmax,layer_z), air);
        DetElement detlayer(endcap,layer_name, layer_num);
        double minphi   = 0.;
        double maxphi   = (repeat-1.)*deltaphi;
        
        //Visualization
        layer_vol.setVisAttributes(lcdd.invisible());
        //vector of rValues
        std::vector<std::pair<float,float>> rValues;
        
        
        int module_num_num = 0;
        for (xml_coll_t i(x_layer,_U(module)); i; i++) {
            xml_comp_t x_module = i;
            
            double radius   = x_module.radius();
            rValues.push_back(std::make_pair<float,float>(radius-x_module.length(),radius+x_module.length()));
            double slicedz  = x_module.dz();
            
            
            int module_num = 0;
            
            //Place the modules
            for (int j = 0; j < repeat; j++)
            {
                //Create Module Volume
                Volume mod_vol("module", Box(x_module.length(),x_module.width(),x_module.thickness()), air);
                //Visualization
                mod_vol.setVisAttributes(lcdd.invisible());
                
                double phi = deltaphi/dd4hep::rad * j;
                string module_name = _toString(repeat*module_num_num+module_num,"module%d");
                
                Position trans(radius*cos(phi),
                               radius*sin(phi),
                               slicedz);
                
                //Create Module Detelement
                DetElement mod_det(detlayer,module_name,repeat*module_num_num+module_num);
                //add Extension to Detelement for the RecoGeometry
                Det::DetModule* detmod = new Det::DetModule();
                mod_det.addExtension<Det::IExtension> (detmod);
                
                int comp_num = 0;
                
                for (xml_coll_t n(x_module,_U(module_component)); n; n++) {
                    xml_comp_t x_comp = n;
                    
                    Volume comp_vol(_toString(comp_num, "component% ") + x_comp.materialStr(), Box(x_comp.length(),x_comp.width(),x_comp.thickness()),lcdd.material(x_comp.materialStr()));
                    //Visualization
                    comp_vol.setVisAttributes(lcdd, x_comp.visStr());
                    if(x_comp.isSensitive()) comp_vol.setSensitiveDetector(sens);
                    //Create DetElement
                    DetElement comp_det(mod_det, "component, " + x_comp.materialStr(),comp_num);
                    //add Extension
                    Det::Extension* ex = new Det::Extension();
                    comp_det.addExtension<Det::IExtension>(ex);
                    //place component in module
                    Position trans (0.,0., x_comp.z());
                    PlacedVolume placedcomp = mod_vol.placeVolume(comp_vol,trans);
                    //assign the placed Volume to the DetElement
                    comp_det.setPlacement(placedcomp);
                    placedcomp.addPhysVolID("component",comp_num);
                    ++comp_num;
                }
                
                //Place Module Box Volumes in layer
                PlacedVolume placedmodule = layer_vol.placeVolume(mod_vol, Transform3D(RotationX(M_PI)*RotationZ(M_PI-phi)*RotationX(M_PI),trans));
                placedmodule.addPhysVolID("module", repeat*module_num_num+module_num);
                // assign module DetElement to the placed Module volume
                mod_det.setPlacement(placedmodule);
                ++module_num;
                
            }
            module_num_num++;
        }
        //add Extension to Detlement for the RecoGeometry
        Det::DetDiscLayer* detdisclayer = new Det::DetDiscLayer(rValues.size(), rValues,repeat,minphi,maxphi);
        detlayer.addExtension<Det::IExtension>(detdisclayer);
        //Placed Layer Volume
        Position layer_pos(0.,0.,x_layer.z());
        PlacedVolume placedLayer = endcap_vol.placeVolume(layer_vol, layer_pos);
        placedLayer.addPhysVolID("layer",layer_num);
        detlayer.setPlacement(placedLayer);
        ++layer_num;
    }
    
    //Place envelope Volume
    Volume mother_vol = lcdd.pickMotherVolume(endcap);
    Position endcap_trans(0.,0.,endcap_pos);
    if (sign ==-1) {
        Transform3D endcap_transform(RotationX(M_PI), endcap_trans);
        PlacedVolume placedEndCap = mother_vol.placeVolume(endcap_vol, endcap_transform);
        placedEndCap.addPhysVolID("system",x_det.id());
        //assign tracker DetElement to tracker volume
        endcap.setPlacement(placedEndCap);
    }
    else {
        Transform3D endcap_transform(endcap_trans);
        PlacedVolume placedEndCap = mother_vol.placeVolume(endcap_vol, endcap_transform);
        placedEndCap.addPhysVolID("system",x_det.id());
        //assign tracker DetElement to tracker volume
        endcap.setPlacement(placedEndCap);
    }
    
    //Place LayerVolume in motherVolume
    
    
    
    return endcap;
}

DECLARE_DETELEMENT(EndCap, create_element)
