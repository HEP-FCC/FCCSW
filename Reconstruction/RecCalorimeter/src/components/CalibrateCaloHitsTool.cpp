#include "CalibrateCaloHitsTool.h"

DECLARE_TOOL_FACTORY(CalibrateCaloHitsTool)

CalibrateCaloHitsTool::CalibrateCaloHitsTool(const std::string& type,const std::string& name, const IInterface* parent) 
  : GaudiTool(type, name, parent)
{
  declareInterface<ICalibrateCaloHitsTool>(this);
  declareProperty("invSamplingFraction", m_invSamplingFraction=1.0);
}

CalibrateCaloHitsTool::~CalibrateCaloHitsTool()
{
}

StatusCode CalibrateCaloHitsTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  info() << "Calibration constant: 1/sampling fraction=" << m_invSamplingFraction << endmsg;
  return sc;
}

void CalibrateCaloHitsTool::calibrate(std::vector<fcc::CaloHit*>& aHits) {
  //Loop through vector with Geant4 energy deposits, multiply energy to get cell energy at electromagnetic scale
  for (auto& ecells : aHits) {
    ecells->Core().Energy *= m_invSamplingFraction;
  }
}

StatusCode CalibrateCaloHitsTool::finalize() {
  StatusCode sc = GaudiTool::finalize();
  return sc;
}
