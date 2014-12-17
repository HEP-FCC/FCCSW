#ifndef GenJetHANDLE_H
#define GenJetHANDLE_H
#include "DataObjects/GenJet.h"
#include "DataObjects/BareJet.h"

#include <vector>

// Basic jet information.
// author: C. Bernet, B. Hegner

//forward declaration of GenJet container
class GenJetCollection;

namespace albers {
  class Registry;
}

class GenJetHandle {

  friend class GenJetCollection;

public:

  GenJetHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  GenJetHandle() = default;



  // precheck whether the pointee actually exists
  bool isAvailable() const;

  // returns a const (read-only) reference to the object pointed by the Handle.
  const GenJet& read() const {return m_container->at(m_index);}

  // returns a non-const (writeable) reference to the object pointed by the Handle 
  GenJet& mod() {return m_container->at(m_index);}
  
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const GenJetHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const GenJetHandle& p1,
			 const GenJetHandle& p2 );

private:
  GenJetHandle(int index, int containerID,  std::vector<GenJet>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<GenJet>* m_container; //! transient
  albers::Registry* m_registry; //! transient
  //  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
