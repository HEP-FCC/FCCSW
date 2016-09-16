#include "CalibrateCaloCellsTool.h"

DECLARE_TOOL_FACTORY(CalibrateCaloCellsTool)

CalibrateCaloCellsTool::CalibrateCaloCellsTool(const std::string& type,const std::string& name, const IInterface* parent) 
  : GaudiTool(type, name, parent),
  m_sf(5.4)
{
  declareInterface<ICalibrateCaloCellsTool>(this);
  declareProperty("SF", m_sf);
}

CalibrateCaloCellsTool::~CalibrateCaloCellsTool()
{
}

StatusCode CalibrateCaloCellsTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  info() << "Calibration constant: 1/sampling fraction=" << m_sf << endmsg;
  return sc;
}

void CalibrateCaloCellsTool::Calibrate(std::vector<fcc::CaloHit*>& aCells) {
  //Loop through cells with Geant4 energy deposits, multiply energy to get cell energy at EM scale
  for (auto& ecells : aCells) {
    ecells->Core().Energy *= m_sf;
  }
}

StatusCode CalibrateCaloCellsTool::finalize() {
  StatusCode sc = GaudiTool::finalize();
  return sc;
}
