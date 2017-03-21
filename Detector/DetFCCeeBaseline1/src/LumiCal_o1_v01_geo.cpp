#include <DD4hep/DetFactoryHelper.h>
#include "DD4hep/DetType.h"
#include <XML/Layering.h>
#include "XML/Utilities.h"
#include "DDRec/DetectorData.h"

#include <string>

using namespace DD4hep;
using namespace DD4hep::Geometry;

// workaround for DD4hep v00-14 (and older) 
#ifndef DD4HEP_VERSION_GE
#define DD4HEP_VERSION_GE(a,b) 0 
#endif

static DD4hep::Geometry::Ref_t create_detector(DD4hep::Geometry::LCDD& lcdd,
                                               xml_h element,
                                               DD4hep::Geometry::SensitiveDetector sens) {
    
    std::cout << __PRETTY_FUNCTION__  << std::endl;
    std::cout << "Here is my LumiCal"  << std::endl;
    std::cout << " and this is the sensitive detector: " << &sens  << std::endl;
    sens.setType("calorimeter");
    //Materials
    DD4hep::Geometry::Material air = lcdd.air();
    
    //Access to the XML File
    xml_det_t     xmlLumiCal    = element;
    const std::string detName   = xmlLumiCal.nameStr();
    
    DD4hep::Geometry::DetElement sdet ( detName, xmlLumiCal.id() );
    DD4hep::Geometry::Volume motherVol = lcdd.pickMotherVolume(sdet);
    
    // --- create an envelope volume and position it into the world ---------------------
    
    DD4hep::Geometry::Volume envelope = DD4hep::XML::createPlacedEnvelope( lcdd, element , sdet ) ;
    
    sdet.setTypeFlag( DetType::CALORIMETER |  DetType::ENDCAP  | DetType::ELECTROMAGNETIC |  DetType::FORWARD ) ;

    if( lcdd.buildType() == DD4hep::BUILD_ENVELOPE ) return sdet ;
    
    //-----------------------------------------------------------------------------------
    
    DD4hep::XML::Dimension dimensions =  xmlLumiCal.dimensions();
    
    //LumiCal Dimensions
    const double lcalInnerR = dimensions.inner_r();
    const double lcalOuterR = dimensions.outer_r();
    const double lcalInnerZ = dimensions.inner_z();
    const double lcalThickness = DD4hep::Layering(xmlLumiCal).totalThickness();
    const double lcalCentreZ = lcalInnerZ+lcalThickness*0.5;
    
    double LumiCal_cell_size      = lcdd.constant<double>("LumiCal_cell_size");
    //========== fill data for reconstruction ============================
    DD4hep::DDRec::LayeredCalorimeterData* caloData = new DD4hep::DDRec::LayeredCalorimeterData ;
    caloData->layoutType = DD4hep::DDRec::LayeredCalorimeterData::EndcapLayout ;
    caloData->inner_symmetry = 0  ; // hardcoded tube
    caloData->outer_symmetry = 0  ; 
    caloData->phi0 = 0 ;
    
    /// extent of the calorimeter in the r-z-plane [ rmin, rmax, zmin, zmax ] in mm.
    caloData->extent[0] = lcalInnerR ;
    caloData->extent[1] = lcalOuterR ;
    caloData->extent[2] = lcalInnerZ ;
    caloData->extent[3] = lcalInnerZ + lcalThickness ;
    
    // counter for the current layer to be placed
    int thisLayerId = 0;
    
    //Parameters we have to know about
    DD4hep::XML::Component xmlParameter = xmlLumiCal.child(_Unicode(parameter));
    const double fullCrossingAngle  = xmlParameter.attr< double >(_Unicode(crossingangle));
    std::cout << " The crossing angle is: " << fullCrossingAngle << " radian"  << std::endl;
    
    
    //Envelope to place the layers in
    DD4hep::Geometry::Tube envelopeTube (lcalInnerR, lcalOuterR, lcalThickness*0.5 );
    DD4hep::Geometry::Volume     envelopeVol(detName+"_module",envelopeTube,air);
    envelopeVol.setVisAttributes(lcdd,xmlLumiCal.visStr());
    
    ////////////////////////////////////////////////////////////////////////////////
    // Create all the layers
    ////////////////////////////////////////////////////////////////////////////////
    
    //Loop over all the layer (repeat=NN) sections
    //This is the starting point to place all layers, we need this when we have more than one layer block
    double referencePosition = -lcalThickness*0.5;
    for(DD4hep::XML::Collection_t coll(xmlLumiCal,_U(layer)); coll; ++coll)  {
        DD4hep::XML::Component xmlLayer(coll); //we know this thing is a layer
        
        
        //This just calculates the total size of a single layer
        //Why no convenience function for this?
        double layerThickness = 0;
        for(DD4hep::XML::Collection_t l(xmlLayer,_U(slice)); l; ++l)
            layerThickness += xml_comp_t(l).thickness();
        
        std::cout << "Total Length "    << lcalThickness/dd4hep::cm  << " cm" << std::endl;
        std::cout << "Layer Thickness " << layerThickness/dd4hep::cm << " cm" << std::endl;
        
        //Loop for repeat=NN
        for(int i=0, repeat=xmlLayer.repeat(); i<repeat; ++i)  {
            
            std::string layer_name = detName + DD4hep::XML::_toString(thisLayerId,"_layer%d");
            DD4hep::Geometry::Tube layer_base(lcalInnerR,lcalOuterR,layerThickness*0.5);
            
            DD4hep::Geometry::Volume layer_vol(layer_name,layer_base,air);
            
            
            int sliceID=0;
            double inThisLayerPosition = -layerThickness*0.5;
            
            double nRadiationLengths=0.;
            double nInteractionLengths=0.;
            double thickness_sum=0;
            
            DD4hep::DDRec::LayeredCalorimeterData::Layer caloLayer ;
            
            for(DD4hep::XML::Collection_t collSlice(xmlLayer,_U(slice)); collSlice; ++collSlice)  {
                DD4hep::XML::Component compSlice = collSlice;
                const double      slice_thickness = compSlice.thickness();
                const std::string sliceName = layer_name + DD4hep::XML::_toString(sliceID,"slice%d");
                DD4hep::Geometry::Material   slice_material  = lcdd.material(compSlice.materialStr());
                
                DD4hep::Geometry::Tube sliceBase(lcalInnerR,lcalOuterR,slice_thickness/2);
                
                DD4hep::Geometry::Volume slice_vol (sliceName,sliceBase,slice_material);
                
                nRadiationLengths += slice_thickness/(2.*slice_material.radLength());
                nInteractionLengths += slice_thickness/(2.*slice_material.intLength());
                thickness_sum += slice_thickness/2;
                
                if ( compSlice.isSensitive() )  {

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
                    slice_vol.setSensitiveDetector(sens);
                }
                
                nRadiationLengths += slice_thickness/(2.*slice_material.radLength());
                nInteractionLengths += slice_thickness/(2.*slice_material.intLength());
                thickness_sum += slice_thickness/2;
                
                slice_vol.setAttributes(lcdd,compSlice.regionStr(),compSlice.limitsStr(),compSlice.visStr());
                layer_vol.placeVolume(slice_vol,DD4hep::Geometry::Position(0,0,inThisLayerPosition+slice_thickness*0.5));
                    
                inThisLayerPosition += slice_thickness;
                ++sliceID;
            }//For all slices in this layer
            
            //-----------------------------------------------------------------------------------------
            
            ///Needs to be innermost face distance
            caloLayer.distance = lcalCentreZ + referencePosition;

#if DD4HEP_VERSION_GE( 0, 15 )
            caloLayer.outer_nRadiationLengths = nRadiationLengths;
            caloLayer.outer_nInteractionLengths = nInteractionLengths;
            caloLayer.outer_thickness = thickness_sum;
#endif            
            
            caloLayer.cellSize0 = LumiCal_cell_size ;
            caloLayer.cellSize1 = LumiCal_cell_size ;
            
            caloData->layers.push_back( caloLayer ) ;
            //-----------------------------------------------------------------------------------------
            
            //Why are we doing this for each layer, this just needs to be done once and then placed multiple times
            //Do we need unique IDs for each piece?
            layer_vol.setVisAttributes(lcdd,xmlLayer.visStr());
            
            DD4hep::Geometry::Position layer_pos(0,0,referencePosition+0.5*layerThickness);
            referencePosition += layerThickness;
            DD4hep::Geometry::PlacedVolume pv = envelopeVol.placeVolume(layer_vol,layer_pos);
            pv.addPhysVolID("layer",thisLayerId);
            
            ++thisLayerId;
            
        }//for all layers
        
    }// for all layer collections
    
    const DD4hep::Geometry::Position bcForwardPos (std::tan(0.5*fullCrossingAngle)*lcalCentreZ,0.0, lcalCentreZ);
    const DD4hep::Geometry::Position bcBackwardPos(std::tan(0.5*fullCrossingAngle)*lcalCentreZ,0.0,-lcalCentreZ);
    const DD4hep::Geometry::Rotation3D bcForwardRot ( DD4hep::Geometry::RotationY(fullCrossingAngle*0.5 ) );
    const DD4hep::Geometry::Rotation3D bcBackwardRot( DD4hep::Geometry::RotationZYX ( (M_PI), (M_PI-fullCrossingAngle*0.5), (0.0)));
    
    DD4hep::Geometry::PlacedVolume pv =
    envelope.placeVolume(envelopeVol, DD4hep::Geometry::Transform3D( bcForwardRot, bcForwardPos ) );
    pv.addPhysVolID("barrel", 1);

    DD4hep::Geometry::PlacedVolume pv2 =
    envelope.placeVolume(envelopeVol, DD4hep::Geometry::Transform3D( bcBackwardRot, bcBackwardPos ) );
    pv2.addPhysVolID("barrel", 2);
    
    sdet.addExtension< DD4hep::DDRec::LayeredCalorimeterData >( caloData ) ;
    
    return sdet;
}
                                               
DECLARE_DETELEMENT(LumiCal_o1_v01,create_detector)
