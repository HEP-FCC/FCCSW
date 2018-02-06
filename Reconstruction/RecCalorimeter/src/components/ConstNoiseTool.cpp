#include "ConstNoiseTool.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

// DD4hep
#include "DD4hep/Detector.h"

// Root
#include "TFile.h"
#include "TH1F.h"
#include "TMath.h"

DECLARE_TOOL_FACTORY(ConstNoiseTool)

ConstNoiseTool::ConstNoiseTool(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<INoiseConstTool>(this);
}

StatusCode ConstNoiseTool::initialize() {
  /// Noise estimates from Atlas [GeV]
  // set for ECalBarrel system:5
  // set for HCalBarrel system:6
  // set for HCalExtBarrel system:7
  m_systemNoiseConstMap.emplace(5, 0.0075 / 4.);
  m_systemNoiseConstMap.emplace(8, 0.0115 / 4.);
  m_systemNoiseConstMap.emplace(9, 0.0115 / 4.);

  // Get GeoSvc
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  // Get PhiEta segmentation
  m_segmentation = dynamic_cast<dd4hep::DDSegmentation::FCCSWGridPhiEta*>(
      m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if (m_segmentation == nullptr) {
    error() << "There is no phi-eta segmentation." << endmsg;
    return StatusCode::FAILURE;
  }
  // Take readout bitfield decoder from GeoSvc
  m_decoder = std::shared_ptr<dd4hep::DDSegmentation::BitField64>(
								  m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder());
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  return sc;
}

StatusCode ConstNoiseTool::finalize() {
  StatusCode sc = GaudiTool::finalize();
  return sc;
}

double ConstNoiseTool::getNoiseConstantPerCell(int64_t aCellId) {

  double Noise = 0.;

  // Get cells global coordinate "system"
  m_decoder->setValue(aCellId);
  unsigned cellSystem = (*m_decoder)[m_systemFieldName];
  // cell noise in system
  if (m_systemNoiseConstMap[cellSystem])
    Noise = m_systemNoiseConstMap[cellSystem];
  else
    warning() << "No noise constants set for this subsystem! Noise of cell set to 0. " << endmsg;
  return Noise;
}
