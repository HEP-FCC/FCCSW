#include "DataObjects/VertexTrackAssociationCollection.h"

VertexTrackAssociationCollection::VertexTrackAssociationCollection() : m_collectionID(0), m_data(new VertexTrackAssociationVector() ){
}

const VertexTrackAssociationHandle& VertexTrackAssociationCollection::get(int index) const{
  return m_handles[index];
}

VertexTrackAssociationHandle& VertexTrackAssociationCollection::create(){
  m_data->emplace_back(VertexTrackAssociation());
  int index = m_data->size()-1;
  m_handles.emplace_back(VertexTrackAssociationHandle(index,m_collectionID, m_data));
  auto& tmp_handle = m_handles.back();

  return tmp_handle;
}

void VertexTrackAssociationCollection::clear(){
  m_data->clear();
  m_handles.clear();

}

void VertexTrackAssociationCollection::prepareForWrite(const albers::Registry* registry){
  for(auto& data : *m_data){
     data.Track.prepareForWrite(registry);
    data.Vertex.prepareForWrite(registry);
  }
}

void VertexTrackAssociationCollection::prepareAfterRead(albers::Registry* registry){
  m_handles.clear();
  int index = 0;
  // fix. otherwise, m_collectionID == 0..
  m_collectionID = registry->getIDFromPODAddress( _getBuffer() );
  for (auto& data : *m_data){
    data.Track.prepareAfterRead(registry);
data.Vertex.prepareAfterRead(registry);

    m_handles.emplace_back(VertexTrackAssociationHandle(index,m_collectionID, m_data));
    ++index;
  }
}


void VertexTrackAssociationCollection::setPODsAddress(const void* address){
  m_data = (VertexTrackAssociationVector*)address;
}


const VertexTrackAssociationHandle VertexTrackAssociationCollectionIterator::operator* () const {
  return m_collection->get(m_index);
}

//std::vector<std::pair<std::string,albers::CollectionBase*>>& referenceCollections() {
//}


void VertexTrackAssociationCollection::print() const {
  std::cout<<"collection "<<m_collectionID
           <<", buf "<<m_data
           <<", nhandles "<<m_handles.size()<<std::endl;
}

