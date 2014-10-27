#ifndef DummyDataHANDLE_H
#define DummyDataHANDLE_H
#include "DataObjects/DummyData.h"

#include <vector>

// test data
// author: C. Bernet, B. Hegner

//forward declaration of DummyData container
class DummyDataCollection;

namespace albers {
  class Registry;
}

class DummyDataHandle {

  friend DummyDataCollection;

public:

DummyDataHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  DummyDataHandle() = default;

  const int& Number() const;

  void setNumber(int value);



  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

private:
  DummyDataHandle(int index, int containerID,  std::vector<DummyData>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<DummyData>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
