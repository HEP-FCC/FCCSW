#include "MergeLayers.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

// datamodel
#include "datamodel/CaloHitCollection.h"

// DD4hep
#include "DD4hep/Detector.h"

// ROOT
#include "TGeoManager.h"

// STL
#include <numeric>

DECLARE_COMPONENT(MergeLayers)

MergeLayers::MergeLayers(const std::string& aName, ISvcLocator* aSvcLoc) : GaudiAlgorithm(aName, aSvcLoc), m_geoSvc("GeoSvc", aName) {
  declareProperty("inhits", m_inHits, "Hit collection to merge (input)");
  declareProperty("outhits", m_outHits, "Merged hit collection (output)");
}

MergeLayers::~MergeLayers() {}

StatusCode MergeLayers::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) return StatusCode::FAILURE;
  if (m_idToMerge.empty()) {
    error() << "No identifier to merge specified." << endmsg;
    return StatusCode::FAILURE;
  }
  
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  // check if readout exists
  if (m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout <<" << m_readoutName << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  auto readout = m_geoSvc->lcdd()->readout(m_readoutName);
  m_descriptor = readout.idSpec();
  // check if identifier exists in the decoder
  auto itIdentifier = std::find_if(m_descriptor.fields().begin(),
                                   m_descriptor.fields().end(),
                                   [this](const std::pair<std::string, const dd4hep::BitFieldElement*>& field) {
                                     return bool(field.first.compare(m_idToMerge) == 0);
                                   });
  if (itIdentifier == m_descriptor.fields().end()) {
    error() << "Identifier <<" << m_idToMerge << ">> does not exist in the readout <<" << m_readoutName << ">>"
            << endmsg;
    return StatusCode::FAILURE;
  }
  info() << "Field description: " << m_descriptor.fieldDescription() << endmsg;
  info() << "Merging volumes named: " << m_volumeName << endmsg;
  info() << "Merging volumes for identifier: " << m_idToMerge << endmsg;
  info() << "List of number of volumes to be merged: " << m_listToMerge << "\n" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode MergeLayers::execute() {
  const auto inHits = m_inHits.get();
  auto outHits = new fcc::CaloHitCollection();

  // rewriting list of cell sizes to list of top boundaries to facilitate the loop over hits
  std::vector<unsigned int> listToMergeBoundary(m_listToMerge.size());
  unsigned int sumCells = 0;
  for (unsigned int i = 0; i < m_listToMerge.size(); i++) {
    sumCells += m_listToMerge[i];
    listToMergeBoundary[i] = sumCells;
  }

  unsigned int field_id = m_descriptor.fieldID(m_idToMerge);
  auto decoder = m_descriptor.decoder();
  dd4hep::DDSegmentation::CellID cellId = 0;
  unsigned int value = 0;
  unsigned int debugIter = 0;

  for (const auto& hit : *inHits) {
    fcc::CaloHit newHit = outHits->create(hit.core());
    cellId = hit.cellId();
    value = decoder->get(cellId, field_id);
    if (debugIter < m_debugPrint) {
      debug() << "old ID = " << value << endmsg;
    }
    for (unsigned int i = 0; i < listToMergeBoundary.size(); i++) {
      if (value < listToMergeBoundary[i]) {
        value = i;
        break;
      }
    }
    if (debugIter < m_debugPrint) {
      debug() << "new ID = " << value << endmsg;
      debugIter++;
    }
    decoder->set(cellId, field_id, value);
    newHit.cellId(cellId);
  }
  m_outHits.put(outHits);

  return StatusCode::SUCCESS;
}

StatusCode MergeLayers::finalize() { return GaudiAlgorithm::finalize(); }
