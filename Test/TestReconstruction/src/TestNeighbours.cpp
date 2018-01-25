#include "TestNeighbours.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

// datamodel
#include "datamodel/CaloHitCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"

DECLARE_ALGORITHM_FACTORY(TestNeighbours)

TestNeighbours::TestNeighbours(const std::string& aName, ISvcLocator* aSvcLoc) : GaudiAlgorithm(aName, aSvcLoc) {
  declareProperty("inhits", m_inHits, "Handle for the EDM Hits to be read");
}

TestNeighbours::~TestNeighbours() {}

StatusCode TestNeighbours::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  m_decoder = std::shared_ptr<DD4hep::DDSegmentation::BitField64>(
      m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation()->decoder());
  m_fieldNames = {"x", "y", "z"};
  info() << "Bitfield: " << m_decoder->fieldDescription() << endmsg;
  // get the minimal and maximal value that can be decoded in the bitfield
  m_fieldExtremes = det::utils::bitfieldExtremes(*m_decoder, m_fieldNames);
  for (uint itField = 0; itField < m_fieldNames.size(); itField++) {
    info() << "Bitfield range for " << m_fieldNames[itField] << " = " << m_fieldExtremes[itField] << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode TestNeighbours::execute() {
  const fcc::CaloHitCollection* inHits = m_inHits.get();
  for (const auto& hit : *inHits) {
    debug() << "cell ID = " << hit.cellId() << endmsg;
    debug() << "energy  = " << hit.energy() << endmsg;
    for (auto& id : det::utils::neighbours(*m_decoder, m_fieldNames, m_fieldExtremes, hit.cellId(), m_steps)) {
      // warning: returned neigbour may not exist!
      // it is possible to decode such a cellID, but maybe it is already outside the detector
      // proper use should include the check on the number of cells, see TestCellCounting
      debug() << "neighbour ID = " << id << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TestNeighbours::finalize() { return GaudiAlgorithm::finalize(); }
