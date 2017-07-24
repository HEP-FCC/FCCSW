#ifndef CLICFIELDSVC_H
#define CLICFIELDSVC_H

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

class ClicFieldSvc : public extends1<Service, IPapasFieldSvc> {

public:
  /// Dfault constructor
  ClicFieldSvc(const std::string& name, ISvcLocator* svc);
  /// Default destructor
  virtual ~ClicFieldSvc() = default;
  /// Initialize function
  virtual StatusCode initialize() override final;
  /// Finalize function
  virtual StatusCode finalize() override final;
  /// pointer to detector
  virtual std::shared_ptr<papas::Field> field() const;

private:
  // Detector
  std::shared_ptr<papas::Field> m_field;
  Gaudi::Property<double> m_fieldMagnitude{this, "magnitude", 2, "Field magnitude"};
  Gaudi::Property<double> m_fieldRadius{this, "radius", 3.5, "Field  radius"};
  Gaudi::Property<double> m_fieldZ{this, "z", 4.8, "Field z"};
  Gaudi::Property<double> m_fieldX0{this, "x0", 0., "Field x0"};
  Gaudi::Property<double> m_fieldLambdaI{this, "lambdaI", 0., "Field lambdaI"};
};

inline std::shared_ptr<papas::Field> ClicFieldSvc::field() const { return m_field; }

#endif  // CLICFIELDSVC_H
