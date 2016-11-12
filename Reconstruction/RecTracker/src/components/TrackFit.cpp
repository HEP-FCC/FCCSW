#include "TrackFit.h"

//FCCSW
#include "DetInterface/ITrackingGeoSvc.h"

// our EDM
#include "datamodel/TrackHitCollection.h"


DECLARE_ALGORITHM_FACTORY(TrackFit)

TrackFit::TrackFit(const std::string& name, ISvcLocator* svcLoc)
  : GaudiAlgorithm(name, svcLoc)
{
  //declareInput("caloCells", m_caloCells,"caloCells");
  //declareOutput("caloPositionedHits", m_caloPositionedHits,"caloPositionedHits");
  //declareProperty("readoutName", m_readoutName="ECalHitsNew");
  //declareProperty("activeFieldName", m_activeFieldName="active_layer");
  //declareProperty("activeVolumeName", m_activeVolumeName="LAr");
  ////ECAL LAr specific: LAr bath in cryostat same material as active layer volume
  //declareProperty("numVolumesRemove",m_numVolumesRemove=1);
}

TrackFit::~TrackFit()
{
}

StatusCode TrackFit::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  m_trkGeoSvc = service("TrackingGeoSvc");
  if (nullptr == m_trkGeoSvc) {
    error() << "Unable to locate Tracking Geometry Service. "
            << endmsg;
    return StatusCode::FAILURE;
  }

  return sc;
}

StatusCode TrackFit::execute() {


  return StatusCode::SUCCESS;
}

StatusCode TrackFit::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
