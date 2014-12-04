#include "DataObjects/VertexCollection.h"

VertexCollection::VertexCollection() : m_collectionID(0), m_data(new VertexVector() ){
}

const VertexHandle& VertexCollection::get(int index) const{
  return m_handles[index];
}

VertexHandle& VertexCollection::create(){
  m_data->emplace_back(Vertex());
  int index = m_data->size()-1;
  m_handles.emplace_back(VertexHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void VertexCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void VertexCollection::prepareForWrite(const albers::Registry* registry){

}

void VertexCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    
    m_handles.emplace_back(VertexHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void VertexCollection::setPODsAddress(const void* address){
  m_data = (VertexVector*)address;
}


const VertexHandle VertexCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void VertexCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

