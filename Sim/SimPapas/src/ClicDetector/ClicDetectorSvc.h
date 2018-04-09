#ifndef CLICDETECTOR_DETECTOR_H
#define CLICDETECTOR_DETECTOR_H

#include "SimPapas/IPapasCalorimeterSvc.h"
#include "SimPapas/IPapasDetSvc.h"
#include "SimPapas/IPapasFieldSvc.h"
#include "SimPapas/IPapasTrackerSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

/** @class ClicDetSvc ClicDetSvc.h
 *
 *  This service provides the Papas Clic Detector Service
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
  Gaudi::Property<std::string> m_ecalServiceName{this, "ecalService", "", "Ecal service"};
  Gaudi::Property<std::string> m_hcalServiceName{this, "hcalService", "", "Hcal service"};
  Gaudi::Property<std::string> m_trackerServiceName{this, "trackerService", "", "Tracker service"};
  Gaudi::Property<std::string> m_fieldServiceName{this, "fieldService", "", "Field service"};
  Gaudi::Property<double> m_electronAcceptanceMagnitude{this, "electronAcceptanceMagnitude", 5.,
                                                        "electron acceptance magnitude"};
  Gaudi::Property<double> m_electronAcceptanceEta{this, "electronAcceptanceEta", 2.5, "electron acceptance eta"};
  Gaudi::Property<double> m_electronAcceptanceEfficiency{this, "electronAcceptanceEfficiency", 0.95,
                                                         "electron acceptance efficiency"};
  Gaudi::Property<double> m_muonAcceptanceMagnitude{this, "muonAcceptanceMagnitude", 7.5, "muon acceptance magnitude"};
  Gaudi::Property<double> m_muonAcceptanceTheta{this, "muonAcceptanceTheta", 80, "muon acceptance theta"};
  Gaudi::Property<double> m_muonResolution{this, "muonResolution", 0.02, "Moun resolution"};
};

inline std::shared_ptr<papas::Detector> ClicDetSvc::detector() const { return m_detector; }

#endif  // CLICDETECTOR_DETECTOR_H
