#include "PodioInput.h"
#include "FWCore/PodioDataSvc.h"



DECLARE_COMPONENT(PodioInput)

PodioInput::PodioInput(const std::string& name, ISvcLocator* svcLoc) :
GaudiAlgorithm(name, svcLoc),m_first(true)
{
  declareProperty("filename", m_filename="input.root",
                  "Name of the file to read");
  std::vector<std::string> defaultCommands;
  defaultCommands.push_back("keep *");
  declareProperty("inputCommands", m_inputCommands=defaultCommands,
                  "A set of commands to declare which collections to keep or drop.");
  declareOutput("trackClusters", m_trackClusters,"hits/trackerClusters");
  declareOutput("trackHits", m_trackHits,"hits/trackerHits");
  declareOutput("trackHitsClusters", m_trackClusterHitsAssociations,"hits/trackerAssociations");
}

StatusCode PodioInput::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;
  // check whether we have the PodioEvtSvc active
  m_podioDataSvc = dynamic_cast<PodioDataSvc*>(evtSvc().get());
  if (nullptr == m_podioDataSvc) return StatusCode::FAILURE;
  m_switch = KeepDropSwitch(m_inputCommands);
  m_reader.openFile("input.root");
  m_store.setReader(&m_reader);
  return StatusCode::SUCCESS;
}

StatusCode PodioInput::execute() {
  collFromStoreToDataService<fcc::TrackClusterCollection>("clusters", m_trackClusters);
  collFromStoreToDataService<fcc::TrackHitCollection>("hits", m_trackHits);
  m_reader.endOfEvent();
  return StatusCode::SUCCESS;
}

StatusCode PodioInput::finalize() {
  m_store.clear();
  if (GaudiAlgorithm::finalize().isFailure())
    return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

template<typename T>
StatusCode PodioInput::collFromStoreToDataService(const std::string& collection_name, DataHandle<T>& data_handle) {
bool is_available;
  const T* collection(nullptr);
  is_available = m_store.get(collection_name, collection);
  if (is_available) {
    debug() << "Reading " << collection_name << " from " << m_filename << "." << endmsg;
    // ownership given to data service
    T* tmp_collection = new T;
    *tmp_collection = *collection;
    data_handle.put(tmp_collection);
  }
  return StatusCode::SUCCESS;
}

