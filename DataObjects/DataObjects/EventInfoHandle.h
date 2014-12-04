#ifndef EventInfoHANDLE_H
#define EventInfoHANDLE_H
#include "DataObjects/EventInfo.h"

#include <vector>

// Event number
// author: C. Bernet, B. Hegner

//forward declaration of EventInfo container
class EventInfoCollection;

namespace albers {
  class Registry;
}

class EventInfoHandle {

  friend class EventInfoCollection;

public:

  EventInfoHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  EventInfoHandle() = default;



  // precheck whether the pointee actually exists
  bool isAvailable() const;

  // returns a const (read-only) reference to the object pointed by the Handle.
  const EventInfo& read() const {return m_container->at(m_index);}

  // returns a non-const (writeable) reference to the object pointed by the Handle 
  EventInfo& mod() {return m_container->at(m_index);}
  
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const EventInfoHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const EventInfoHandle& p1,
			 const EventInfoHandle& p2 );

private:
  EventInfoHandle(int index, int containerID,  std::vector<EventInfo>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<EventInfo>* m_container; //! transient
  albers::Registry* m_registry; //! transient
  //  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
