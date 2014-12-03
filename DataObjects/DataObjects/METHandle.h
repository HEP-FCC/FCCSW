#ifndef METHANDLE_H
#define METHANDLE_H
#include "DataObjects/MET.h"

#include <vector>

// 
// author: C. Bernet, B. Hegner

//forward declaration of MET container
class METCollection;

namespace albers {
  class Registry;
}

class METHandle {

  friend class METCollection;

public:

METHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  METHandle() = default;

  const float& Pt() const;
  const float& Phi() const;

  void setPt(float value);
  void setPhi(float value);



  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const METHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const METHandle& p1,
			 const METHandle& p2 );

private:
  METHandle(int index, int containerID,  std::vector<MET>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<MET>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
