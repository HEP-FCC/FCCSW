#ifndef CaloClusterHANDLE_H
#define CaloClusterHANDLE_H
#include "DataObjects/CaloCluster.h"
#include "DataObjects/BareCluster.h"

#include <vector>

// contains basic cluster information
// author: C. Bernet, B. Hegner

//forward declaration of CaloCluster container
class CaloClusterCollection;

namespace albers {
  class Registry;
}

class CaloClusterHandle {

  friend class CaloClusterCollection;

public:

  CaloClusterHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  CaloClusterHandle() = default;



  // precheck whether the pointee actually exists
  bool isAvailable() const;

  // returns a const (read-only) reference to the object pointed by the Handle.
  const CaloCluster& read() const {return m_container->at(m_index);}

  // returns a non-const (writeable) reference to the object pointed by the Handle 
  CaloCluster& mod() {return m_container->at(m_index);}
  
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const CaloClusterHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const CaloClusterHandle& p1,
			 const CaloClusterHandle& p2 );

private:
  CaloClusterHandle(int index, int containerID,  std::vector<CaloCluster>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<CaloCluster>* m_container; //! transient
  albers::Registry* m_registry; //! transient
  //  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
