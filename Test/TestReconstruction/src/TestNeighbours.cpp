#include "TestNeighbours.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

// datamodel
#include "datamodel/CaloHitCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"

DECLARE_ALGORITHM_FACTORY(TestNeighbours)

TestNeighbours::TestNeighbours(const std::string& aName, ISvcLocator* aSvcLoc):
  GaudiAlgorithm(aName, aSvcLoc) {
  declareProperty("readout",m_readoutName);
  declareInput("inhits", m_inHits,"hits/caloInHits");
}

TestNeighbours::~TestNeighbours() {}

StatusCode TestNeighbours::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_geoSvc = service ("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  m_decoder = std::shared_ptr<DD4hep::DDSegmentation::BitField64>(m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation()->decoder());
  info() << "Bitfield: "<<m_decoder->fieldDescription() << endmsg;
  auto extremes = det::utils::bitfieldExtremes(*m_decoder);
  info()<<"Bitfield range for system = "<<extremes[m_decoder->index("system")]<<endmsg;
  info()<<"Bitfield range for x = "<<extremes[m_decoder->index("x")]<<endmsg;
  info()<<"Bitfield range for y = "<<extremes[m_decoder->index("y")]<<endmsg;
  info()<<"Bitfield range for z = "<<extremes[m_decoder->index("z")]<<endmsg;
  return StatusCode::SUCCESS;
}

StatusCode TestNeighbours::execute() {
  const fcc::CaloHitCollection* inHits = m_inHits.get();
  for(const auto& hit: *inHits) {
    debug() << "cell ID =" << hit.Core().Cellid << endmsg;
    debug() << "energy  =" << hit.Core().Energy << endmsg;
    for(auto& id: det::utils::neighbours(*m_decoder, {"x", "y", "z"}, hit.Core().Cellid)) {
      // warning: returned neigbour does not need to exist!
      // it is possible to decode such a cellID, but maybe it is already outside the detector
      // proper use should include the check on the number of cells, see TestCellCounting
      debug() << "neighbour: ID = " << id << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode TestNeighbours::finalize() {
  return GaudiAlgorithm::finalize();
}
