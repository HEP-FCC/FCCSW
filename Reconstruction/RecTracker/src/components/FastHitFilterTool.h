#ifndef RECTRACKER_DOUBLETCREATIONTOOL_H
#define RECTRACKER_DOUBLETCREATIONTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ITrackSeedingTool.h"
#include "RecInterface/IHitFilterTool.h"
#include "RecInterface/ILayerGraphTool.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"

#include "tricktrack/SpacePoint.h"
#include "tricktrack/CMGraph.h"
#include "tricktrack/TrackingRegion.h"
#include "tricktrack/HitChainMaker.h"

#include <map>


class FastHitFilterTool : public GaudiTool, virtual public IHitFilterTool {
public:
  FastHitFilterTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~FastHitFilterTool() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  virtual bool filter(fcc::TrackHit hit) override final;
  virtual void setIds(unsigned int systemId, unsigned int layerId) override final;
private:

  unsigned int m_layerId = 0;
  unsigned int m_systemId = 0;

};

#endif /* RECTRACKER_DOUBLETCREATIONTOOL_H */
