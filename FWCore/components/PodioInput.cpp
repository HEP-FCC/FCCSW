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
  //declareOutput("trackHits", m_trackHits,"hits/trackerHits");
  declareOutput("trackHitsClusters", m_trackHitsClusters,"hits/trackerAssociations");
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
  fcc::TrackCluster cluster = 0;
  const fcc::TrackClusterCollection* clusters = nullptr;
  bool is_available = m_store.get("clusters",clusters);
  debug() << "clusters available: " << is_available << endmsg;
  if(is_available){
    for (auto cluster = clusters->begin(); cluster != clusters->end(); ++cluster) {
      debug() << "Cluster has an energy of " << (*cluster).Core().Energy << endmsg;
    }
      m_store.clear();
      m_reader.endOfEvent();
      debug() << "End of event" << endmsg;
      fcc::TrackClusterCollection* new_clusters = new fcc::TrackClusterCollection;
      *new_clusters = *clusters;
      m_trackClusters.put(new_clusters); 
  }
  /*
  is_available = m_store.get("hitClusterAssociation", clusterHitsAssociations);
  if(is_available) {
    m_trackHitsClusters.put(clusterHitsAssociations); 
  }
  */
  return StatusCode::SUCCESS;
}

StatusCode PodioInput::finalize() {
  if (GaudiAlgorithm::finalize().isFailure())
    return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}
