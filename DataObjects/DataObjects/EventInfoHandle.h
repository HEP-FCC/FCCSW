#ifndef EventInfoHANDLE_H
#define EventInfoHANDLE_H
#include "DataObjects/EventInfo.h"

#include <vector>

// 
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

  const int& Number() const;

  void setNumber(int value);



  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

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
