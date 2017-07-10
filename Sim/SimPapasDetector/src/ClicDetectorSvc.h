#ifndef CLICDETECTOR_DETECTOR_H
#define CLICDETECTOR_DETECTOR_H


#include "SimPapasDetector/IPapasDetSvc.h"
#include "SimPapasDetector/IPapasFieldSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"


/** @class DetetctorSvc DetetctorSvc.h DetetctorSvc/DetetctorSvc.h
 *
 *  This service provides the Papas Detector
 *  @author alice.robson@cern.ch
 */

namespace papas {
  class Detector;
}


class ClicDetSvc : public extends1<Service, IPapasDetSvc> {

public:
  /// Default constructor
  ClicDetSvc(const std::string& name, ISvcLocator* svc);
  /// Default destructor
  virtual ~ClicDetSvc() = default;
  /// Initialize function
  virtual StatusCode initialize() override final;
  /// Finalize function
  virtual StatusCode finalize() override final;
  /// pointer to detector
  virtual std::shared_ptr<papas::Detector> detector() const;

private:
  // Detector
  std::shared_ptr<papas::Detector> m_detector;
  /// Pointer to the interface of papas detector
  SmartIF<IPapasFieldSvc> m_papasFieldSvc;
  
  Gaudi::Property<double> m_cyl1{this, "innerEcalCylinder", 2.15, "Radius ECAL inner"};
  Gaudi::Property<double> m_cyl2{this, "outerEcalCylinder", 2.4, "Radius ECAL outer"};
  Gaudi::Property<std::string> m_fieldServiceName{this, "fieldService", "", "Field service"};
  
};

inline std::shared_ptr<papas::Detector> ClicDetSvc::detector() const { return m_detector; }

#endif  // CMSDETECTOR_DETECTOR_H
