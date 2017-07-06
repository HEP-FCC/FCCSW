#ifndef FWCORE_DATAHANDLE_H
#define FWCORE_DATAHANDLE_H

#include "FWCore/DataWrapper.h"

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/Algorithm.h"
#include <GaudiKernel/DataObjectHandle.h>
#include <GaudiKernel/GaudiException.h>
#include <GaudiKernel/Property.h>
#include <GaudiKernel/ServiceLocatorHelper.h>

template <typename T>
class DataHandle : public DataObjectHandle<DataWrapper<T>> {

public:
  friend class Algorithm;
  friend class AlgTool;

public:
  DataHandle();

  /// Initialises mother class
  DataHandle(DataObjID& descriptor, Gaudi::DataHandle::Mode a, IDataHandleHolder* fatherAlg);

  DataHandle(const std::string& k, Gaudi::DataHandle::Mode a, IDataHandleHolder* fatherAlg);
  /**
   * Retrieve object from transient data store
   */
  const T* get();

  /**
   * Register object in transient store
   */
  void put(T* object);

  /**
  * Create and register object in transient store
  */
  T* createAndPut();

private:
  ServiceHandle<IDataProviderSvc> m_eds;
  bool m_isGoodType{false};
  bool m_isCollection{false};
};

//---------------------------------------------------------------------------
template <typename T>
DataHandle<T>::DataHandle(DataObjID& descriptor, Gaudi::DataHandle::Mode a, IDataHandleHolder* fatherAlg)
    : DataObjectHandle<DataWrapper<T>>(descriptor, a, fatherAlg), m_eds("EventDataSvc", "DataHandle") {}
//---------------------------------------------------------------------------
template <typename T>
DataHandle<T>::DataHandle(const std::string& descriptor, Gaudi::DataHandle::Mode a, IDataHandleHolder* fatherAlg)
    : DataObjectHandle<DataWrapper<T>>(descriptor, a, fatherAlg), m_eds("EventDataSvc", "DataHandle") {}

/**
 * Try to retrieve from the transient store. If the retrieval succeded and
 * this is the first time we retrieve, perform a dynamic cast to the desired
 * object. Then finally set the handle as Read.
 * If this is not the first time we cast and the cast worked, just use the
 * static cast: we do not need the checks of the dynamic cast for every access!
 */
template <typename T>
const T* DataHandle<T>::get() {
  DataObject* dataObjectp = nullptr;
  auto sc = m_eds->retrieveObject(DataObjectHandle<DataWrapper<T>>::fullKey().key(), dataObjectp);

  if (LIKELY(sc.isSuccess())) {
    if (UNLIKELY(!m_isGoodType && !m_isCollection)) {
      // only do this once (if both are false after this, we throw exception)
      m_isGoodType = nullptr != dynamic_cast<DataWrapper<T>*>(dataObjectp);
      if (!m_isGoodType) {
        auto tmp = dynamic_cast<DataWrapper<podio::CollectionBase>*>(dataObjectp);
        if (tmp != nullptr) {
          m_isCollection = nullptr != dynamic_cast<T*>(tmp->collectionBase());
        }
      }
    }
    if (LIKELY(m_isGoodType)) {
      DataObjectHandle<DataWrapper<T>>::setRead();
      return static_cast<DataWrapper<T>*>(dataObjectp)->getData();
    } else if (m_isCollection) {
      // The reader does not know the specific type of the collection. So we need a reinterpret_cast if the handle was
      // created by the reader.
      DataWrapper<podio::CollectionBase>* tmp = static_cast<DataWrapper<podio::CollectionBase>*>(dataObjectp);
      DataObjectHandle<DataWrapper<T>>::setRead();
      return reinterpret_cast<const T*>(tmp->collectionBase());
    } else {
      std::string errorMsg("The type provided for " + DataObjectHandle<DataWrapper<T>>::toString() +
                           " is different from the one of the object in the store.");
      throw GaudiException(errorMsg, "wrong product type", StatusCode::FAILURE);
    }
  }
  std::string msg("Could not retrieve product " + DataObjectHandle<DataWrapper<T>>::toString());
  throw GaudiException(msg, "wrong product name", StatusCode::FAILURE);
}

//---------------------------------------------------------------------------
template <typename T>
void DataHandle<T>::put(T* objectp) {
  DataWrapper<T>* dw = new DataWrapper<T>();
  dw->setData(objectp);
  DataObjectHandle<DataWrapper<T>>::put(dw);
}
//---------------------------------------------------------------------------
/**
 * Create the collection, put it in the DataObjectHandle and return the
 * pointer to the data. Call this function if you create a collection and
 * want to save it.
 */
template <typename T>
T* DataHandle<T>::createAndPut() {
  T* objectp = new T();
  this->put(objectp);
  return objectp;
}

// temporary to allow property declaration
namespace Gaudi {
template <class T>
class Property<::DataHandle<T>&> : public ::DataObjectHandleProperty {
public:
  Property(const std::string& name, ::DataHandle<T>& value) : ::DataObjectHandleProperty(name, value) {}

  /// virtual Destructor
  virtual ~Property() {}
};
}

#endif
