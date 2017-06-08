#ifndef RECCALORIMETER_CALIBRATECALOHITSTOOL_H
#define RECCALORIMETER_CALIBRATECALOHITSTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "RecInterface/ICalibrateCaloHitsTool.h"

#include <vector>

/** @class CalibrateCaloHitsTool
 *
 *  Tool for energy calibration to electromagnetic scale
 *  Geant4 energy deposits calibration in sampling calorimeters
 *
 *  1/sampling fraction (Ebeam/Etotal_hit_energy) used for the calibration, same factor for each cell used
 *    - depends on geometry (thickness of active/passive material, materials)
 *
 *  @author Jana Faltova
 *  @date   2016-09
 */

class CalibrateCaloHitsTool : public GaudiTool, virtual public ICalibrateCaloHitsTool {
public:
  CalibrateCaloHitsTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~CalibrateCaloHitsTool() = default;
  StatusCode initialize() override final;
  StatusCode finalize() override final;

  /** @brief  Calibrate Geant4 hit energy to EM scale
   */
  void calibrate(std::unordered_map<uint64_t, double>& aHits) override final;

private:
  /// Value of 1/sampling fraction
  Gaudi::Property<double> m_invSamplingFraction{this, "invSamplingFraction", 1.0, "Value of 1/sampling fraction"};
};
#endif /* RECCALORIMETER_CALIBRATECALOHITSTOOL_H */
