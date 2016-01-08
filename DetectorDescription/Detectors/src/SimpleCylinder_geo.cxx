// $Id: $
//====================================================================
//  AIDA Detector description implementation for FCC
//-------------------------------------------------------------------- 
// Simple cylinder using Tube to be used to define cylinder composed of 1 single material
//
// @author Clement Helsens
//====================================================================
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;


static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {


  xml_det_t    x_det = e;
  string       name  = x_det.nameStr();
  DetElement   cylinderDet( name, x_det.id() );
  
  Volume experimentalHall =  lcdd.pickMotherVolume( cylinderDet ) ;
  
  xml_comp_t  cylinderDim ( x_det.child( _U(dimensions) ) );
  
  Tube cylinder( cylinderDim.rmin(),cylinderDim.rmax(),cylinderDim.dz(), cylinderDim.phi0(),cylinderDim.deltaphi() ); 
  
  Volume cylinderVol( x_det.nameStr()+ "_SimpleCylinder", cylinder, lcdd.material(cylinderDim.materialStr()) );
  
  PlacedVolume cylinderPhys;
 
  double zoff = cylinderDim.z_offset();
  if (fabs(zoff)>0.000000000001) {
    Position trans(0.,0.,zoff ) ;
    cylinderPhys = experimentalHall.placeVolume( cylinderVol, Transform3D( RotationZ(0.) , trans ));
  }  
  else cylinderPhys = experimentalHall.placeVolume( cylinderVol );
  

  cylinderPhys.addPhysVolID( "system", x_det.id() ).addPhysVolID("side",0 )  ;

  cylinderDet.setPlacement( cylinderPhys );

  cylinderDet.setVisAttributes( lcdd, x_det.visStr(), cylinderVol );



  
  return cylinderDet;
}

DECLARE_DETELEMENT( SimpleCylinder ,create_element )
