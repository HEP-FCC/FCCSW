#ifndef CMSDETECTOR_DETECTOR_H
#define CMSDETECTOR_DETECTOR_H

#include "SimPapas/IPapasCalorimeterSvc.h"
#include "SimPapas/IPapasDetSvc.h"
#include "SimPapas/IPapasFieldSvc.h"
#include "SimPapas/IPapasTrackerSvc.h"

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
  /// Detector
  std::shared_ptr<papas::Detector> m_detector;
  Gaudi::Property<std::string> m_ecalServiceName{this, "ecalService", "", "Ecal service"};
  Gaudi::Property<std::string> m_hcalServiceName{this, "hcalService", "", "Hcal service"};
  Gaudi::Property<std::string> m_trackerServiceName{this, "trackerService", "", "Tracker service"};
  Gaudi::Property<std::string> m_fieldServiceName{this, "fieldService", "", "Field service"};
  Gaudi::Property<double> m_electronAcceptanceMagnitude{this, "electronAcceptanceMagnitude", 5.,
                                                        "electron acceptance magnitude"};
  Gaudi::Property<double> m_electronAcceptanceEta{this, "electronAcceptanceEta", 2.5, "electron acceptance eta"};
  Gaudi::Property<double> m_muonAcceptanceMagnitude{this, "muonAcceptanceMagnitude", 7.5, "muon acceptance magnitude"};
  Gaudi::Property<double> m_muonAcceptanceTheta{this, "muonAcceptanceTheta", 80, "muon acceptance theta"};
  Gaudi::Property<double> m_electronEnergyFactor{this, "electronEnergyFactor", 0.1, "electron energy factor"};
  Gaudi::Property<double> m_muonResolution{this, "muonResolution", 0.02, "Moun resolution"};
};
inline std::shared_ptr<papas::Detector> CMSDetSvc::detector() const { return m_detector; }

#endif  // CMSDETECTOR_DETECTOR_H
