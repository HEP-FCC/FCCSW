#ifndef RECTRACKER_COMBINATORIALSEEDINGTOOL_H
#define RECTRACKER_COMBINATORIALSEEDINGTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ITrackSeedingTool.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"

#include "ACTS/Seeding/SpacePoint.hpp"

#include <map>

class IGeoSvc;

namespace DD4hep {
namespace DDSegmentation {
class BitField64;
}
}

class CombinatorialSeedingTool : public GaudiTool, virtual public ITrackSeedingTool {
public:
  CombinatorialSeedingTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~CombinatorialSeedingTool() = default;
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  virtual std::multimap<unsigned int, unsigned int> findSeeds(const fcc::PositionedTrackHitCollection* theHits) final;
  void createBarrelSpacePoints(Acts::Seeding::BarrelSpacePoints<size_t>& thePoints,
                               const fcc::PositionedTrackHitCollection* theHits, std::pair<int, int> sIndex);

private:
  SmartIF<IGeoSvc> m_geoSvc;
  DD4hep::DDSegmentation::BitField64* m_decoder;
  Gaudi::Property<std::pair<int, int>> m_seedingLayerIndices0{this, "seedingLayerIndices0", {0, 0}};
  Gaudi::Property<std::pair<int, int>> m_seedingLayerIndices1{this, "seedingLayerIndices1", {0, 1}};
  Gaudi::Property<std::pair<int, int>> m_seedingLayerIndices2{this, "seedingLayerIndices2", {0, 2}};
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "TrackerBarrelReadout"};
  Gaudi::Property<float> m_rangePhi1{this, "rangePhi1", 0.};
  Gaudi::Property<float> m_rangePhi2{this, "rangePhi2", 0.5};
  Gaudi::Property<float> m_dTheta{this, "maxDeltaTheta", 0.5};
  DataHandle<fcc::TrackStateCollection> m_trackSeeds{"tracks/trackSeeds", Gaudi::DataHandle::Writer, this};
};

#endif /* RECTRACKER_COMBINATORIALSEEDINGTOOL_H */
