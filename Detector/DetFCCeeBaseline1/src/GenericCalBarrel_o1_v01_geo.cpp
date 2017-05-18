//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : N. Nikiforou
//               Adapted from PolyhedraBarrelCalorimeter by M. Frank
//
//====================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "XML/Layering.h"
#include "XML/Utilities.h"
#include "DDRec/DetectorData.h"
#include "DDSegmentation/Segmentation.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

// workaround for DD4hep v00-14 (and older) 
#ifndef DD4HEP_VERSION_GE
#define DD4HEP_VERSION_GE(a,b) 0 
#endif

static void placeStaves(DetElement& parent, DetElement& stave, double rmin, int numsides, double total_thickness,
                        Volume envelopeVolume, double innerAngle, Volume sectVolume) {
    double innerRotation = innerAngle;
    double offsetRotation = -innerRotation / 2;
    double sectCenterRadius = rmin + total_thickness / 2;
    double rotX = M_PI / 2;
    double rotY = -offsetRotation;
    double posX = -sectCenterRadius * std::sin(rotY);
    double posY = sectCenterRadius * std::cos(rotY);
    
    for (int module = 0; module < numsides; ++module) {
        DetElement det = module > 0 ? stave.clone(_toString(module,"stave%d")) : stave;
        Transform3D trafo(RotationZYX(0, rotY, rotX), Translation3D(-posX, -posY, 0));
        PlacedVolume pv = envelopeVolume.placeVolume(sectVolume,trafo);
        // Not a valid volID: pv.addPhysVolID("stave", 0);
        pv.addPhysVolID("module", module);
        det.setPlacement(pv);
        parent.add(det);
        rotY -= innerRotation;
        posX = -sectCenterRadius * std::sin(rotY);
        posY = sectCenterRadius * std::cos(rotY);
    }
}

static Ref_t create_detector(LCDD& lcdd, xml_h e, SensitiveDetector sens) {
    xml_det_t x_det = e;
    Layering layering(x_det);
    xml_comp_t staves = x_det.staves();
    xml_dim_t dim = x_det.dimensions();
    string det_name = x_det.nameStr();
    string det_type = x_det.typeStr();
    Material air = lcdd.air();
    double totalThickness = layering.totalThickness();
    int totalRepeat = 0;
    int totalSlices = 0;
    double gap = xml_dim_t(x_det).gap();
    int nsides = dim.numsides();
    
    double detZ = dim.z();
    double rmin = dim.rmin();
    DetElement sdet(det_name, x_det.id());
    DetElement stave("stave0", x_det.id());
    //  Volume motherVol = lcdd.pickMotherVolume(sdet);
    Readout readout = sens.readout();
    Segmentation seg = readout.segmentation();
    
    std::vector<double> cellSizeVector = seg.segmentation()->cellDimensions(0); //Assume uniform cell sizes, provide dummy cellID
    double cell_sizeX      = cellSizeVector[0];
    double cell_sizeY      = cellSizeVector[1];
    
    //Create caloData object to extend driver with data required for reconstruction
    DDRec::LayeredCalorimeterData* caloData = new DDRec::LayeredCalorimeterData ;
    caloData->layoutType = DDRec::LayeredCalorimeterData::BarrelLayout ;
    caloData->inner_symmetry = nsides;
    caloData->outer_symmetry = nsides; 
    
    /** NOTE: phi0=0 means lower face flat parallel to experimental floor
     *  This is achieved by rotating the modules with respect to the envelope
     *  which is assumed to be a Polyhedron and has its axes rotated with respect
     *  to the world by 180/nsides. In any other case (e.g. if you want to have
     *  a tip of the calorimeter touching the ground) this value needs to be computed
     */
    
    caloData->inner_phi0 = 0.; 
    caloData->outer_phi0 = 0.; 
    caloData->gap0 = 0.; //FIXME
    caloData->gap1 = 0.; //FIXME
    caloData->gap2 = 0.; //FIXME  
    
    
    for (xml_coll_t c(x_det, _U(layer)); c; ++c) {
        xml_comp_t x_layer = c;
        int repeat = x_layer.repeat();
        totalRepeat += repeat;
        totalSlices += x_layer.numChildren(_U(slice));
    }
    
    // --- create an envelope volume and position it into the world ---------------------
    
    Volume envelopeVol = XML::createPlacedEnvelope( lcdd,  e , sdet ) ;
    XML::setDetectorTypeFlag( e, sdet ) ;
    
    if( lcdd.buildType() == BUILD_ENVELOPE ) return sdet ;
    
    //-----------------------------------------------------------------------------------
    
    // Add the subdetector envelope to the structure.
    double innerAngle = 2 * M_PI / nsides;
    double halfInnerAngle = innerAngle / 2;
    double tan_inner = std::tan(halfInnerAngle) * 2;
    double innerFaceLen = rmin * tan_inner;
    double outerFaceLen = (rmin + totalThickness) * tan_inner;
    double staveThickness = totalThickness;
    
    /// extent of the calorimeter in the r-z-plane [ rmin, rmax, zmin, zmax ] in mm.
    caloData->extent[0] = rmin ;
    caloData->extent[1] = rmin + totalThickness ;
    caloData->extent[2] = 0. ;
    caloData->extent[3] = detZ/2.0 ;
    
    Trapezoid staveTrdOuter(innerFaceLen / 2, outerFaceLen / 2, detZ / 2, detZ / 2, staveThickness / 2);
    Volume staveOuterVol("stave_outer", staveTrdOuter, air);
    
    Trapezoid staveTrdInner(innerFaceLen / 2 - gap, outerFaceLen / 2 - gap, detZ / 2, detZ / 2, staveThickness / 2);
    Volume staveInnerVol("stave_inner", staveTrdInner, air);
    
    double layerOuterAngle = (M_PI - innerAngle) / 2;
    double layerInnerAngle = (M_PI / 2 - layerOuterAngle);
    double layer_pos_z = -(staveThickness / 2);
    double layer_dim_x = innerFaceLen / 2 - gap * 2;

    int layer_num = 0;
    int layerType   = 0;

    for (xml_coll_t c(x_det, _U(layer)); c; ++c) {
        xml_comp_t x_layer = c;
        int repeat = x_layer.repeat();            // Get number of times to repeat this layer.
        const Layer* lay = layering.layer(layer_num); // Get the layer from the layering engine.
        double layer_thickness = lay->thickness();
        string layer_type_name   = _toString(layerType,"layerType%d");

        DDRec::LayeredCalorimeterData::Layer caloLayer ;
        caloLayer.cellSize0 = cell_sizeX;
        caloLayer.cellSize1 = cell_sizeY;

        // Loop over repeats for this layer.
        for (int j = 0; j < repeat; j++) {
            string layer_name = _toString(layer_num, "layer%d");
            DetElement layer(stave, layer_name, layer_num);

            // Layer box & volume
            Volume layer_vol(layer_type_name, Box(layer_dim_x, detZ / 2, layer_thickness / 2), air);
            
            // Create the slices (sublayers) within the layer.
            double slice_pos_z = -(layer_thickness / 2);
            int slice_number = 0;
            
            double nRadiationLengths=0.;
            double nInteractionLengths=0.;
            double thickness_sum=0;
            
            for (xml_coll_t k(x_layer, _U(slice)); k; ++k) {
                xml_comp_t x_slice = k;
                string slice_name = _toString(slice_number, "slice%d");
                double slice_thickness = x_slice.thickness();
                Material slice_material = lcdd.material(x_slice.materialStr());
                
                slice_pos_z += slice_thickness / 2;
                // Slice volume & box
                Volume slice_vol(slice_name, Box(layer_dim_x, detZ / 2, slice_thickness / 2), slice_material);
                
                nRadiationLengths += slice_thickness/(2.*slice_material.radLength());
                nInteractionLengths += slice_thickness/(2.*slice_material.intLength());
                thickness_sum += slice_thickness/2;
                
                if (x_slice.isSensitive()) {
                    sens.setType("calorimeter");
                    slice_vol.setSensitiveDetector(sens);
                    
#if DD4HEP_VERSION_GE( 0, 15 )
                    //Store "inner" quantities
                    caloLayer.inner_nRadiationLengths = nRadiationLengths;
                    caloLayer.inner_nInteractionLengths = nInteractionLengths;
                    caloLayer.inner_thickness = thickness_sum;
                    //Store scintillator thickness
                    caloLayer.sensitive_thickness = slice_thickness;
#endif
                    //Reset counters to measure "outside" quantitites
                    nRadiationLengths=0.;
                    nInteractionLengths=0.;
                    thickness_sum = 0.;
                } 
                
                nRadiationLengths += slice_thickness/(2.*slice_material.radLength());
                nInteractionLengths += slice_thickness/(2.*slice_material.intLength());
                thickness_sum += slice_thickness/2;
                
                // Set region, limitset, and vis.
                slice_vol.setAttributes(lcdd, x_slice.regionStr(), x_slice.limitsStr(), x_slice.visStr());
                // slice PlacedVolume
                layer_vol.placeVolume(slice_vol, Position(0, 0, slice_pos_z));
                
                // Increment Z position for next slice.
                slice_pos_z += slice_thickness / 2;
                // Increment slice number.
                ++slice_number;
            }
            
#if DD4HEP_VERSION_GE( 0, 15 )
            //Store "outer" quantities
            caloLayer.outer_nRadiationLengths = nRadiationLengths;
            caloLayer.outer_nInteractionLengths = nInteractionLengths;
            caloLayer.outer_thickness = thickness_sum;
#endif        
            
            // Set region, limitset, and vis.
            layer_vol.setAttributes(lcdd, x_layer.regionStr(), x_layer.limitsStr(), x_layer.visStr());


            
            // Layer position in Z within the stave.
            layer_pos_z += layer_thickness / 2;
            // Layer physical volume.
            PlacedVolume layer_phv = staveInnerVol.placeVolume(layer_vol, Position(0, 0, layer_pos_z));
            layer_phv.addPhysVolID("layer", layer_num);
            layer.setPlacement(layer_phv);
            
            //The rest of the data is constant; only the distance needs to be updated  
            //Store the position up to the inner face of the layer
            caloLayer.distance = rmin+layer_pos_z + staveThickness / 2 - layer_thickness / 2; 
            std::cout<<"Layer: "<<layer_num<<" Rmin: "<<rmin<<" layer_pos_z: "<<layer_pos_z<<" Dist: "<<caloLayer.distance
#if DD4HEP_VERSION_GE( 0, 15 )
		     <<" inner_thickness: "<<caloLayer.inner_thickness<<" outer_thickness: "<<caloLayer.outer_thickness
#endif
		     <<std::endl;
            //Push back a copy to the caloData structure
            caloData->layers.push_back( caloLayer ) ;
            
            // Increment the layer X dimension.
            layer_dim_x += layer_thickness * std::tan(layerInnerAngle);
            // Increment the layer Z position.
            layer_pos_z += layer_thickness / 2;
            // Increment the layer number.
            ++layer_num;
        }
        
        ++layerType;
    }
    
    // Add stave inner physical volume to outer stave volume.
    staveOuterVol.placeVolume(staveInnerVol);
    if ( staves )  {
        // Set the vis attributes of the outer stave section.
        stave.setVisAttributes(lcdd, staves.visStr(), staveInnerVol);
        stave.setVisAttributes(lcdd, staves.visStr(), staveOuterVol);
    }
    // Place the staves.
    placeStaves(sdet, stave, rmin, nsides, totalThickness, envelopeVol, innerAngle, staveOuterVol);
    // Set envelope volume attributes.
    envelopeVol.setAttributes(lcdd, x_det.regionStr(), x_det.limitsStr(), x_det.visStr());
    
    
    //FOR NOW, USE A MORE "SIMPLE" VERSION OF EXTENSIONS, INCLUDING NECESSARY GEAR PARAMETERS
    //Copied from Frank's SHcalSc04 Implementation
    sdet.addExtension< DDRec::LayeredCalorimeterData >( caloData ) ;
    
    return sdet;
}

DECLARE_DETELEMENT(GenericCalBarrel_o1_v01, create_detector)
