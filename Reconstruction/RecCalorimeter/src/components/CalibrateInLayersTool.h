#ifndef RECCALORIMETER_CALIBRATEINLAYERSTOOL_H
#define RECCALORIMETER_CALIBRATEINLAYERSTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ServiceHandle.h"

// FCCSW
#include "RecInterface/ICalibrateCaloHitsTool.h"
class IGeoSvc;

#include <vector>

/** @class CalibrateInLayersTool Reconstruction/RecCalorimeter/src/components/CalibrateInLayersTool.h
 * CalibrateInLayersTool.h
 *
 *  Tool for energy calibration to the electromagnetic scale.
 *  Geant4 energy deposits calibration in sampling calorimeters.
 *
 *  Sampling fraction (energy deposited in the active material to the total deposited energy) is defined for layers of
 *  the calorimeter.
 *  It is used for calorimeters with varying sampling fraction in radial distance.
 *  Sampling fraction for each layer is used for the calibration of the deposits within this layer.
 *  Sampling fractions for layers may be obtained using SamplingFractionInLayers algorithm.
 *
 *  @author Anna Zaborowska
 */

class CalibrateInLayersTool : public GaudiTool, virtual public ICalibrateCaloHitsTool {
public:
  CalibrateInLayersTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~CalibrateInLayersTool() = default;
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;

  /** @brief  Calibrate Geant4 hit energy to EM scale
   */
  virtual void calibrate(std::unordered_map<uint64_t, double>& aHits) final;

private:
  /// Pointer to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;
  /// Name of the detector readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "", "Name of the detector readout"};
  /// Name of the cells/layer field
  Gaudi::Property<std::string> m_layerFieldName{this, "layerFieldName", "", "Identifier of layers"};
  /// Id of the first layer (current design starts layer ids at 1)
  Gaudi::Property<uint> m_firstLayerId{this, "firstLayerId", 0, "ID of first layer"};
  /// Values of sampling fraction
  Gaudi::Property<std::vector<double>> m_samplingFraction{
      this, "samplingFraction", {}, "Values of sampling fraction per layer"};
};
#endif /* RECCALORIMETER_CALIBRATEINLAYERSTOOL_H */
