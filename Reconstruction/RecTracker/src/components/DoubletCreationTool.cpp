#include "DoubletCreationTool.h"

#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"


// TrickTrack headers
#include "tricktrack/HitChainMaker.h"
#include "tricktrack/HitDoublets.h"
#include "tricktrack/SpacePoint.h"

using namespace tricktrack;



DECLARE_TOOL_FACTORY(DoubletCreationTool)

DoubletCreationTool::DoubletCreationTool(const std::string& type, const std::string& name,
                                                   const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<IDoubletCreationTool>(this);

}

StatusCode DoubletCreationTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }
  return sc;
}

   tricktrack::HitDoublets<Hit>*   DoubletCreationTool::findDoublets( std::vector<Hit> theInnerHits,  std::vector<Hit> theOuterHits) {

  auto doublets = new HitDoublets<Hit>(theInnerHits, theOuterHits);

  // brute force doublet creation
  for (unsigned int i = 0; i < theInnerHits.size(); ++i) {
    for( unsigned int j = 0; j < theOuterHits.size(); ++j) {
        // @todo: very ad-hoc
        if ((std::fmod(std::abs(theOuterHits[j].phi() - theInnerHits[i].phi()), 2* M_PI)) < 0.3) {
        if (std::abs(theOuterHits[j].z() - theInnerHits[i].z()) < 50) {
        doublets->add(i, j);
        }
        }
    }
  }
 return doublets;
}




StatusCode DoubletCreationTool::finalize() { return GaudiTool::finalize(); }
