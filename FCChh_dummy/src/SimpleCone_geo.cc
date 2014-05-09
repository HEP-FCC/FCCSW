// $Id: $
//====================================================================
//  AIDA Detector description implementation for FCC
//-------------------------------------------------------------------- 
// Simple cone using Tube to be used to define cone composed of 1 single material
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
  DetElement   cone( name, x_det.id() );
  
  Volume experimentalHall_log =  lcdd.pickMotherVolume( cone ) ;
  
  xml_comp_t  coneTubs ( x_det.child( _U(tubs) ) );  
  ///correct implementation but bug in the xml parser??
  //Cone cone_tubs( coneTubs.dz(), coneTubs.rmin1(), coneTubs.rmax1(), coneTubs.rmin2(), coneTubs.rmax2());
  Cone cone_tubs( coneTubs.dz(), coneTubs.rmin(), coneTubs.rmax(), coneTubs.outer_r(), coneTubs.outer_radius());

  Volume cone_log( x_det.nameStr()+ "_SimpleCone", cone_tubs, lcdd.material(coneTubs.materialStr()) );
  
  PlacedVolume cone_phys;
 
  double zoff = coneTubs.z_offset();
  if (fabs(zoff)>0.000000000001) {
    Position trans(0.,0.,zoff ) ;
    cone_phys = experimentalHall_log.placeVolume( cone_log, Transform3D( RotationZ(0.) , trans ));
  }  
  else cone_phys = experimentalHall_log.placeVolume( cone_log );
  

  cone_phys.addPhysVolID( "system", x_det.id() ).addPhysVolID("side",0 )  ;

  cone.setPlacement( cone_phys );

  cone.setVisAttributes( lcdd, x_det.visStr(), cone_log );



  
  return cone;
}

DECLARE_DETELEMENT( SimpleCone ,create_element )
