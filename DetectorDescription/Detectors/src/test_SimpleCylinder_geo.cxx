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
  DetElement   cylinder( name, x_det.id() );
  
  Volume experimentalHall_log =  lcdd.pickMotherVolume( cylinder ) ;
  
  xml_comp_t  cylTubs ( x_det.child( _U(tubs) ) );
  
  Tube cylinder_tubs( cylTubs.rmin(),cylTubs.rmax(),cylTubs.dz(), cylTubs.phi0(), cylTubs.deltaphi() ); 
  
  Volume cylinder_log( x_det.nameStr()+ "_SimpleCylinder", cylinder_tubs, lcdd.material(cylTubs.materialStr()) );
  
  PlacedVolume cylinder_phys;
 
  double zoff = cylTubs.z_offset();
  if (fabs(zoff)>0.000000000001) {
    Position trans(0.,0.,zoff ) ;
    cylinder_phys = experimentalHall_log.placeVolume( cylinder_log, Transform3D( RotationZ(0.) , trans ));
  }  
  else cylinder_phys = experimentalHall_log.placeVolume( cylinder_log );
  

  cylinder_phys.addPhysVolID( "system", x_det.id() ).addPhysVolID("side",0 )  ;

  cylinder.setPlacement( cylinder_phys );

  cylinder.setVisAttributes( lcdd, x_det.visStr(), cylinder_log );



  
  return cylinder;
}

DECLARE_DETELEMENT( SimpleCylinder ,create_element )
