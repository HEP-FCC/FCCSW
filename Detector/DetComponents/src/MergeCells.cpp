#include "MergeCells.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// datamodel
#include "datamodel/CaloHitCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"

DECLARE_ALGORITHM_FACTORY(MergeCells)

MergeCells::MergeCells(const std::string& aName, ISvcLocator* aSvcLoc) : GaudiAlgorithm(aName, aSvcLoc) {
  declareProperty("inhits", m_inHits, "Hit collection to merge (input)");
  declareProperty("outhits", m_outHits, "Merged hit collection (output)");
}

MergeCells::~MergeCells() {}

StatusCode MergeCells::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) return StatusCode::FAILURE;
  if (m_idToMerge.empty()) {
    error() << "No identifier to merge specified." << endmsg;
    return StatusCode::FAILURE;
  }
  m_geoSvc = service("GeoSvc");
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
                                   [this](const std::pair<std::string, DD4hep::Geometry::IDDescriptor::Field>& field) {
                                     return bool(field.first.compare(m_idToMerge) == 0);
                                   });
  if (itIdentifier == m_descriptor.fields().end()) {
    error() << "Identifier  << " << m_idToMerge << ">> does not exist in the readout <<" << m_readoutName << ">>"
            << endmsg;
    return StatusCode::FAILURE;
  }
  // check if proper number of cells to be merged was given (>1 and odd for signed fields)
  // it'd be nice to get max num of cells and warn user if it's not multipicity
  if (m_numToMerge > std::pow(2, (*itIdentifier).second->width())) {
    error() << "It is not possible to merge more cells than the maximum number of cells." << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_numToMerge < 2u) {
    error() << "Number of cells to me merged must be larger than 1." << endmsg;
    return StatusCode::FAILURE;
  }
  if ((*itIdentifier).second->isSigned() && (m_numToMerge % 2 == 0)) {
    error() << "If field is signed, merge can only be done for an odd number of cells"
            << "(to ensure that middle cell is centred at 0)." << endmsg;
    return StatusCode::FAILURE;
  }
  info() << "Field description: " << m_descriptor.fieldDescription() << endmsg;
  info() << "Merging cells for identifier: " << m_idToMerge << endmsg;
  info() << "Number of adjacent cells to be merged: " << m_numToMerge << "\n" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode MergeCells::execute() {
  const auto inHits = m_inHits.get();
  auto outHits = new fcc::CaloHitCollection();

  uint field_id = m_descriptor.fieldID(m_idToMerge);
  auto decoder = m_descriptor.decoder();
  uint64_t cellId = 0;
  int value = 0;
  uint debugIter = 0;

  for (const auto& hit : *inHits) {
    fcc::CaloHit newHit = outHits->create(hit.core());
    cellId = hit.cellId();
    decoder->setValue(cellId);
    value = (*decoder)[field_id].value();
    if (debugIter < m_debugPrint) {
      debug() << "old ID = " << value << endmsg;
    }
    if ((*decoder)[field_id].isSigned()) {
      if (value < 0) {
        value -= m_numToMerge / 2;
      } else {
        value += m_numToMerge / 2;
      }
    }
    value /= int(m_numToMerge);
    if (debugIter < m_debugPrint) {
      debug() << "new ID = " << value << endmsg;
      debugIter++;
    }
    (*decoder)[field_id] = value;
    newHit.cellId(decoder->getValue());
  }
  m_outHits.put(outHits);

  return StatusCode::SUCCESS;
}

StatusCode MergeCells::finalize() { return GaudiAlgorithm::finalize(); }
