#ifndef CLICHCALSVC_H
#define CLICHCALSVC_H


#include "SimPapasDetector/IPapasCalorimeterSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"


/** @class DetetctorSvc DetetctorSvc.h DetetctorSvc/DetetctorSvc.h
 *
 *  This service provides the Papas Detector
 *  @author alice.robson@cern.ch
 */

namespace papas {
  class Calorimeter;
}


class ClicHcalSvc : public extends1<Service, IPapasCalorimeterSvc> {

public:
  /// Dfault constructor
  ClicHcalSvc(const std::string& name, ISvcLocator* svc);
  /// Default destructor
  virtual ~ClicHcalSvc() = default;
  /// Initialize function
  virtual StatusCode initialize() override final;
  /// Finalize function
  virtual StatusCode finalize() override final;
  /// pointer to detector
  virtual std::shared_ptr<papas::Calorimeter> calorimeter() const;

private:
  // Detector
  std::shared_ptr<papas::Calorimeter> m_hcal;
  //Gaudi::Property<double> m_hcalMagnitude{this, "hcal", 2, "Hcal magnitude"};
  
};

inline std::shared_ptr<papas::Calorimeter> ClicHcalSvc::calorimeter() const { return m_hcal; }

#endif  // CLICHCALSVC_H
