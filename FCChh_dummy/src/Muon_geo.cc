// $Id: $
//====================================================================
//  AIDA Detector description implementation for LCD
//-------------------------------------------------------------------- 
// geant4 example/novice/N04 ported to DD4hep
//
// @author F.Gaede, DESY
//====================================================================
#include "DD4hep/DetFactoryHelper.h"

#include "DD4hep/TGeoUnits.h"

using namespace std;
using namespace tgeo ;
using namespace DD4hep;
using namespace DD4hep::Geometry;


static Ref_t create_element(LCDD& lcdd, xml_h e, SensitiveDetector sens)  {


  xml_det_t    x_det = e;
  string       name  = x_det.nameStr();
  DetElement   muon( name, x_det.id() );
  
  Volume experimentalHall_log =  lcdd.pickMotherVolume( muon ) ;
  
  xml_comp_t  muonBox ( x_det.child( _U(box) ) );
  
  int layerID = 0 ;
  for(xml_coll_t c(e,_U(layer)); c; ++c, ++layerID)  {

    xml_comp_t  x_layer(c);
    double radius = x_layer.radius() ;
    double radius_eff = radius*sqrt(2./(1+cos(2.*M_PI/muonBox.repeat())));
    double mBoxWidth = radius_eff*sqrt(2.*(1. - cos( 2.*M_PI/muonBox.repeat() )));

    Box muon_box( mBoxWidth/2., muonBox.thickness(), muonBox.length() ) ;
  
    Volume muon_log( "muonT_L", muon_box, lcdd.material(x_layer.materialStr() ) );
    
    muon_log.setSensitiveDetector( sens ) ;
    
    double deltaphi = muonBox.deltaphi()  ;

    for(int i=0,N = muonBox.repeat() ; i < N ; ++i ){
    
      double phi = deltaphi/rad * i ;
      
      Position trans( radius * sin( phi ) ,
                      radius * cos( phi ) ,
                      0. ) ;
      


      PlacedVolume muon_phys = experimentalHall_log.placeVolume( muon_log , Transform3D( RotationZ(-phi) , trans ) );
      
      muon_phys.addPhysVolID( "layer",layerID )  ;
      
      muon.setPlacement( muon_phys );
    }
    muon.setVisAttributes( lcdd, x_layer.visStr(), muon_log );
  }

  
  return muon;
}

//first argument is the type from the xml file
DECLARE_DETELEMENT( Muon ,create_element )
