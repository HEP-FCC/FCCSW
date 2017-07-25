#ifndef CMSDETECTOR_DETECTOR_H
#define CMSDETECTOR_DETECTOR_H

#include "SimPapasDetector/IPapasDetSvc.h"
#include "SimPapasDetector/IPapasFieldSvc.h"
#include "SimPapasDetector/IPapasCalorimeterSvc.h"
#include "SimPapasDetector/IPapasTrackerSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

/** @class CMSDetSvc CMSDetSvc.h 
 *
 *  This service provides the Papas CMS Detector
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
  SmartIF<IPapasCalorimeterSvc> m_papasEcalSvc;
  SmartIF<IPapasCalorimeterSvc> m_papasHcalSvc;
  SmartIF<IPapasTrackerSvc> m_papasTrackerSvc;
  
  //Gaudi::Property<double> m_cyl1{this, "innerEcalCylinder", 2.15, "Radius ECAL inner"};
  //Gaudi::Property<double> m_cyl2{this, "outerEcalCylinder", 2.4, "Radius ECAL outer"};
  Gaudi::Property<std::string> m_ecalServiceName{this, "ecalService", "", "Ecal service"};
  Gaudi::Property<std::string> m_hcalServiceName{this, "hcalService", "", "Hcal service"};
  Gaudi::Property<std::string> m_trackerServiceName{this, "trackerService", "", "Tracker service"};
  Gaudi::Property<std::string> m_fieldServiceName{this, "fieldService", "", "Field service"};};

inline std::shared_ptr<papas::Detector> CMSDetSvc::detector() const { return m_detector; }

#endif  // CMSDETECTOR_DETECTOR_H
