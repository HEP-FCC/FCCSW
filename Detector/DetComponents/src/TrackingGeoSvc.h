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

  /// Destructor
  virtual ~TrackingGeoSvc();
  //!< Retrieve interface ID
  //        static const InterfaceID& interfaceID() { return IID_ITrackingGeoSvc; }
  /// Initialize function
  virtual StatusCode initialize();
  /// Finalize function
  virtual StatusCode finalize();
  /** Query the interfaces.
   /   Input: riid, Requested interface ID
   /          ppvInterface, Pointer to requested interface **/
  //       StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );
  // receive DD4hep Geometry
  virtual std::shared_ptr<Acts::TrackingGeometry> trackingGeometry() const;

private:
  /// Handle to the FCC geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  // Tracking  Geometry
  std::shared_ptr<Acts::TrackingGeometry> m_trackingGeo;
};

inline std::shared_ptr<Acts::TrackingGeometry> TrackingGeoSvc::trackingGeometry() const { return m_trackingGeo; }

#endif  // TRACKINGSVC_TRACKINGSVC_H
