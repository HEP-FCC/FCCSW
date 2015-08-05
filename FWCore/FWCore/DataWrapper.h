#ifndef FWCORE_DATAWRAPPER_H
#define FWCORE_DATAWRAPPER_H

#include <typeinfo>

//Include files
#include "GaudiKernel/DataObject.h"

class GAUDI_API DataWrapperBase : public DataObject {
 public:
   virtual const std::type_info& wrappedTypeID() = 0;
   virtual ~DataWrapperBase(){};
};

template<class T>
class GAUDI_API DataWrapper : public DataWrapperBase {
 public:

  DataWrapper() : DataWrapperBase(), m_data(nullptr){};
  virtual ~DataWrapper(); 

  const T* getData() {return m_data;} 
  void setData(T* data) {m_data = data;}

  const std::type_info& wrappedTypeID() {return typeid(m_data); }

 private:
  T* m_data; 

};

template<class T>
DataWrapper<T>::~DataWrapper<T>() {
  if (m_data!=nullptr) delete m_data;
}

#endif
