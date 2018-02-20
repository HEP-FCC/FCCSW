#include "RiemannFitTool.h"


#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"



// TrickTrack headers
#include "tricktrack/RiemannFit.h"





DECLARE_TOOL_FACTORY(RiemannFitTool)

RiemannFitTool::RiemannFitTool(const std::string& type, const std::string& name,
                                                   const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ITrackFittingTool>(this); 

}

StatusCode RiemannFitTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }
  return sc;
}
fcc::TrackState RiemannFitTool::fitTrack(const fcc::PositionedTrackHitCollection* theHits, std::vector<unsigned int> hitIndices) {


return fcc::TrackState();
}


StatusCode RiemannFitTool::finalize() { return GaudiTool::finalize(); }
