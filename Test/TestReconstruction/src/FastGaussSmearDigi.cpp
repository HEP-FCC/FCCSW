
#include "DetInterface/IGeoSvc.h"
#include "DD4hep/Detector.h"
#include "DD4hep/BitFieldCoder.h"
#include "DDSegmentation/CartesianGridXZ.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"


#include <cmath>
#include <random>

#include "FastGaussSmearDigi.h"
#include "RecTracker/TrackingUtils.h"


DECLARE_ALGORITHM_FACTORY(FastGaussSmearDigi)



FastGaussSmearDigi::FastGaussSmearDigi(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {

  declareProperty("smearedHits", m_smearedTrackHits, "Smeared Tracker hits (Output)");
  declareProperty("trackHits", m_trackHits, "Tracker hits (Input)");
}

StatusCode FastGaussSmearDigi::initialize() {
  info() << "initialize" << endmsg;

  m_geoSvc = service("GeoSvc");

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  return sc;
}

StatusCode FastGaussSmearDigi::execute() {

  auto lcdd = m_geoSvc->lcdd();
  auto readout = lcdd->readout(m_readoutName);
  auto m_decoder = readout.idSpec().decoder();
  auto segmentationXZ = dynamic_cast<dd4hep::DDSegmentation::CartesianGridXZ*>(readout.segmentation().segmentation());
  if (nullptr == segmentationXZ) {
    error() << "Could not retrieve segmentation!" << endmsg;
    return StatusCode::FAILURE;
  }
  m_segGridSizeX = segmentationXZ->gridSizeX();
  m_segGridSizeZ = segmentationXZ->gridSizeZ();
  auto m_volman = lcdd->volumeManager();
  // get hits from event store
  const fcc::TrackHitCollection* hits = m_trackHits.get();
  std::vector<fcc::TrackHit> sortedHits;
  rec::sortTrackHits(hits, sortedHits, m_decoder);

  auto edmPositions = m_smearedTrackHits.createAndPut();
  for (const auto& hit : sortedHits) {
    const fcc::BareHit& hitCore = hit.core();
    dd4hep::DDSegmentation::CellID cID = hit.core().cellId;
    /// the local coordinates on the module
    // add segmentation info and smearing here
    int x = m_decoder->get(cID, "x");
    int z = m_decoder->get(cID, "z");
    std::array<double, 3> localPos = {x * m_segGridSizeX + m_gaussDist() / sqrt(12.) * m_segGridSizeX, 0,
                   		      z * m_segGridSizeZ + m_gaussDist() / sqrt(12.) * m_segGridSizeZ};
    // global coordinates, will be filled by the transform
    std::array<double, 3> globalPos = {0, 0, 0};
    // direct lookup of transformation in the volume manager is broken in dd4hep
    auto detelement = m_volman.lookupDetElement(cID);
    const auto& localToGlobal = detelement.nominal().worldTransformation();
    localToGlobal.LocalToMaster(localPos.data(), globalPos.data());
    auto position = fcc::Point();
    // default dd4hep units differ from fcc ones
    position.x = globalPos[0] * CM_2_MM;
    position.y = globalPos[1] * CM_2_MM;
    position.z = globalPos[2] * CM_2_MM;
    edmPositions->create(position, hitCore);
  }
  return StatusCode::SUCCESS;
}

StatusCode FastGaussSmearDigi::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
