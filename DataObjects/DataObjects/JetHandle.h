#ifndef JetHANDLE_H
#define JetHANDLE_H
#include "DataObjects/Jet.h"
#include "DataObjects/BareJet.h"

#include <vector>

// Basic jet information.
// author: C. Bernet, B. Hegner

//forward declaration of Jet container
class JetCollection;

namespace albers {
  class Registry;
}

class JetHandle {

  friend class JetCollection;

public:

  JetHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  JetHandle() = default;



  // precheck whether the pointee actually exists
  bool isAvailable() const;

  // returns a const (read-only) reference to the object pointed by the Handle.
  const Jet& read() const {return m_container->at(m_index);}

  // returns a non-const (writeable) reference to the object pointed by the Handle 
  Jet& mod() {return m_container->at(m_index);}
  
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const JetHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const JetHandle& p1,
			 const JetHandle& p2 );

private:
  JetHandle(int index, int containerID,  std::vector<Jet>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<Jet>* m_container; //! transient
  albers::Registry* m_registry; //! transient
  //  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
