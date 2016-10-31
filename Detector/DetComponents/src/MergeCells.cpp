#include "MergeCells.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"

// datamodel
#include "datamodel/CaloHitCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"

// ROOT
#include "TGeoManager.h"

DECLARE_ALGORITHM_FACTORY(MergeCells)

MergeCells::MergeCells(const std::string& aName, ISvcLocator* aSvcLoc):
GaudiAlgorithm(aName, aSvcLoc){
  declareInput("inhits", m_inHits,"hits/caloInHits");
  declareOutput("outhits", m_outHits,"hits/caloOutHits");
  declareProperty("readout", m_readoutName);
  declareProperty("identifier", m_idToMerge);
  declareProperty("merge", m_numToMerge = 0);
  declareProperty("mergeList", m_listToMerge);
  declareProperty("volumeMatchName", m_volumeMatchName = "");
}

MergeCells::~MergeCells() {}

StatusCode MergeCells::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;
  if(m_idToMerge.empty()) {
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
  if(m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
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
  if( itIdentifier == m_descriptor.fields().end()) {
    error()<<"Identifier <<"<<m_idToMerge<<">> does not exist in the readout <<"<<m_readoutName<<">>"<<endmsg;
    return StatusCode::FAILURE;
  }
  // check if only one field: either merge or mergeList is specified
  if(m_numToMerge > 0 && m_listToMerge.size() > 0) {
    error()<<"Specify either how many consequitive cells to merge (e.g. merge = 2)\n"
           <<"or the vector of number of cells to merge (e.g. mergeList = [2,2,3])."<<endmsg;
    return StatusCode::FAILURE;
  }
  if(m_numToMerge > 0) {
    // check if proper number of cells to be merged was given (>1 and odd for signed fields)
    // it'd be nice to get max num of cells and warn user if it's not multipicity
    if(m_numToMerge > std::pow(2,(*itIdentifier).second->width())) {
      error()<<"It is not possible to merge more cells than the maximum number of cells."<<endmsg;
      return StatusCode::FAILURE;
    }
    if(m_numToMerge < 2) {
      error()<<"Number of cells to me merged must be larger than 1."<<endmsg;
      return StatusCode::FAILURE;
    }
    if((*itIdentifier).second->isSigned() && (m_numToMerge%2 == 0)) {
      error()<<"If field is signed, merge can only be done for an odd number of cells"
             <<"(to ensure that middle cell is centred at 0)."<<endmsg;
      return StatusCode::FAILURE;
    }
    info()<<"Field description: "<<m_descriptor.fieldDescription()<<endmsg;
    info()<<"Merging cells for identifier: "<<m_idToMerge<<endmsg;
    info()<<"Number of adjacent cells to be merged: "<<m_numToMerge<<"\n"<<endmsg;
  } else if(m_listToMerge.size() > 0) {
    uint sumCells = std::accumulate(m_listToMerge.begin(), m_listToMerge.end(), 0);
    // optional (and recommended) checks
    // check list of cells to be merged - it must sum to the total number of cells
    if(!m_volumeMatchName.empty()) {
      // get the total number of volumes in the geometry
      auto highestVol = gGeoManager->GetTopVolume();
      auto numPlacedVol = det::utils::countPlacedVolumes(highestVol, m_volumeMatchName);
      if(numPlacedVol != sumCells) {
        error()<<"Total number of cells named "<<m_volumeMatchName<<" ("<<numPlacedVol<<") "
               <<"is not equal to the sum of cells from the list given in py config ("<<sumCells<<")"<<endmsg;
        return StatusCode::FAILURE;
      }
    } else {
      warning()<<"Assuming the total number of cells is "<<sumCells<<".\n"
               <<"If there are any cells beyond that, they will be attached to the last merged cell"<<endmsg;
    }
    info()<<"Field description: "<<m_descriptor.fieldDescription()<<endmsg;
    info()<<"Merging cells for identifier: "<<m_idToMerge<<endmsg;
    info()<<"List of number of cells to be merged: "<<m_listToMerge<<"\n"<<endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode MergeCells::execute() {
  const fcc::CaloHitCollection* inHits = m_inHits.get();
  fcc::CaloHitCollection* outHits = new fcc::CaloHitCollection();

  uint field_id = m_descriptor.fieldID(m_idToMerge);
  auto decoder = m_descriptor.decoder();
  uint64_t cellId = 0;
  int value = 0;

  for(const auto& hit: *inHits) {
    fcc::CaloHit newHit = outHits->create();
    newHit.Core().Energy = hit.Core().Energy;
    newHit.Core().Time = hit.Core().Time;
    cellId = hit.Core().Cellid;
    decoder->setValue(cellId);
    int value = (*decoder)[field_id].value();
    debug()<<"old ID = "<<value<<endmsg;
    if((*decoder)[field_id].isSigned()) {
      if(value<0)
        value -= m_numToMerge/2;
      else
        value += m_numToMerge/2;
    }
    value /= int(m_numToMerge);
    debug()<<"new ID = "<<value<<endmsg;
    (*decoder)[field_id] = value;
    newHit.Core().Cellid = decoder->getValue();
  }
  m_outHits.put(outHits);

  return StatusCode::SUCCESS;
}

StatusCode MergeCells::finalize() {
  return GaudiAlgorithm::finalize();
}
