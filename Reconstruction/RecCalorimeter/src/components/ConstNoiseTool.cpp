#include "ConstNoiseTool.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

// DD4hep
#include "DD4hep/Detector.h"

DECLARE_TOOL_FACTORY(ConstNoiseTool)

ConstNoiseTool::ConstNoiseTool(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
   declareInterface<INoiseConstTool>(this);
}

StatusCode ConstNoiseTool::initialize() {
  // set for ECalBarrel system:5
  // set for ECalEndcap system:6
  // set for HCalEndcap system:7
  // set for HCalBarrel system:8
  // set for HCalExtBarrel system:9
  // set for ECalFwd system:10
  // set for HCalFwd system:11

  m_systemNoiseConstMap.emplace(5, m_ECalBThreshold );
  m_systemNoiseConstMap.emplace(6, m_ECalECThreshold  );
  m_systemNoiseConstMap.emplace(7, m_HCalECThreshold );
  m_systemNoiseConstMap.emplace(8, m_HCalBThreshold );
  m_systemNoiseConstMap.emplace(9, m_HCalBThreshold );
  m_systemNoiseConstMap.emplace(10, m_ECalFwdThreshold );
  m_systemNoiseConstMap.emplace(11, m_HCalFwdThreshold );

  m_systemNoiseOffsetMap.emplace(5, 0. );
  m_systemNoiseOffsetMap.emplace(6, 0.  );
  m_systemNoiseOffsetMap.emplace(7, 0. );
  m_systemNoiseOffsetMap.emplace(8, 0.  );
  m_systemNoiseOffsetMap.emplace(9, 0.  );
  m_systemNoiseOffsetMap.emplace(10,0.  );
  m_systemNoiseOffsetMap.emplace(11,0.  );

  // Get GeoSvc
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
 
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  return sc;
}

StatusCode ConstNoiseTool::finalize() {
  StatusCode sc = GaudiTool::finalize();
  return sc;
}

double ConstNoiseTool::getNoiseConstantPerCell(uint64_t aCellId) {

  double Noise = 0.;
  // Get cells global coordinate "system"
  dd4hep::DDSegmentation::CellID cID = aCellId;
  unsigned cellSystem = m_decoder->get(cID, "system");
  // cell noise in system
  if (m_systemNoiseConstMap[cellSystem])
    Noise = m_systemNoiseConstMap[cellSystem];
  else
    warning() << "No noise constants set for this subsystem! Noise of cell set to 0. " << endmsg;
  return Noise;
}

double ConstNoiseTool::getNoiseOffsetPerCell(uint64_t aCellId) {

  double Noise = 0.;
  // Get cells global coordinate "system"
  dd4hep::DDSegmentation::CellID cID = aCellId;
  unsigned cellSystem = m_decoder->get(cID, "system");
  // cell noise in system
  if (m_systemNoiseOffsetMap[cellSystem])
    Noise = m_systemNoiseOffsetMap[cellSystem];
  else
    warning() << "No noise constants set for this subsystem! Noise of cell set to 0. " << endmsg;
  return Noise;
}
