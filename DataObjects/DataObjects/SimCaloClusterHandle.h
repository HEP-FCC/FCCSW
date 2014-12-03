#ifndef SimCaloClusterHANDLE_H
#define SimCaloClusterHANDLE_H
#include "DataObjects/SimCaloCluster.h"
#include "DataObjects/BareCluster.h"

#include <vector>

// contains basic cluster information.
// author: C. Bernet, B. Hegner

//forward declaration of SimCaloCluster container
class SimCaloClusterCollection;

namespace albers {
  class Registry;
}

class SimCaloClusterHandle {

  friend class SimCaloClusterCollection;

public:

SimCaloClusterHandle(){};

//TODO: Proper syntax to use, but ROOT doesn't handle it:  SimCaloClusterHandle() = default;

  const BareCluster& Core() const;

  void setCore(BareCluster value);



  bool isAvailable() const; // precheck whether the pointee actually exists
  void prepareForWrite(const albers::Registry*);  // use m_container to set m_containerID properly
  void prepareAfterRead(albers::Registry*);   // use m_containerID to set m_container properly

  /// equality operator (true if both the index and the container ID are equal)
  bool operator==(const SimCaloClusterHandle& other) const {
       return (m_index==other.m_index) && (other.m_containerID==other.m_containerID);
  }

  /// less comparison operator, so that Handles can be e.g. stored in sets.
  friend bool operator< (const SimCaloClusterHandle& p1,
			 const SimCaloClusterHandle& p2 );

private:
  SimCaloClusterHandle(int index, int containerID,  std::vector<SimCaloCluster>* container);
  int m_index;
  int m_containerID;
  mutable std::vector<SimCaloCluster>* m_container; //! transient
  albers::Registry* m_registry; //! transient
//  bool _retrieveData();
  // members to support 1-to-N relations
  

};

#endif
