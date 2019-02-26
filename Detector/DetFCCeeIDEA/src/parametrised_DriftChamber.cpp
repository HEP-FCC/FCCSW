#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/Printout.h"
#include "DDSegmentation/Segmentation.h"
#include "DetSegmentation/GridDriftChamber.h"
#include "XML/Utilities.h"
#include "XML/XMLElements.h"

namespace det {

static dd4hep::Ref_t createParamDriftChamber(dd4hep::Detector& theDetector, xml_h e,
                                             dd4hep::SensitiveDetector aSensDet) {
  // ------- Lambda functions ---- //
  auto delta_a_func = [](auto x, auto y) { return 2 * std::sqrt((x + y) * (x - y)); };
  auto epsilon_func = [](auto delta_a, auto L) {
    return std::atan(delta_a / L);
  };  // L is the full length of the detector
  // ----------------------------- //

  xml_det_t x_det = e;
  std::string name = x_det.nameStr();
  dd4hep::DetElement sdet(name, x_det.id());
  dd4hep::Volume mother = theDetector.pickMotherVolume(sdet);

  dd4hep::SensitiveDetector sens = aSensDet;
  dd4hep::xml::Dimension sdType = x_det.child(_U(sensitive));
  sens.setType(sdType.typeStr());

  xml_comp_t detectorDim(x_det.child(_U(dimensions)));
  xml_comp_t layerDim(x_det.child(_U(layer)));

  int nSuperLayer = layerDim.attr<int>(_Unicode(nSuperLayer));
  int nRing = layerDim.attr<int>(_Unicode(nRing));
  double cellDimension = layerDim.attr<double>(_Unicode(cellDimension));
  double halfLength = layerDim.attr<double>(_Unicode(halfLength));
  double r0 = layerDim.attr<double>(_Unicode(r0));
  double alpha = layerDim.attr<double>(_Unicode(alpha));

  // Initialize the segmentation
  dd4hep::Readout readout = sens.readout();
  dd4hep::Segmentation geomseg = readout.segmentation();
  dd4hep::Segmentation* _geoSeg = &geomseg;

  auto DCHseg = dynamic_cast<dd4hep::DDSegmentation::GridDriftChamber*>(_geoSeg->segmentation());

  dd4hep::Tube envelopeVol(detectorDim.rmin(), detectorDim.rmax(), detectorDim.dz());
  dd4hep::Volume envelopeVolume;

  dd4hep::PlacedVolume pv;

  for (int superlayer = 0; superlayer < nSuperLayer; superlayer++) {
    for (int iring = 0; iring < nRing; iring++) {
      auto layer_id = superlayer * nRing + iring;
      int numWire = 192 + superlayer * 48;

      double R_i0 = r0 + (layer_id)*cellDimension;  // Location of the wire at z=0
      double R_i = R_i0 /
          std::cos(alpha / 2.0);  // the radius of the circle containing the wire (at z=halfLength or z=-halfLength)
      double sign_eps = 1;
      if (layer_id % 2) {
        sign_eps = -1;
      }
      double eps_wire = sign_eps * std::atan(2 * R_i0 * std::tan(alpha / 2.0) / (halfLength * 2.0));

      double r_in0 = R_i0 - cellDimension / 2.0;
      double r_in = r_in0 / std::cos(alpha / 2.0);

      double r_out0 = R_i0 + cellDimension / 2.0;
      double r_out = r_out0 / std::cos(alpha / 2.0);

      double delta_a_in = delta_a_func(r_in, r_in0);
      double delta_a_out = delta_a_func(r_out, r_out0);

      double eps_in = epsilon_func(delta_a_in, halfLength * 2);
      double eps_out = epsilon_func(delta_a_out, halfLength * 2);

      if (layer_id == 0) {
        dd4hep::Hyperboloid negativeVol(0, 0, r_in0, eps_in, 5 * halfLength);  // to avoid overlaps
        dd4hep::SubtractionSolid envelopeShape(envelopeVol, negativeVol);
        envelopeVolume = dd4hep::Volume(x_det.nameStr(), envelopeShape, theDetector.material("Air"));
        envelopeVolume.setVisAttributes(theDetector, detectorDim.visStr());
      }

      dd4hep::Hyperboloid hyperbol(r_in0, eps_in, r_out0, eps_out, halfLength);
      std::string layerName = dd4hep::xml::_toString(layer_id, "layer%i");
      dd4hep::Volume hyperbolVol(x_det.nameStr() + layerName, hyperbol,
                                 theDetector.material(detectorDim.materialStr()));

      hyperbolVol.setVisAttributes(theDetector, x_det.visStr());
      hyperbolVol.setSensitiveDetector(sens);

      DCHseg->setGeomParams(layer_id, 2.0 * M_PI / double(numWire), R_i, eps_wire);
      DCHseg->setWiresInLayer(layer_id, numWire);

      pv = envelopeVolume.placeVolume(hyperbolVol);
      pv.addPhysVolID("layer", layer_id);
    }
  }  // end superlayer

  pv = mother.placeVolume(envelopeVolume);
  pv.addPhysVolID("system", x_det.id());
  sdet.setPlacement(pv);
  return sdet;
}
}

DECLARE_DETELEMENT(parametrised_DriftChamber, det::createParamDriftChamber)
