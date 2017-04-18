
#include "DD4hep/Factories.h"
#include "DD4hep/Objects.h"
#include "DD4hep/Printout.h"

#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

#include "datamodel/CaloHitCollection.h"
#include "datamodel/GenVertexCollection.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"

#include "TGMsgBox.h"
#include "TSystem.h"

#include <climits>
#include <stdexcept>

#include "FCCEventHandler.h"

using DD4hep::DDEveParticleActor;
using DD4hep::DDEveHitActor;
using DD4hep::DDEveHit;

static void* _createFCCEventHandler(const char*) {
  vis::FCCEventHandler* h = new vis::FCCEventHandler();
  return h;
}

DECLARE_CONSTRUCTOR(DDEve_FCCEventHandler, _createFCCEventHandler)

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
  if (cl == "caloPositionedHits")
    return CALO_HIT_COLLECTION;
  else if (cl == "positionedHits")
    return TRACKER_HIT_COLLECTION;
  else if (cl == "allGenParticles")
    return PARTICLE_COLLECTION;
  else
    return NO_COLLECTION;
}

template <typename T>
void clusterEveConversion(T& hitcoll, DDEveHitActor& actor) {
  int collsize = hitcoll->size();
  actor.setSize(collsize);
  for (const auto& fcchit : *hitcoll) {
    fcc::Point pos = fcchit.position();
    // hardcoded first argument is an unused pdgid
    DDEveHit* hit = new DDEveHit(11, pos.x, pos.y, pos.z, fcchit.energy());
    actor(*hit);
  }
}

/// Call functor on hit collection
size_t FCCEventHandler::collectionLoop(const std::string& collection, DDEveHitActor& actor) {
  const podio::CollectionBase* collBase(nullptr);
  bool clusterPresent = m_podioStore.get(collection, collBase);
  if (clusterPresent) {
    const fcc::PositionedTrackHitCollection* trackHitColl =
        dynamic_cast<const fcc::PositionedTrackHitCollection*>(collBase);
    if (nullptr != trackHitColl) {
      int collsize = trackHitColl->size();
      m_data[collection].back().second = collsize;
      clusterEveConversion(trackHitColl, actor);
      return collsize;
    }
    const fcc::PositionedCaloHitCollection* caloHitColl =
        dynamic_cast<const fcc::PositionedCaloHitCollection*>(collBase);
    if (nullptr != caloHitColl) {
      int collsize = caloHitColl->size();
      m_data[collection].back().second = collsize;
      clusterEveConversion(caloHitColl, actor);
      return collsize;
    }
  }
  return 0;
}

/// Loop over collection and extract particle data
size_t FCCEventHandler::collectionLoop(const std::string&, DDEveParticleActor&) {
  /// TODO: implement (reconstructed particles not available in FCCSW yet)
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
