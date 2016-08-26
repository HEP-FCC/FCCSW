
#include "DD4hep/Printout.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Factories.h"

#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

#include "datamodel/TrackClusterCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/CaloClusterCollection.h"
#include "datamodel/CaloHitCollection.h"
#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"

#include "TSystem.h"
#include "TGMsgBox.h"

#include <stdexcept>
#include <climits>

#include "FCCEventHandler.h"

using DD4hep::DDEveParticleActor;
using DD4hep::DDEveHitActor;
using DD4hep::DDEveHit;


static void* _createFCCEventHandler(const char*) {
  vis::FCCEventHandler* h = new vis::FCCEventHandler();
  return h;
}

DECLARE_CONSTRUCTOR(DD4hep_FCCEventHandler, _createFCCEventHandler)

namespace vis {

/// Standard constructor
FCCEventHandler::FCCEventHandler() : EventHandler() {}

/// Default destructor
FCCEventHandler::~FCCEventHandler() {}

/// Access the number of events on the current input data source (-1 if no data source connected)
long FCCEventHandler::numEvents() const {
  if (m_hasFile) {
    return m_ROOTReader.getEntries();
  }
  return -1;
}

/// Access to the collection type by name
DD4hep::EventHandler::CollectionType FCCEventHandler::collectionType(const std::string& cl) const {
  if (cl == "caloClusters")
    return CALO_HIT_COLLECTION;
  else if (cl == "clusters")
    return TRACKER_HIT_COLLECTION;
  else if (cl == "allGenParticles")
    return PARTICLE_COLLECTION;
  else
    return NO_COLLECTION;
}

template<typename T>
void clusterEveConversion(T& clustercoll, DDEveHitActor& actor) {
  int collsize = clustercoll->size();
  actor.setSize(collsize);
  for (const auto& cluster : *clustercoll) {
    fcc::BareCluster core = cluster.Core();
    // hardcoded first argument is an unused pdgid
    DDEveHit* hit = new DDEveHit(11, core.position.X, core.position.Y, core.position.Z, core.Energy);
    actor(*hit);
  }
}


/// Call functor on hit collection
size_t FCCEventHandler::collectionLoop(const std::string& collection, DDEveHitActor& actor) {
  /// TODO: avoid code duplication
  const podio::CollectionBase* collBase(nullptr);
  const fcc::TrackClusterCollection* clustercoll(nullptr);
  bool clusterPresent = m_podioStore.get(collection, collBase);
  if (clusterPresent) {
    const fcc::TrackClusterCollection* clustercoll = dynamic_cast<const fcc::TrackClusterCollection*>(collBase);
    std::cout<<"cluster collection pointer " << clustercoll << std::endl;
    if (nullptr != clustercoll) {
      int collsize = clustercoll->size();
      m_data[collection].back().second = collsize; 
      clusterEveConversion(clustercoll, actor);
      return collsize;
    } 
    const fcc::CaloClusterCollection* caloclustercoll = dynamic_cast<const fcc::CaloClusterCollection*>(collBase);
    if (nullptr != caloclustercoll) {
      int collsize = caloclustercoll->size();
      m_data[collection].back().second = collsize; 
      clusterEveConversion(caloclustercoll, actor);
      return collsize;
    } 
  }
  return 0;
}

/// Loop over collection and extract particle data
size_t FCCEventHandler::collectionLoop(const std::string& collection, DDEveParticleActor& actor) {
  ///TODO: implement (reconstructed particles not available in FCCSW yet) 
  return 0;
}

/// Open new data file
bool FCCEventHandler::Open(const std::string&, const std::string& name) {
  m_hasFile = false;
  m_hasEvent = true;
  m_ROOTReader.openFile(name);
  m_podioStore.setReader(&m_ROOTReader);
  m_hasFile = true;
  return true;
}

/// Load the next event
bool FCCEventHandler::NextEvent() {
  if (m_hasEvent) {
    m_podioStore.clear();
    m_ROOTReader.endOfEvent();
  }
  m_hasEvent = true;
  m_data.clear();
  /// TODO: move to configuration
  m_data["clusters"].push_back(std::make_pair("clusters", 1));
  m_data["caloClusters"].push_back(std::make_pair("caloClusters", 1));
  return 1;
}

/// Load the previous event
bool FCCEventHandler::PreviousEvent() {
  throw std::runtime_error("+++ This version of the reader can only access files sequentially!\n"
                           "+++ Access to the previous event is not supported.");
}

/// Goto a specified event in the file
bool FCCEventHandler::GotoEvent(long /* event_number */) {
  throw std::runtime_error("+++ This version of the reader can only access files sequentially!\n"
                           "+++ Random access is not supported.");
}

}
