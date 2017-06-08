#include "CombinatorialSeedingTool.h"

#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackStateCollection.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/Volumes.h"
#include "DDRec/API/IDDecoder.h"
#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/CartesianGridXZ.h"

#include "ACTS/Seeding/BarrelSeedFinder.hpp"
#include "ACTS/Seeding/SpacePoint.hpp"
#include "ACTS/Seeding/TrackSeed.hpp"

DECLARE_TOOL_FACTORY(CombinatorialSeedingTool)

CombinatorialSeedingTool::CombinatorialSeedingTool(const std::string& type, const std::string& name,
                                                   const IInterface* parent)
    : GaudiTool(type, name, parent),
      m_geoSvc("GeoSvc", "CombinatorialSeedingTool") {
  declareInterface<ITrackSeedingTool>(this);
  declareProperty("trackSeeds", m_trackSeeds, "Track Seeds (Output)");
}

StatusCode CombinatorialSeedingTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) {
    return sc;
  }
  auto lcdd = m_geoSvc->lcdd();
  auto readout = lcdd->readout(m_readoutName);
  m_decoder = readout.idSpec().decoder();
  return sc;
}

void CombinatorialSeedingTool::createBarrelSpacePoints(Acts::Seeding::BarrelSpacePoints<size_t>& thePoints,
                                                       const fcc::PositionedTrackHitCollection* theHits,
                                                       std::pair<int, int> sIndex) {
  size_t hitCounter = 0;
  for (auto hit : *theHits) {
    m_decoder->setValue(hit.core().cellId);
    if ((*m_decoder)["system"] == sIndex.first) {
      if ((*m_decoder)["layer"] == sIndex.second) {
        thePoints.points.emplace_back(hit.position().x, hit.position().y, hit.position().z, hitCounter);
      }
    }
    ++hitCounter;
  }
}

std::multimap<unsigned int, unsigned int>
CombinatorialSeedingTool::findSeeds(const fcc::PositionedTrackHitCollection* theHits) {

  fcc::TrackStateCollection* trackSeedCollection = m_trackSeeds.createAndPut();
  std::multimap<unsigned int, unsigned int> theSeeds;

  Acts::Seeding::HelixSeedConfig seedFinderCfg;
  seedFinderCfg.rangePhi1 = m_rangePhi1;
  seedFinderCfg.rangePhi2 = m_rangePhi1;
  seedFinderCfg.maxDeltaTheta = m_dTheta;
  Acts::Seeding::TrackSeeds3<size_t> seeds;

  Acts::Seeding::BarrelSpacePoints<size_t> pointsLayer1(10);
  Acts::Seeding::BarrelSpacePoints<size_t> pointsLayer2(20);
  Acts::Seeding::BarrelSpacePoints<size_t> pointsLayer3(30);

  createBarrelSpacePoints(pointsLayer1, theHits, m_seedingLayerIndices0);
  createBarrelSpacePoints(pointsLayer2, theHits, m_seedingLayerIndices1);
  createBarrelSpacePoints(pointsLayer3, theHits, m_seedingLayerIndices2);

  findHelixSeeds(seedFinderCfg, pointsLayer1, pointsLayer2, pointsLayer3, seeds);

  debug() << "found " << seeds.size() << " track seeds" << endmsg;

  unsigned int trackCounter = 0;
  for (const auto& seed : seeds) {
    std::array<float, 15> edmCov;
    float l_phi = seed.phi();
    float l_theta = seed.theta();
    float l_curvature = seed.curvature();
    trackSeedCollection->create(l_phi, l_theta, l_curvature, 0.f, 0.f, fcc::Point(), edmCov);
    for (const auto& id : seed.identifiers()) {
      theSeeds.insert(std::pair<unsigned int, unsigned int>(trackCounter, id));
    }

    ++trackCounter;
  }

  return theSeeds;
}

StatusCode CombinatorialSeedingTool::finalize() { return GaudiTool::finalize(); }
