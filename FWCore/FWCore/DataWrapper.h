#ifndef FWCORE_DATAWRAPPER_H
#define FWCORE_DATAWRAPPER_H

//Include files
#include "GaudiKernel/DataObject.h"

template<class T>
class GAUDI_API DataWrapper : public DataObject {
 public:
  DataWrapper() : m_data(nullptr){};
  virtual ~DataWrapper(); 

  const T* getData() {return m_data;} 
  void setData(T* data) {m_data = data;}

 private:
  T* m_data; 

};

template<class T>
DataWrapper<T>::~DataWrapper<T>() {
  if (m_data!=nullptr) delete m_data;
}

#endif
