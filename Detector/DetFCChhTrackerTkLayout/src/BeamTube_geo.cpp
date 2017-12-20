

#include "ACTS/Plugins/DD4hepPlugins/ActsExtension.hpp"
#include "DD4hep/DetFactoryHelper.h"

using dd4hep::Volume;
using dd4hep::PlacedVolume;
using dd4hep::Tube;
using dd4hep::DetElement;

static dd4hep::Ref_t create_element(dd4hep::Detector& lcdd, xml_h e, dd4hep::SensitiveDetector ) {
  xml_det_t x_det = e;
  std::string det_name = x_det.nameStr();
  // Make DetElement
  DetElement beamtube(det_name, x_det.id());
  Acts::ActsExtension::Config volConfig;
  volConfig.isBeampipe = true;
  Acts::ActsExtension* detvolume = new Acts::ActsExtension(volConfig);
  beamtube.addExtension<Acts::IActsExtension>(detvolume);
  dd4hep::xml::Dimension x_det_dim(x_det.dimensions());
  Tube tube_shape(x_det_dim.rmin(), x_det_dim.rmax(), x_det_dim.z());
  Volume tube_vol(det_name, tube_shape, lcdd.material(x_det_dim.attr<std::string>("material")));
  tube_vol.setVisAttributes(lcdd, x_det_dim.visStr());
  // Place Volume
  Volume mother_vol = lcdd.pickMotherVolume(beamtube);
  PlacedVolume placedTube = mother_vol.placeVolume(tube_vol);
  beamtube.setPlacement(placedTube);
  return beamtube;
}

DECLARE_DETELEMENT(BeamTube, create_element)
