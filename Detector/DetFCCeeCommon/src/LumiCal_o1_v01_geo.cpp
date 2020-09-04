#include <DD4hep/DetFactoryHelper.h>
#include "DD4hep/DetType.h"
#include <XML/Layering.h>
#include "XML/Utilities.h"
#include "DDRec/DetectorData.h"

// Gaudi
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

#include <string>

using dd4hep::Assembly;
using dd4hep::BUILD_ENVELOPE;
using dd4hep::Box;
using dd4hep::Cone;
using dd4hep::DetElement;
using dd4hep::Detector;
using dd4hep::DetType;
using dd4hep::IntersectionSolid;
using dd4hep::Layering;
using dd4hep::Layer;
using dd4hep::Material;
using dd4hep::PlacedVolume;
using dd4hep::PolyhedraRegular;
using dd4hep::Position;
using dd4hep::Readout;
using dd4hep::Ref_t;
using dd4hep::Rotation3D;
using dd4hep::RotationY;
using dd4hep::RotationZYX;
using dd4hep::SensitiveDetector;
using dd4hep::SubtractionSolid;
using dd4hep::Transform3D;
using dd4hep::Translation3D;
using dd4hep::Trapezoid;
using dd4hep::Tube;
using dd4hep::Volume;
using dd4hep::_toString;
using dd4hep::UnionSolid;
using dd4hep::IntersectionSolid;
using dd4hep::Segmentation;

using dd4hep::rec::LayeredCalorimeterData;

namespace det {
static Ref_t  LumiCal_o1_v01_geo(Detector& theDetector,
                             xml_h element,
                             SensitiveDetector sens) {
  ServiceHandle<IMessageSvc> msgSvc("MessageSvc", "LumiCalConstruction");
  MsgStream lLog(&(*msgSvc), "LumiCalConstruction");
    
  lLog << MSG::DEBUG << __PRETTY_FUNCTION__ << endmsg;
  lLog << MSG::DEBUG << " The sensitive detector for the LumiCal: " << &sens  << endmsg;

  // sens.setType("calorimeter");   // MANU
     sens.setType("SimpleCalorimeterSD");
    //Materials
    Material air = theDetector.air();
    
    //Access to the XML File
    xml_det_t     xmlLumiCal    = element;
    const std::string detName   = xmlLumiCal.nameStr();
    
    DetElement sdet ( detName, xmlLumiCal.id() );
    
    // --- create an envelope volume and position it into the world ---------------------
    
    Volume envelope = dd4hep::xml::createPlacedEnvelope( theDetector, element , sdet ) ;
    DetElement lumiCalDE_1(sdet,"Calorimeter1",1);
    DetElement lumiCalDE_2(sdet,"Calorimeter2",2);

    sdet.setTypeFlag( DetType::CALORIMETER |  DetType::ENDCAP  | DetType::ELECTROMAGNETIC |  DetType::FORWARD ) ;

    if( theDetector.buildType() == BUILD_ENVELOPE ) return sdet ;
    
    //-----------------------------------------------------------------------------------
    
    dd4hep::xml::Dimension dimensions =  xmlLumiCal.dimensions();
    
    //LumiCal Dimensions
    const double lcalInnerR = dimensions.inner_r();
    const double lcalOuterR = dimensions.outer_r();
    const double lcalInnerZ = dimensions.inner_z();
    const double lcalThickness = Layering(xmlLumiCal).totalThickness();
    const double lcalCentreZ = lcalInnerZ+lcalThickness*0.5;
    
    double LumiCal_cell_size      = theDetector.constant<double>("LumiCal_cell_size");
    //========== fill data for reconstruction ============================
    LayeredCalorimeterData* caloData = new LayeredCalorimeterData ;
    caloData->layoutType = LayeredCalorimeterData::EndcapLayout ;
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
    dd4hep::xml::Component xmlParameter = xmlLumiCal.child(_Unicode(parameter));
    const double fullCrossingAngle  = xmlParameter.attr< double >(_Unicode(crossingangle));
    lLog << MSG::DEBUG << " The crossing angle is: " << fullCrossingAngle << " radian"  << endmsg;
    
    
    //Envelope to place the layers in
    Tube envelopeTube (lcalInnerR, lcalOuterR, lcalThickness*0.5 );
    Volume     envelopeVol(detName+"_module",envelopeTube,air);
    envelopeVol.setVisAttributes(theDetector,xmlLumiCal.visStr());
    
    ////////////////////////////////////////////////////////////////////////////////
    // Create all the layers
    ////////////////////////////////////////////////////////////////////////////////
    
    //Loop over all the layer (repeat=NN) sections
    //This is the starting point to place all layers, we need this when we have more than one layer block
    double referencePosition = -lcalThickness*0.5;
    for(dd4hep::xml::Collection_t coll(xmlLumiCal,_U(layer)); coll; ++coll)  {
        dd4hep::xml::Component xmlLayer(coll); //we know this thing is a layer
        
        
        //This just calculates the total size of a single layer
        //Why no convenience function for this?
        double layerThickness = 0;
        for(dd4hep::xml::Collection_t l(xmlLayer,_U(slice)); l; ++l)
            layerThickness += xml_comp_t(l).thickness();
        
        lLog << MSG::DEBUG << "Total Length "    << lcalThickness/dd4hep::cm  << " cm" << endmsg;
        lLog << MSG::DEBUG << "Layer Thickness " << layerThickness/dd4hep::cm << " cm" << endmsg;
        
        //Loop for repeat=NN
        for(int i=0, repeat=xmlLayer.repeat(); i<repeat; ++i)  {
            
            std::string layer_name = detName + dd4hep::xml::_toString(thisLayerId,"_layer%d");
            Tube layer_base(lcalInnerR,lcalOuterR,layerThickness*0.5);
            
            Volume layer_vol(layer_name,layer_base,air);
            
            
            int sliceID=0;
            double inThisLayerPosition = -layerThickness*0.5;
            
            double nRadiationLengths=0.;
            double nInteractionLengths=0.;
            double thickness_sum=0;
            
            LayeredCalorimeterData::Layer caloLayer ;
            
            for(dd4hep::xml::Collection_t collSlice(xmlLayer,_U(slice)); collSlice; ++collSlice)  {
                dd4hep::xml::Component compSlice = collSlice;
                const double      slice_thickness = compSlice.thickness();
                const std::string sliceName = layer_name + dd4hep::xml::_toString(sliceID,"slice%d");
                Material   slice_material  = theDetector.material(compSlice.materialStr());
                
                Tube sliceBase(lcalInnerR,lcalOuterR,slice_thickness/2);
                
                Volume slice_vol (sliceName,sliceBase,slice_material);
                
                nRadiationLengths += slice_thickness/(2.*slice_material.radLength());
                nInteractionLengths += slice_thickness/(2.*slice_material.intLength());
                thickness_sum += slice_thickness/2;
                
                if ( compSlice.isSensitive() )  {
                    //Reset counters to measure "outside" quantitites
                    nRadiationLengths=0.;
                    nInteractionLengths=0.;
                    thickness_sum = 0.;
                    slice_vol.setSensitiveDetector(sens);
                }
                
                nRadiationLengths += slice_thickness/(2.*slice_material.radLength());
                nInteractionLengths += slice_thickness/(2.*slice_material.intLength());
                thickness_sum += slice_thickness/2;
                
                slice_vol.setAttributes(theDetector,compSlice.regionStr(),compSlice.limitsStr(),compSlice.visStr());
                layer_vol.placeVolume(slice_vol,Position(0,0,inThisLayerPosition+slice_thickness*0.5));
                    
                inThisLayerPosition += slice_thickness;
                ++sliceID;
            }//For all slices in this layer
            
            //-----------------------------------------------------------------------------------------
            
            ///Needs to be innermost face distance
            caloLayer.distance = lcalCentreZ + referencePosition;
            
            caloLayer.cellSize0 = LumiCal_cell_size ;
            caloLayer.cellSize1 = LumiCal_cell_size ;
            
            caloData->layers.push_back( caloLayer ) ;
            //-----------------------------------------------------------------------------------------
            
            //Why are we doing this for each layer, this just needs to be done once and then placed multiple times
            //Do we need unique IDs for each piece?
            layer_vol.setVisAttributes(theDetector,xmlLayer.visStr());
            
            Position layer_pos(0,0,referencePosition+0.5*layerThickness);
            referencePosition += layerThickness;
            PlacedVolume pv = envelopeVol.placeVolume(layer_vol,layer_pos);
            pv.addPhysVolID("layer",thisLayerId);
            
            ++thisLayerId;
            
        }//for all layers
        
    }// for all layer collections
    
    const Position bcForwardPos (std::tan(0.5*fullCrossingAngle)*lcalCentreZ,0.0, lcalCentreZ);
    const Position bcBackwardPos(std::tan(0.5*fullCrossingAngle)*lcalCentreZ,0.0,-lcalCentreZ);
    const Rotation3D bcForwardRot ( RotationY(fullCrossingAngle*0.5 ) );
    const Rotation3D bcBackwardRot( RotationZYX ( (M_PI), (M_PI-fullCrossingAngle*0.5), (0.0)));
    
    PlacedVolume pv =
    envelope.placeVolume(envelopeVol, Transform3D( bcForwardRot, bcForwardPos ) );
    pv.addPhysVolID("barrel", 1);
    lumiCalDE_1.setPlacement(pv);

    PlacedVolume pv2 =
    envelope.placeVolume(envelopeVol, Transform3D( bcBackwardRot, bcBackwardPos ) );
    pv2.addPhysVolID("barrel", 2);
    lumiCalDE_2.setPlacement(pv2);
    
    sdet.addExtension< LayeredCalorimeterData >( caloData ) ;
    
    return sdet;
}
}
                                               
DECLARE_DETELEMENT(LumiCal_o1_v01,det:: LumiCal_o1_v01_geo )
