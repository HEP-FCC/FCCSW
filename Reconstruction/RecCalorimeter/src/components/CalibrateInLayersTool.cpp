#include "CalibrateInLayersTool.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DD4hep/Readout.h"

DECLARE_TOOL_FACTORY(CalibrateInLayersTool)

CalibrateInLayersTool::CalibrateInLayersTool(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent), m_geoSvc("GeoSvc", "CalibrateInLayers") {
  declareInterface<ICalibrateCaloHitsTool>(this);
}

StatusCode CalibrateInLayersTool::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  // check if readout exists
  if (m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout <<" << m_readoutName << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

void CalibrateInLayersTool::calibrate(std::unordered_map<uint64_t, double>& aHits) {
  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
  // Loop through energy deposits, multiply energy to get cell energy at electromagnetic scale
  std::for_each(aHits.begin(), aHits.end(), [this, decoder](std::pair<const uint64_t, double>& p) {
    decoder->setValue(p.first);
    // shift layer id if the numbering does not start at 0
    uint layer = (*decoder)[m_layerFieldName].value() - m_firstLayerId;
    if (layer < m_samplingFraction.size()) {
      p.second /= m_samplingFraction[layer];
    } else {
      p.second /= m_samplingFraction[m_samplingFraction.size() - 1];
      warning() << "Size of sampling fraction values is smaller than the number of existing layers."
                << " Taking the sampling fraction for last layer."
                << " Layer ID: " << layer << endmsg;
    }
  });
}

StatusCode CalibrateInLayersTool::finalize() { return GaudiTool::finalize(); }
