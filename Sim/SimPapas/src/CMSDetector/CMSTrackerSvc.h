#ifndef CMSTRACKERSVC_H
#define CMSTRACKERSVC_H

#include "SimPapas/IPapasTrackerSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

/** @class CMSTrackerSvc
 *
 *  This service provides the Papas CMS tracker
 *  @author alice.robson@cern.ch
 */

namespace papas {
class Tracker;
}

class CMSTrackerSvc : public extends1<Service, IPapasTrackerSvc> {

public:
  /// Default constructor
  CMSTrackerSvc(const std::string& name, ISvcLocator* svc);
  /// Default destructor
  virtual ~CMSTrackerSvc() = default;
  /// Initialize function
  virtual StatusCode initialize() override final;
  /// Finalize function
  virtual StatusCode finalize() override final;
  /// pointer to tracker
  virtual std::shared_ptr<papas::Tracker> tracker() const;

private:
  ///< Tracker
  std::shared_ptr<papas::Tracker> m_tracker;
  Gaudi::Property<double> m_radius{this, "radius", 1.29, "Tracker cylinder radius"};
  Gaudi::Property<double> m_z{this, "z", 1.99, "Tracker cylinder z"};
  Gaudi::Property<double> m_x0{this, "x0", 0.0, "tracker material x0"};
  Gaudi::Property<double> m_lambdaI{this, "lambdaI", 0.0, "Tracker material lambdaI"};
  Gaudi::Property<double> m_resolution{this, "resolution", 1.1e-2, "Tracker resolution"};
  Gaudi::Property<double> m_ptThreshold{this, "ptThreshold", 0.5, "Tracker pt threshold"};
  Gaudi::Property<double> m_ptThresholdLow{this, "etaThresholdLow", 1.35, "Tracker lower pt threshold"};
  Gaudi::Property<double> m_ptProbabilityLow{this, "ptProbablityLow", 0.95, "Tracker lower pt probablility"};
  Gaudi::Property<double> m_ptThresholdHigh{this, "etaThresholdHigh", 2.5, "Tracker upper pt threshold"};
  Gaudi::Property<double> m_ptProbabilityHigh{this, "ptProbablityHigh", 0.9, "Tracker upper pt probablility"};
};

inline std::shared_ptr<papas::Tracker> CMSTrackerSvc::tracker() const { return m_tracker; }

#endif  // CMSTRACKERSVC_H
