#ifndef RECTRACKER_TRUTHSEEDINGTOOL_H
#define RECTRACKER_TRUTHSEEDINGTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimG4Interface/ISimG4SaveOutputTool.h"
#include "RecInterface/ITrackSeedingTool.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"


#include <map>

class IGeoSvc;


class TruthSeedingTool : public GaudiTool, virtual public ITrackSeedingTool {
public:
  TruthSeedingTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~TruthSeedingTool() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  virtual std::multimap<unsigned int, unsigned int> findSeeds(const fcc::PositionedTrackHitCollection* theHits) override final;

};

#endif /* RECTRACKER_TRUTHSEEDINGTOOL_H */
