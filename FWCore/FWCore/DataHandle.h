#ifndef FWCORE_DATAHANDLE_H
#define FWCORE_DATAHANDLE_H

#include "FWCore/DataWrapper.h"
#include "FWCore/PodioDataSvc.h"

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/Algorithm.h"
#include <GaudiKernel/DataObjectHandle.h>
#include <GaudiKernel/GaudiException.h>
#include <GaudiKernel/Property.h>
#include <GaudiKernel/ServiceLocatorHelper.h>

#include "TTree.h"

#include <type_traits>

/**
 * Specialisation of the Gaudi DataHandle
 * for use with podio collections.
 */ 
template <typename T>
class DataHandle : public DataObjectHandle<DataWrapper<T>> {

public:
  friend class Algorithm;
  friend class AlgTool;

public:
  DataHandle();
  ~DataHandle();

  /// Initialises mother class
  DataHandle(DataObjID& descriptor, Gaudi::DataHandle::Mode a, IDataHandleHolder* fatherAlg);

  DataHandle(const std::string& k, Gaudi::DataHandle::Mode a, IDataHandleHolder* fatherAlg);
   
   ///Retrieve object from transient data store
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
  T* m_dataPtr;
};

template <typename T>
DataHandle<T>::~DataHandle() {
  // release memory allocated for primitive types (see comments in ctor)
  if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
    delete m_dataPtr;
  }
}

//---------------------------------------------------------------------------
template <typename T>
DataHandle<T>::DataHandle(DataObjID& descriptor, Gaudi::DataHandle::Mode a, IDataHandleHolder* fatherAlg)
    : DataObjectHandle<DataWrapper<T>>(descriptor, a, fatherAlg), m_eds("EventDataSvc", "DataHandle") {
      
}
/// The DataHandle::Writer constructor is used to create the corresponding branch in the output file
template <typename T>
DataHandle<T>::DataHandle(const std::string& descriptor, Gaudi::DataHandle::Mode a, IDataHandleHolder* fatherAlg)
    : DataObjectHandle<DataWrapper<T>>(descriptor, a, fatherAlg), m_eds("EventDataSvc", "DataHandle") {

  if (a == Gaudi::DataHandle::Writer) {
    m_eds.retrieve();
    PodioDataSvc* pds;
    pds = dynamic_cast<PodioDataSvc*>( m_eds.get());
    m_dataPtr = nullptr;
  if (nullptr != pds) {
    if (std::is_convertible<T*,podio::CollectionBase*>::value) {
       // case 1: T is a podio collection
       // for this case creation of branches is still handled in PodioOutput
       // (but could be moved here in the future) 
    } else if constexpr (std::is_integral_v<T>) {
       // case 2: T is some integer type
       // the call signature for TTree Branch is different for primitive types
       // in particular, we pass the pointer, not the adress of the pointer
       // and have to append a char indicating type (see TTree documentation)
       // therefore  space needs to be allocated for the integer 
       m_dataPtr = new T();
       TTree* tree = pds->eventDataTree();
       tree->Branch(descriptor.c_str(),  m_dataPtr, (descriptor + "/I").c_str());
    } else if constexpr (std::is_floating_point_v<T>) {
       // case 3: T is some floating point type
       // similar case 2, distinguish floats and doubles by size
       m_dataPtr = new T();
       TTree* tree = pds->eventDataTree();
       if (sizeof(T) > 4) {
         tree->Branch(descriptor.c_str(),  m_dataPtr, (descriptor + "/D").c_str());
       } else {
         tree->Branch(descriptor.c_str(),  m_dataPtr, (descriptor + "/F").c_str());
       }
    } else {
       // case 4: T is any other type (for which exists a root dictionary,
       // otherwise i/o will fail)
       // this includes std::vectors of ints, floats
       TTree* tree = pds->eventDataTree();
       tree->Branch(descriptor.c_str(),  &m_dataPtr);
      }
    }
  }
}

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
  // in case T is of primitive type, we must not change the pointer address
  // (see comments in ctor) instead copy the value of T into allocated memory
  if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
    *m_dataPtr = *objectp;
  } else {
    m_dataPtr = objectp;
  }
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
