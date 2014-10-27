// albers specific includes
#include "albers/Reader.h"
#include "albers/Registry.h"

#include <cassert>

namespace albers {

  void Registry::doGetPODAddressFromID(unsigned ID, void*& address) const {
    auto result = std::find(begin(m_collectionIDs), end(m_collectionIDs), ID);
    if (result == end(m_collectionIDs)){
      address = nullptr;
    } else {
      auto index = result - m_collectionIDs.begin();
      address = m_addresses[index];
      if (address == nullptr){
	// this is needed to set the address of handles embedded in other objects
	// std::cout << "On demand reading of container with ID " << ID << std::endl;
        address = m_reader->getBuffer(ID);
      }
    }
  }

  void Registry::print() const {
    assert(m_addresses.size() == m_collections.size() &&
	   m_addresses.size() == m_collectionIDs.size() &&
	   m_addresses.size() == m_names.size());
    std::cout<<"Registry"<<std::endl;
    for(unsigned i=0; i<m_names.size(); ++i ) {
      std::cout<<"\t"
	       <<m_names[i]<<" "<<m_collectionIDs[i]<<" "
	       <<m_collections[i]<<" "<<m_addresses[i]<<std::endl;
    }
  }
} // namespace
