#include <DD4hep/DetFactoryHelper.h>
#include "DD4hep/DetType.h"
#include <XML/Layering.h>
#include "XML/Utilities.h"
#include "DDRec/DetectorData.h"

#include <string>

using namespace DD4hep;
using namespace DD4hep::Geometry;


static DD4hep::Geometry::Ref_t create_detector(DD4hep::Geometry::LCDD& lcdd,
                                               xml_h element,
                                               DD4hep::Geometry::SensitiveDetector /*sens*/) {

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
    // Making a conical envelope

    DD4hep::XML::Dimension dimensions =  xmlLumiCal.dimensions();
    
    //LumiCal Dimensions
    const double lcalInnerZ = dimensions.inner_z();
    const double lcalOuterZ = dimensions.outer_z() ;
    const double rInnerStart = dimensions.rmin1();
    const double rOuterStart = dimensions.rmin2();
    const double rInnerEnd = dimensions.rmax1();
    const double rOuterEnd = dimensions.rmax2();

    //const double zHalf = dimensions.zhalf();
    const double lcalThickness = DD4hep::Layering(xmlLumiCal).totalThickness();
    //const double lcalThickness = 2*zHalf;
    const double lcalCentreZ = lcalOuterZ-lcalThickness*0.5;
 
    
    //double LumiCal_cell_size      = lcdd.constant<double>("LumiCal_cell_size");

    //** DD4hep/TGeo seems to need rad (as opposed to the manual)
    const double phi1 = 0 ;
    const double phi2 = 360.0*dd4hep::degree;
    const double thetaInn = 0.050*dd4hep::rad;
    const double thetaOut = 0.140*dd4hep::rad;

    std::cout << " LumiCal dimensions " << "rInnerStart " << rInnerStart << "rInnerEnd " << rInnerEnd << "rOuterStart " << rOuterStart << "rOuterEnd " << rOuterEnd << " size along z " << lcalThickness <<  " starts at " << lcalInnerZ << " ends at " << lcalOuterZ << "centered at " << lcalCentreZ << std::endl ;


    // counter for the current layer to be placed
    int thisLayerId = 0;
    
    //Parameters we have to know about
    DD4hep::XML::Component xmlParameter = xmlLumiCal.child(_Unicode(parameter));
    const double fullCrossingAngle  = xmlParameter.attr< double >(_Unicode(crossingangle));
    std::cout << " The crossing angle is: " << fullCrossingAngle << " radian"  << std::endl;
    
    
    //Envelope to place the layers in
    //Moving from cylindrical to conical geometry
    //DD4hep::Geometry::Tube envelopeTube (lcalInnerR, lcalOuterR, lcalThickness*0.5 );
    DD4hep::Geometry::ConeSegment envelopeCone (lcalThickness*0.5, rInnerStart, rOuterStart, rInnerEnd, rOuterEnd, phi1, phi2) ;
    DD4hep::Geometry::Volume envelopeVol(detName+"_module",envelopeCone,air);
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
        
	// Initialisation for rInn1 and rOut1 for the first conical layer
	double rInn1 = rInnerStart ;
	double rOut1 = rOuterStart ;
	double rInn2 = 0 ;
	double rOut2 = 0 ; 

        //Loop for repeat=NN
        for(int i=0, repeat=xmlLayer.repeat(); i<repeat; ++i)  {
            
            std::string layer_name = detName + DD4hep::XML::_toString(thisLayerId,"_layer%d");
	    //Need to create layers from conical segments
            //DD4hep::Geometry::Tube layer_base(lcalInnerR,lcalOuterR,layerThickness*0.5);
	    //Definition of inner and outer end radii for each layer
	    rInn2 = rInn1 + tan(thetaInn)*layerThickness ;
	    rOut2 = rOut1 + tan(thetaOut)*layerThickness ;

	    std::cout << " Starting radii for layer " << i << " rinn " << rInn1 << " rout " << rOut1 << std::endl ;

	    DD4hep::Geometry::ConeSegment layer_base(layerThickness*0.5,rInn1,rOut1,rInn2,rOut2,phi1,phi2);

	    std::cout << " angle of the layer " << tan((rOut2-rOut1)/layerThickness) << std::endl ;
            
            DD4hep::Geometry::Volume layer_vol(layer_name,layer_base,air);
            

            //Why are we doing this for each layer, this just needs to be done once and then placed multiple times
            //Do we need unique IDs for each piece?
            layer_vol.setVisAttributes(lcdd,xmlLayer.visStr());
            
            DD4hep::Geometry::Position layer_pos(0,0,referencePosition+0.5*layerThickness);
            referencePosition += layerThickness;
            DD4hep::Geometry::PlacedVolume pv = envelopeVol.placeVolume(layer_vol,layer_pos);
            pv.addPhysVolID("layer",thisLayerId);

	    rInn1 = rInn2 ;
	    rOut1 = rOut2 ;

	    std::cout << " I am putting layer " << i << " at " << layer_pos << std::endl ;
	    std::cout << " Starting radii for the NEXT layer " << i+1 << " rinn " << rInn1 << " rout " << rOut1 << std::endl ;
    	    
            
            ++thisLayerId;
            
        }//for all layers
        
    }// for all layer collections





    const DD4hep::Geometry::Position bcForwardPos (std::tan(0.5*fullCrossingAngle)*lcalCentreZ,0.0, lcalCentreZ);
    const DD4hep::Geometry::Position bcBackwardPos(std::tan(0.5*fullCrossingAngle)*lcalCentreZ,0.0,-lcalCentreZ);
    const DD4hep::Geometry::Rotation3D bcForwardRot ( DD4hep::Geometry::RotationY(fullCrossingAngle*0.5 ) );
    const DD4hep::Geometry::Rotation3D bcBackwardRot( DD4hep::Geometry::RotationZYX ( (M_PI), (M_PI-fullCrossingAngle*0.5), (0.0)));

    DD4hep::Geometry::PlacedVolume pv = envelope.placeVolume(envelopeVol, DD4hep::Geometry::Transform3D( bcForwardRot, bcForwardPos ) );
    pv.addPhysVolID("barrel", 1);

    DD4hep::Geometry::PlacedVolume pv2 = envelope.placeVolume(envelopeVol, DD4hep::Geometry::Transform3D( bcBackwardRot, bcBackwardPos ) );
    pv2.addPhysVolID("barrel", 2);


    return sdet;
}
                                               
DECLARE_DETELEMENT(LumiCal_o2_v02,create_detector)
