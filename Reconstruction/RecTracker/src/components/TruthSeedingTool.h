#ifndef RECTRACKER_TRUTHSEEDINGTOOL_H
#define RECTRACKER_TRUTHSEEDINGTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "RecInterface/ITrackSeedingTool.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"

#include <map>

namespace rec {
class TrackSeed {
public:
  std::multimap<unsigned int, unsigned int>  getHitIndices() {return m_hitIndices;};

private:
  std::multimap<unsigned int, unsigned int>  m_hitIndices;
};
}


class TruthSeedingTool : public GaudiTool, virtual public ITrackSeedingTool {
public:
  TruthSeedingTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~TruthSeedingTool() = default;
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  virtual std::multimap<unsigned int, unsigned int>  findSeeds(const fcc::PositionedTrackHitCollection* theHits) final;

private:
};

#endif /* RECTRACKER_TRUTHSEEDINGTOOL_H */
