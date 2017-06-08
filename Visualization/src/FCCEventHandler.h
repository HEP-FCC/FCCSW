#ifndef DD4HEP_DDEVE_FCCEVENTHANDLER_H
#define DD4HEP_DDEVE_FCCEVENTHANDLER_H

// Framework include files
#include "DDEve/EventHandler.h"

#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

#include <map>
#include <string>

namespace vis {

class FCCEventHandler : public DD4hep::EventHandler {
private:
  podio::EventStore m_podioStore = podio::EventStore();
  podio::ROOTReader m_ROOTReader = podio::ROOTReader();

  std::string m_fileName;
  /// Data collection map
  TypedEventCollections m_data;

public:
  /// Standard constructor
  FCCEventHandler();
  /// Default destructor
  virtual ~FCCEventHandler();

  /// Access the map of simulation data collections
  const TypedEventCollections& data() const override { return m_data; }
  /// Access the data source name
  std::string datasourceName() const override { return hasFile() ? m_fileName : std::string("UNKNOWN"); }
  /// Access the number of events on the current input data source (-1 if no data source connected)
  long numEvents() const override;
  /// Access to the collection type by name
  CollectionType collectionType(const std::string& collection) const override;
  /// Call functor on hit collection
  size_t collectionLoop(const std::string& collection, DD4hep::DDEveHitActor& actor) override;
  /// Loop over collection and extract particle data
  size_t collectionLoop(const std::string& collection, DD4hep::DDEveParticleActor& actor) override;
  /// Open new data file
  bool Open(const std::string& type, const std::string& file_name) override;
  /// User overloadable function: Load the next event
  bool NextEvent() override;
  /// User overloadable function: Load the previous event
  bool PreviousEvent() override;
  /// Goto a specified event in the file
  bool GotoEvent(long event_number) override;
};

} /* End namespace vis   */

#endif /* DD4HEP_DDEVE_FCCEVENTHANDLER_H */
