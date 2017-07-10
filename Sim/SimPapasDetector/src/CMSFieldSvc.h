#ifndef CMSFIELDSVC_H
#define CMSFIELDSVC_H


#include "SimPapasDetector/IPapasFieldSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"


/** @class DetetctorSvc DetetctorSvc.h DetetctorSvc/DetetctorSvc.h
 *
 *  This service provides the Papas Detector
 *  @author alice.robson@cern.ch
 */

namespace papas {
  class Field;
}


class CMSFieldSvc : public extends1<Service, IPapasFieldSvc> {

public:
  /// Dfault constructor
  CMSFieldSvc(const std::string& name, ISvcLocator* svc);
  /// Default destructor
  virtual ~CMSFieldSvc() = default;
  /// Initialize function
  virtual StatusCode initialize() override final;
  /// Finalize function
  virtual StatusCode finalize() override final;
  /// pointer to detector
  virtual std::shared_ptr<papas::Field> field() const;

private:
  // Detector
  std::shared_ptr<papas::Field> m_field;
  Gaudi::Property<double> m_fieldMagnitude{this, "field", 3.8, "Field magnitude"};
  
};

inline std::shared_ptr<papas::Field> CMSFieldSvc::field() const { return m_field; }

#endif  // CLICFIELDSVC_H
