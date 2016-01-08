// $Id: $
//====================================================================
//  AIDA Detector description implementation for FCC
//-------------------------------------------------------------------- 
// Simple parallelogram using dimensions to be used to define cone composed of 1 single material
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
  DetElement   parallelogramDet( name, x_det.id() );
  
  Volume experimentalHall =  lcdd.pickMotherVolume( parallelogramDet ) ;
  
  xml_comp_t  parallelogramDim ( x_det.child( _U(dimensions) ) );  
  ///correct implementation but bug in the xml parser??

   const double parallelogramPoints[16] = {parallelogramDim.x0(),parallelogramDim.y0(),
                                           parallelogramDim.x0(),parallelogramDim.y0()+parallelogramDim.dy(),
                                           parallelogramDim.x0()+parallelogramDim.dx(),parallelogramDim.y0()+parallelogramDim.dy(),
                                           parallelogramDim.x0()+parallelogramDim.dx(),parallelogramDim.y0(),
                                           parallelogramDim.x1(),parallelogramDim.y1(),
                                           parallelogramDim.x1(),parallelogramDim.y1()+parallelogramDim.dy(),
                                           parallelogramDim.x1()+parallelogramDim.dx(),parallelogramDim.y1()+parallelogramDim.dy(),
                                           parallelogramDim.x1()+parallelogramDim.dx(),parallelogramDim.y1()};


   EightPointSolid parallelogram(parallelogramDim.dz(), parallelogramPoints);

   Volume parallelogramVol( x_det.nameStr()+ "_SimpleParallelogram", parallelogram, lcdd.material(parallelogramDim.materialStr()) );
  
   PlacedVolume parallelogramPhys;
 
   double zoff = parallelogramDim.z_offset();

   if ( fabs(zoff)>0.000000000001) {
     Position trans(0.,0.,zoff ) ;
     parallelogramPhys = experimentalHall.placeVolume( parallelogramVol, Transform3D( RotationZ(0.) , trans ));
   }  
  else parallelogramPhys = experimentalHall.placeVolume( parallelogramVol );
  

  parallelogramPhys.addPhysVolID( "system", x_det.id() ).addPhysVolID("side",0 )  ;

  parallelogramDet.setPlacement( parallelogramPhys );

  parallelogramDet.setVisAttributes( lcdd, x_det.visStr(), parallelogramVol );



  
  return parallelogramDet;
}

DECLARE_DETELEMENT( SimpleParallelogram ,create_element )
