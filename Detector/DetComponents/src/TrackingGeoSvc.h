#ifndef TRACKINGSVC_TRACKINGSVC_H
#define TRACKINGSVC_TRACKINGSVC_H

// ATS
#include "DetInterface/ITrackingGeoSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

// DD4Hep
#include "DD4hep/LCDD.h"

// DetectorDescription
#include "DetInterface/IGeoSvc.h"

/** @class TrackingSvc TrackingSvc.h TrackingSvc/TrackingSvc.h
 *
 *  This service provides the DD4hepGeometry to the external tracking package ACTS.
 *
 *  @author julia.hrdinka@cern.ch
 */

namespace DD4hep {
namespace Geometry {
class DetElement;
}
}

namespace Acts {
class TrackingGeometry;
}

class TrackingGeoSvc : public extends1<Service, ITrackingGeoSvc> {

public:
  /// Default constructor
  TrackingGeoSvc(const std::string& name, ISvcLocator* svc);
  /// Default destructor
  virtual ~TrackingGeoSvc() = default;
  /// Initialize function
  virtual StatusCode initialize() override final;
  /// Finalize function
  virtual StatusCode finalize() override final;
  // receive DD4hep Geometry
  virtual std::shared_ptr<Acts::TrackingGeometry> trackingGeometry() const;

private:
  // Tracking  Geometry
  std::shared_ptr<Acts::TrackingGeometry> m_trackingGeo;
  /// Handle to the FCC geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;
};

inline std::shared_ptr<Acts::TrackingGeometry> TrackingGeoSvc::trackingGeometry() const { return m_trackingGeo; }

#endif  // TRACKINGSVC_TRACKINGSVC_H
