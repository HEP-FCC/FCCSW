#ifndef ReferencingDataHANDLE_H
#define ReferencingDataHANDLE_H
#include "DataObjects/ReferencingData.h"
#include "DataObjects/DummyDataHandle.h"
#include <vector>
#include "DummyDataHandle.h"

#include <vector>

// ref to dummy data
// author: C. Bernet, B. Hegner

//forward declaration of ReferencingData container
class ReferencingDataCollection;

namespace albers {
  class Registry;
}

class ReferencingDataHandle {

  friend ReferencingDataCollection;

public:

ReferencingDataHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  ReferencingDataHandle() = default;

  const DummyDataHandle& Dummy() const;

  void setDummy(DummyDataHandle value);

  void addDummies(DummyDataHandle&);
  std::vector<DummyDataHandle>::const_iterator Dummies_begin() const;
  std::vector<DummyDataHandle>::const_iterator Dummies_end() const;


  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

private:
  ReferencingDataHandle(int index, int containerID,  std::vector<ReferencingData>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<ReferencingData>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  // members to support 1-to-N relations
  std::vector<DummyDataHandle>* m_Dummies; //! transient 


};

#endif
