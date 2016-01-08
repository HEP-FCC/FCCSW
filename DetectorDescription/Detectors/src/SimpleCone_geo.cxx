// $Id: $
//====================================================================
//  AIDA Detector description implementation for FCC
//-------------------------------------------------------------------- 
// Simple cone using dimensions to be used to define cone composed of 1 single material
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
  DetElement   coneDet( name, x_det.id() );
  
  Volume experimentalHall =  lcdd.pickMotherVolume( coneDet ) ;
  
  xml_comp_t  coneDim ( x_det.child( _U(dimensions) ) );  
  Cone cone( coneDim.dz(), coneDim.rmin1(), coneDim.rmax1(), coneDim.rmin2(), coneDim.rmax2());

  Volume coneVol( x_det.nameStr()+ "_SimpleCone", cone, lcdd.material(coneDim.materialStr()) );
  
  PlacedVolume conePhys;
 
  double zoff = coneDim.z_offset();
  if (fabs(zoff)>0.000000000001) {
    Position trans(0.,0.,zoff ) ;
    conePhys = experimentalHall.placeVolume( coneVol, Transform3D( RotationZ(0.) , trans ));
  }  
  else conePhys = experimentalHall.placeVolume( coneVol );
  
  conePhys.addPhysVolID( "system", x_det.id() ).addPhysVolID("side",0 )  ;

  coneDet.setPlacement( conePhys );

  coneDet.setVisAttributes( lcdd, x_det.visStr(), coneVol );



  
  return coneDet;
}

DECLARE_DETELEMENT( SimpleCone ,create_element )
