#ifndef FWCORE_DATAHANDLE_H
#define FWCORE_DATAHANDLE_H

#include "FWCore/DataWrapper.h"

#include <GaudiKernel/DataObjectHandle.h>
#include <GaudiKernel/ServiceLocatorHelper.h>
#include <GaudiKernel/GaudiException.h>
#include <GaudiKernel/Property.h>
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/AlgTool.h"

template<typename T>
class DataHandle : public DataObjectHandle<DataWrapper<T> > {

public:
	friend class Algorithm;
	friend class AlgTool;

public:

  DataHandle();

  /// Initialises mother class
  DataHandle(DataObjectDescriptor & descriptor,
                   IAlgorithm* fatherAlg);

  /// Initialises mother class
  DataHandle(DataObjectDescriptor & descriptor,
                   IAlgTool* fatherAlg);

  /**
   * Retrieve object from transient data store
   */
  const T* get();

  /**
   * Register object in transient store
   */
  void put (T* object);

  /**
  * Create and register object in transient store
  */
  T* createAndPut();

private:
  /// FIXME: not needed in Gaudi v27r1+ anymore, remove once we migrate.
  // DetaObjectHandle has this as a protected member and gets it from fatherAlg / fatherTool
  ServiceHandle<IDataProviderSvc> m_eds;

};

//---------------------------------------------------------------------------
template<typename T>
DataHandle<T>::DataHandle() : m_eds("EventDataSvc", "DataHandle") {
}
//---------------------------------------------------------------------------

/**
 * Try to retrieve from the transient store. If the retrieval succeded and
 * this is the first time we retrieve, perform a dynamic cast to the desired
 * object. Then finally set the handle as Read.
 * If this is not the first time we cast and the cast worked, just use the
 * static cast: we do not need the checks of the dynamic cast for every access!
 */
template<typename T>
const T* DataHandle<T>::get() {
  // return DataObjectHandle<DataWrapper<T> >::get()->getData();
  DataObject* dataObjectp = nullptr;
  DataHandle<T>::m_eds->retrieveObject(DataObjectHandle<DataWrapper<T> >::dataProductName(), dataObjectp);
  // once we migrate to Gaudi v27r1+ replace with this:
  // DataObjectHandle<DataWrapper<T>>::m_EDS->retrieveObject(DataObjectHandle<DataWrapper<T> >::dataProductName(), 
  //    dataObjectp);

  // The reader does not know the specific type of the collection. So we need a reinterpret_cast if the handle was 
  // created by the reader.
  bool goodType = (nullptr != dynamic_cast<DataWrapper<T>*> (dataObjectp));
  if (goodType) {
    return DataObjectHandle<DataWrapper<T> >::get()->getData();
  } else {
    DataWrapper<podio::CollectionBase>* tmp = static_cast<DataWrapper<podio::CollectionBase>*>(dataObjectp);
    DataObjectHandle<DataWrapper<T> >::setRead();
    return reinterpret_cast<const T*>(tmp->collectionBase());
  }
}

//---------------------------------------------------------------------------
template<typename T>
void DataHandle<T>::put (T *objectp){
    DataWrapper<T>* dw = new DataWrapper<T>();
    dw->setData(objectp);
    DataObjectHandle<DataWrapper<T> >::put(dw);
}
//---------------------------------------------------------------------------
/**
 * Create the collection, put it in the DataObjectHandle and return the
 * pointer to the data. Call this function if you create a collection and
 * want to save it.
 */
template<typename T>
T* DataHandle<T>::createAndPut () {
    T *objectp = new T();
    this->put(objectp);
    return objectp;
}

#endif
