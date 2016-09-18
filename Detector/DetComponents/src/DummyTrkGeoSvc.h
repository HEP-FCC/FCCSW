#ifndef DUMMYTRKGEOSVC_DUMMYTRKGEOSVC_H
#define DUMMYTRKGEOSVC_DUMMYTRKGEOSVC_H

#include "DetInterface/IDummyTrkGeoSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"

#include "DD4hep/LCDD.h"

#include "DetInterface/IGeoSvc.h"


namespace Acts {
  class TrackingGeometry;
}

class DummyTrkGeoSvc : public extends1<Service, IDummyTrkGeoSvc> {

public:
  DummyTrkGeoSvc(const std::string& name, ISvcLocator* svc);
  virtual ~DummyTrkGeoSvc();
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry() const;
private:
  std::shared_ptr<const Acts::TrackingGeometry> m_trackingGeo;
};

inline std::shared_ptr<const Acts::TrackingGeometry> DummyTrkGeoSvc::trackingGeometry() const { return m_trackingGeo; }

#endif  // DUMMYTRKGEOSVC_DUMMYTRKGEOSVC_H
