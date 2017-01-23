
#include "ACTS/Plugins/DD4hepPlugins/ActsExtension.hpp"
#include "DD4hep/DetFactoryHelper.h"

using DD4hep::Geometry::Volume;
using DD4hep::Geometry::PlacedVolume;
using DD4hep::Geometry::Tube;
using DD4hep::Geometry::DetElement;


static DD4hep::Ref_t create_element(DD4hep::Geometry::LCDD& lcdd, xml_h e, DD4hep::Geometry::SensitiveDetector ) {
  xml_det_t x_det = e;
  std::string det_name = x_det.nameStr();
  // Make DetElement
  DetElement beamtube(det_name, x_det.id());
  // add Extension to Detlement for the RecoGeometry
  Acts::ActsExtension::Config volConfig;
  volConfig.isBeampipe = true;
  // ownership of extension object given to the DetElement 
  Acts::ActsExtension* detvolume = new Acts::ActsExtension(volConfig);
  beamtube.addExtension<Acts::IActsExtension>(detvolume);
  // add Extension to Detlement for the RecoGeometry
  DD4hep::XML::Dimension x_det_dim(x_det.dimensions());
  Tube tube_shape(x_det_dim.rmin(), x_det_dim.rmax(), x_det_dim.z());
  Volume tube_vol(det_name, tube_shape, lcdd.air());  
  tube_vol.setVisAttributes(lcdd, x_det_dim.visStr());
  // Place Volume
  Volume mother_vol = lcdd.pickMotherVolume(beamtube);
  PlacedVolume placedTube = mother_vol.placeVolume(tube_vol);
  placedTube.addPhysVolID("tube", beamtube.id());
  beamtube.setPlacement(placedTube);

  return beamtube;
}

DECLARE_DETELEMENT(BeamTube, create_element)
