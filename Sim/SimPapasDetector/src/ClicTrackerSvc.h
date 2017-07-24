#ifndef CLICTRACKERSVC_H
#define CLICTRACKERSVC_H


#include "SimPapasDetector/IPapasTrackerSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"


/** @class DetetctorSvc DetetctorSvc.h DetetctorSvc/DetetctorSvc.h
 *
 *  This service provides the Papas Detector
 *  @author alice.robson@cern.ch
 */

namespace papas {
  class Tracker;
}


class ClicTrackerSvc : public extends1<Service, IPapasTrackerSvc> {

public:
  /// Dfault constructor
  ClicTrackerSvc(const std::string& name, ISvcLocator* svc);
  /// Default destructor
  virtual ~ClicTrackerSvc() = default;
  /// Initialize function
  virtual StatusCode initialize() override final;
  /// Finalize function
  virtual StatusCode finalize() override final;
  /// pointer to detector
  virtual std::shared_ptr<papas::Tracker> tracker() const;

private:
  // Detector
  std::shared_ptr<papas::Tracker> m_tracker;
  //Gaudi::Property<double> m_trackerMagnitude{this, "tracker", 2, "Tracker magnitude"};
  
};

inline std::shared_ptr<papas::Tracker> ClicTrackerSvc::tracker() const { return m_tracker; }

#endif  // CLICTRACKERSVC_H
