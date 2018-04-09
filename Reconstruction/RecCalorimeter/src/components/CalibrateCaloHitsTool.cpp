#include "CalibrateCaloHitsTool.h"

DECLARE_TOOL_FACTORY(CalibrateCaloHitsTool)

CalibrateCaloHitsTool::CalibrateCaloHitsTool(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ICalibrateCaloHitsTool>(this);
}

StatusCode CalibrateCaloHitsTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }
  info() << "Calibration constant: 1/sampling fraction=" << m_invSamplingFraction << endmsg;
  return sc;
}

void CalibrateCaloHitsTool::calibrate(std::unordered_map<uint64_t, double>& aHits) {
  // Loop through energy deposits, multiply energy to get cell energy at electromagnetic scale
  std::for_each(aHits.begin(), aHits.end(),
                [this](std::pair<const uint64_t, double>& p) { p.second *= m_invSamplingFraction; });
}

StatusCode CalibrateCaloHitsTool::finalize() { return GaudiTool::finalize(); }
