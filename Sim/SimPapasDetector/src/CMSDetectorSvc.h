#ifndef CMSDETECTOR_DETECTOR_H
#define CMSDETECTOR_DETECTOR_H


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


class CMSDetSvc : public extends1<Service, IPapasDetSvc> {

public:
  /// Default constructor
  CMSDetSvc(const std::string& name, ISvcLocator* svc);
  /// Default destructor
  virtual ~CMSDetSvc() = default;
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

  Gaudi::Property<double> m_cyl1{this, "innerEcalCylinder", 1.3, "Size  of CMS ECAL inner"};
  Gaudi::Property<double> m_cyl2{this, "outerEcalCylinder", 1.55, "Size  of CMS ECAL inner"};
  Gaudi::Property<std::string> m_fieldServiceName{this, "fieldService", "", "CMS Field service"};
};

inline std::shared_ptr<papas::Detector> CMSDetSvc::detector() const { return m_detector; }

#endif  // CMSDETECTOR_DETECTOR_H
