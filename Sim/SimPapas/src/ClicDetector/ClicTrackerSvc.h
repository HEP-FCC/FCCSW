#ifndef CLICTRACKERSVC_H
#define CLICTRACKERSVC_H

#include "SimPapas/IPapasTrackerSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

#include "GaudiKernel/Map.h"
#include "GaudiKernel/ParsersFactory.h"
#include "GaudiKernel/StdArrayAsProperty.h"

#include <map>
#include <tuple>
#include <vector>

/** @class ClicTrackerrSvc
 *
 *  This service provides the Papas Clic Tracker
 *  @author alice.robson@cern.ch
 */

namespace papas {
class Tracker;
}

class ClicTrackerSvc : public extends1<Service, IPapasTrackerSvc> {

public:
  /// Default constructor
  ClicTrackerSvc(const std::string& name, ISvcLocator* svc);
  /// Default destructor
  virtual ~ClicTrackerSvc() = default;
  /// Initialize function
  virtual StatusCode initialize() override final;
  /// Finalize function
  virtual StatusCode finalize() override final;
  /// pointer to tracker
  virtual std::shared_ptr<papas::Tracker> tracker() const;

private:
  // Tracker
  std::shared_ptr<papas::Tracker> m_tracker;
  Gaudi::Property<double> m_radius{this, "radius", 2.14, "Tracker cylinder radius"};
  Gaudi::Property<double> m_z{this, "z", 2.6, "Tracker cylinder z"};
  Gaudi::Property<double> m_x0{this, "x0", 0.0, "tracker material x0"};
  Gaudi::Property<double> m_lambdaI{this, "lambdaI", 0.0, "Tracker material lambdaI"};
  Gaudi::Property<double> m_thetaParam{this, "thetapar", 80, "Tracker theta parameter"};

  // Really we want a map with int as an index but Gaudi does not appear to support this in combination
  // with a pair, however it does support a map indexed by a string
  // we will convert the string to an integer when we use it.
  Gaudi::Property<std::map<std::string, std::pair<double, double>>> m_resMap{this,
                                                                             "resMap",
                                                                             {{"90", {8.2e-2, 9.1e-2}},
                                                                              {"80", {8.2e-4, 9.1e-3}},
                                                                              {"30", {9.9e-5, 3.8e-3}},
                                                                              {"20", {3.9e-5, 1.6e-3}},
                                                                              {"10", {2e-5, 7.2e-4}}},
                                                                             "Tracker res map"};
  Gaudi::Property<double> m_ptThresholdLow{this, "ptThresholdLow", 0.4, "Tracker lower pt threshold"};
  Gaudi::Property<double> m_ptProbabilityLow{this, "ptProbablityLow", 0.95, "Tracker lower pt probablility"};
  Gaudi::Property<double> m_ptThresholdHigh{this, "ptThresholdHigh", 2., "Tracker upper pt threshold"};
  Gaudi::Property<double> m_ptProbabilityHigh{this, "ptProbablityHigh", 0.99, "Tracker upper pt probablility"};
};

inline std::shared_ptr<papas::Tracker> ClicTrackerSvc::tracker() const { return m_tracker; }

#endif  // CLICTRACKERSVC_H
