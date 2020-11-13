
#include "DetInterface/IGeoSvc.h"
#include "DD4hep/Detector.h"
#include "DD4hep/BitFieldCoder.h"
#include "DDSegmentation/CartesianGridXZ.h"

#include "edm4hep/SimTrackerHitsCollection.h"

#include <cmath>
#include <random>

#include "FastGaussSmearDigi.h"
#include "RecTracker/TrackingUtils.h"

// todo: review unit handling
#ifdef HAVE_GEANT4_UNITS
#define MM_2_CM 1.0
#define CM_2_MM 1.0
#else
#define MM_2_CM 0.1
#define CM_2_MM 10.0
#endif


DECLARE_COMPONENT(FastGaussSmearDigi)



FastGaussSmearDigi::FastGaussSmearDigi(const std::string& name, ISvcLocator* svcLoc) :
    GaudiAlgorithm(name, svcLoc), m_geoSvc("GeoSvc", name) {

  declareProperty("TrackHitsSmeared", m_smearedTrackHits, "Smeared Tracker hits (Output)");
  declareProperty("TrackHits", m_trackHits, "Tracker hits (Input)");
}

StatusCode FastGaussSmearDigi::initialize() {
  info() << "initialize" << endmsg;

  

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
  auto edmPositions = m_smearedTrackHits.createAndPut();
  // get hits from event store
  const edm4hep::SimTrackerHitsCollection* hits = m_trackHits.get();
  for (const auto& hit : gits) {
    dd4hep::DDSegmentation::CellID cID = hit.getCellID();
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
    auto position = edm4hep::Vector3f();
    // default dd4hep units differ from fcc ones
    position.x = globalPos[0] * CM_2_MM;
    position.y = globalPos[1] * CM_2_MM;
    position.z = globalPos[2] * CM_2_MM;
  }
  return StatusCode::SUCCESS;
}

StatusCode FastGaussSmearDigi::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
