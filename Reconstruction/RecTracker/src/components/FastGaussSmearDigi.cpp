
#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"

#include "DD4hep/LCDD.h"
#include "DD4hep/Volumes.h"
#include "DDRec/API/IDDecoder.h"
#include "DDSegmentation/BitField64.h"
#include "DDSegmentation/CartesianGridXZ.h"

#include <cmath>
#include <random>

#include "FastGaussSmearDigi.h"


DECLARE_ALGORITHM_FACTORY(FastGaussSmearDigi)

// For use with standard sorting algorithms
// compares the system fields of the hits first, then the layer field
struct CellIdOrder {
  CellIdOrder(DD4hep::DDSegmentation::BitField64* dec) : m_decoder(dec) {}

  bool operator()(const fcc::TrackHit& h1, const fcc::TrackHit& h2) {
    m_decoder->setValue(h1.cellId());
    int system1 = (*m_decoder)["system"];
    int layer1 = (*m_decoder)["layer"];
    m_decoder->setValue(h2.cellId());
    int system2 = (*m_decoder)["system"];
    int layer2 = (*m_decoder)["layer"];
    if (system1 < system2) return true;
    if (system2 < system1) return false;
    return (layer1 < layer2);
  }

private:
  DD4hep::DDSegmentation::BitField64* m_decoder;
};

/// fill vector with hits ordered according to the CellIdOrder struct
void sortTrackHits(const fcc::TrackHitCollection* unsortedHits, std::vector<fcc::TrackHit>& sortedHits,
                   DD4hep::DDSegmentation::BitField64* decoder) {
  sortedHits.reserve(unsortedHits->size());
  for (const auto& hit : *unsortedHits) {
    sortedHits.push_back(hit);
  }
  std::sort(sortedHits.begin(), sortedHits.end(), CellIdOrder(decoder));
}

FastGaussSmearDigi::FastGaussSmearDigi(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {

  declareProperty("smearedHits", m_smearedTrackHits, "Smeared Tracker hits (Output)");
  declareProperty("trackHits", m_trackHits, "Tracker hits (Input)");
}

FastGaussSmearDigi::~FastGaussSmearDigi() {}

StatusCode FastGaussSmearDigi::initialize() {
  info() << "initialize" << endmsg;

  m_geoSvc = service("GeoSvc");

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  auto lcdd = m_geoSvc->lcdd();
  auto readout = lcdd->readout(m_readoutName);
  m_decoder = readout.idSpec().decoder();
  auto segmentationXZ = dynamic_cast<DD4hep::DDSegmentation::CartesianGridXZ*>(readout.segmentation().segmentation());
  m_segGridSizeX = segmentationXZ->gridSizeX();
  m_segGridSizeZ = segmentationXZ->gridSizeZ();
  m_volman = lcdd->volumeManager();
  return sc;
}

StatusCode FastGaussSmearDigi::execute() {

  // get hits from event store
  const fcc::TrackHitCollection* hits = m_trackHits.get();
  std::vector<fcc::TrackHit> sortedHits;
  sortTrackHits(hits, sortedHits, m_decoder);

  auto edmPositions = m_smearedTrackHits.createAndPut();
  for (const auto& hit : sortedHits) {
    const fcc::BareHit& hitCore = hit.core();
    m_decoder->setValue(hit.core().cellId);
    /// the local coordinates on the module
    // add segmentation info and smearing here
    std::array<double, 3> l = {(*m_decoder)["x"] * m_segGridSizeX + m_gaussDist() / sqrt(12.) * m_segGridSizeX, 0,
                   (*m_decoder)["z"] * m_segGridSizeZ + m_gaussDist() / sqrt(12.) * m_segGridSizeZ};
    // global coordinates, will be filled by the transform
    std::array<double, 3> g = {0, 0, 0};
    // direct lookup of transformation in the volume manager is broken in dd4hep
    auto detelement = m_volman.lookupDetElement(m_decoder->getValue());
    const auto& localToGlobal = detelement.worldTransformation();
    localToGlobal.LocalToMaster(l.data(), g.data());
    auto position = fcc::Point();
    // default dd4hep units differ from fcc ones
    position.x = g[0] * CM_2_MM;
    position.y = g[1] * CM_2_MM;
    position.z = g[2] * CM_2_MM;
    edmPositions->create(position, hitCore);
  }
  return StatusCode::SUCCESS;
}

StatusCode FastGaussSmearDigi::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
