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

};

//---------------------------------------------------------------------------
template<typename T>
DataHandle<T>::DataHandle() {}

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
  return DataObjectHandle<DataWrapper<T> >::get()->getData();
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
