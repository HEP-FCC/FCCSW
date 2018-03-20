#ifndef RECTRACKER_DOUBLETCREATIONTOOL_H
#define RECTRACKER_DOUBLETCREATIONTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ITrackSeedingTool.h"
#include "RecInterface/IDoubletCreationTool.h"
#include "RecInterface/ILayerGraphTool.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"

#include "tricktrack/SpacePoint.h"
#include "tricktrack/CMGraph.h"
#include "tricktrack/TrackingRegion.h"
#include "tricktrack/HitChainMaker.h"

#include <map>


class DoubletCreationTool : public GaudiTool, virtual public IDoubletCreationTool {
public:
  DoubletCreationTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~DoubletCreationTool() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  virtual tricktrack::HitDoublets<Hit>* findDoublets(std::vector<Hit> theInnerHits, std::vector<Hit> theOuterHits) override final;

};

#endif /* RECTRACKER_DOUBLETCREATIONTOOL_H */
