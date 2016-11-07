#include "MergeLayers.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"

// datamodel
#include "datamodel/CaloHitCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"

// ROOT
#include "TGeoManager.h"

DECLARE_ALGORITHM_FACTORY(MergeLayers)

MergeLayers::MergeLayers(const std::string& aName, ISvcLocator* aSvcLoc):
GaudiAlgorithm(aName, aSvcLoc){
  declareInput("inhits", m_inHits,"hits/caloInHits");
  declareOutput("outhits", m_outHits,"hits/caloOutHits");
  declareProperty("readout", m_readoutName);
  declareProperty("identifier", m_idToMerge);
  declareProperty("volumeName", m_volumeName = "");
  declareProperty("merge", m_listToMerge);
  declareProperty("debugPrint", m_debugPrint = 10);
}

MergeLayers::~MergeLayers() {}

StatusCode MergeLayers::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;
  if (m_idToMerge.empty()) {
    error()<<"No identifier to merge specified."<<endmsg;
    return StatusCode::FAILURE;
  }
  m_geoSvc = service ("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  // check if readout exists
  if (m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error()<<"Readout <<"<<m_readoutName<<">> does not exist."<<endmsg;
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
  if ( itIdentifier == m_descriptor.fields().end()) {
    error()<<"Identifier <<"<<m_idToMerge<<">> does not exist in the readout <<"<<m_readoutName<<">>"<<endmsg;
    return StatusCode::FAILURE;
  }
  // check sizes of new volumes in the list - it must sum to the total number of volumes
  uint sumCells = std::accumulate(m_listToMerge.begin(), m_listToMerge.end(), 0);
  auto highestVol = gGeoManager->GetTopVolume();
  auto numPlacedVol = det::utils::countPlacedVolumes(highestVol, m_volumeName);
  if (numPlacedVol != sumCells) {
    error()<<"Total number of volumes named "<<m_volumeName<<" ("<<numPlacedVol<<") "
           <<"is not equal to the sum of volumes from the list 'merge' given in job options ("
           <<sumCells<<")"<<endmsg;
    return StatusCode::FAILURE;
  }
  info()<<"Field description: "<<m_descriptor.fieldDescription()<<endmsg;
  info()<<"Merging volumes named: "<<m_volumeName<<endmsg;
  info()<<"Merging volumes for identifier: "<<m_idToMerge<<endmsg;
  info()<<"List of number of volumes to be merged: "<<m_listToMerge<<"\n"<<endmsg;
  return StatusCode::SUCCESS;
}

StatusCode MergeLayers::execute() {
  const auto inHits = m_inHits.get();
  auto outHits = new fcc::CaloHitCollection();

  // rewriting list of cell sizes to list of top boundaries to facilitate the loop over hits
  std::vector<uint> listToMergeBoundary(m_listToMerge.size());
  uint sumCells = 0;
  for (uint i=0; i<m_listToMerge.size(); i++) {
    sumCells += m_listToMerge[i];
    listToMergeBoundary[i] = sumCells;
  }

  uint field_id = m_descriptor.fieldID(m_idToMerge);
  auto decoder = m_descriptor.decoder();
  uint64_t cellId = 0;
  int value = 0;
  uint debugIter = 0;

  for (const auto& hit: *inHits) {
    fcc::CaloHit newHit = outHits->create(hit.core());
    cellId = hit.cellId();
    decoder->setValue(cellId);
    value = (*decoder)[field_id].value();
    if (debugIter < m_debugPrint) {
      debug() << "old ID = " << value << endmsg;
    }
    for (uint i=0; i<listToMergeBoundary.size(); i++) {
      if (value < listToMergeBoundary[i]) {
        value = i;
        break;
      }
    }
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

StatusCode MergeLayers::finalize() {
  return GaudiAlgorithm::finalize();
}
