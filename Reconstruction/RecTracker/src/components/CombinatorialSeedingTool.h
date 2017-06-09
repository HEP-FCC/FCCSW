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
  virtual StatusCode initialize() override final;
  virtual StatusCode finalize() override final;

  virtual std::multimap<unsigned int, unsigned int> findSeeds(const fcc::PositionedTrackHitCollection* theHits) override final;
  void createBarrelSpacePoints(Acts::Seeding::BarrelSpacePoints<size_t>& thePoints,
                               const fcc::PositionedTrackHitCollection* theHits, std::pair<int, int> sIndex);

private:
  ServiceHandle<IGeoSvc> m_geoSvc;
  DD4hep::DDSegmentation::BitField64* m_decoder;
  /// system and layer ids for the inner barrel layer to be used for seeding
  Gaudi::Property<std::pair<int, int>> m_seedingLayerIndices0{this, "seedingLayerIndices0", {0, 0}};
  /// system and layer ids for the middle barrel layer to be used for seeding
  Gaudi::Property<std::pair<int, int>> m_seedingLayerIndices1{this, "seedingLayerIndices1", {0, 1}};
  /// system and layer ids for the outer barrel layer to be used for seeding
  Gaudi::Property<std::pair<int, int>> m_seedingLayerIndices2{this, "seedingLayerIndices2", {0, 2}};
  /// readout used for the barrel seeding layers
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "TrackerBarrelReadout"};
  /// phi window for geometric cut on the middle layer
  Gaudi::Property<float> m_rangePhi1{this, "rangePhi1", 0.};
  /// phi window for geometric cut on the outer layer
  Gaudi::Property<float> m_rangePhi2{this, "rangePhi2", 0.5};
  /// theta window for geometric cut on middle and outer layer
  Gaudi::Property<float> m_dTheta{this, "maxDeltaTheta", 0.5};
  /// output trackStates for found seeds
  DataHandle<fcc::TrackStateCollection> m_trackSeeds{"tracks/trackSeeds", Gaudi::DataHandle::Writer, this};
};

#endif /* RECTRACKER_COMBINATORIALSEEDINGTOOL_H */
