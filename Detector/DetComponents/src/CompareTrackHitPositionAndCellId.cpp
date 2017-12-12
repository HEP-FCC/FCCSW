#include "CompareTrackHitPositionAndCellId.h"

#include "GaudiKernel/SystemOfUnits.h"


// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DD4hep/Volumes.h"
#include "DDSegmentation/CartesianGridXZ.h"
#include "TGeoManager.h"

// EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"

DECLARE_ALGORITHM_FACTORY(CompareTrackHitPositionAndCellId)

CompareTrackHitPositionAndCellId::CompareTrackHitPositionAndCellId(const std::string& name, ISvcLocator* svcLoc)
    : GaudiAlgorithm(name, svcLoc), m_geoSvc("GeoSvc", "CompareTrackHitPositionAndCellId") {
  declareProperty("hits", m_trackHits, "Hit collection (input)");
  declareProperty("positionedHits", m_positionedTrackHits, "Positions of centres of volumes (output)");
}

StatusCode CompareTrackHitPositionAndCellId::initialize() { return GaudiAlgorithm::initialize(); }

StatusCode CompareTrackHitPositionAndCellId::execute() {
  auto lcdd = m_geoSvc->lcdd();
  auto allReadouts = lcdd->readouts();
  auto readoutBarrel = lcdd->readout("TrackerBarrelReadout");
  auto m_decoderBarrel = readoutBarrel.idSpec().decoder();
  auto readoutEndcap = lcdd->readout("TrackerEndcapReadout");
  auto m_decoderEndcap = readoutEndcap.idSpec().decoder();
  auto segmentationXZ =
      dynamic_cast<DD4hep::DDSegmentation::CartesianGridXZ*>(readoutBarrel.segmentation().segmentation());
  if (nullptr == segmentationXZ) {
    error() << "Could not retrieve segmentation!" << endmsg;
    return StatusCode::FAILURE;
  }
  double l_segGridSizeXBarrel = segmentationXZ->gridSizeX();
  double l_segGridSizeZBarrel = segmentationXZ->gridSizeZ();

  segmentationXZ = dynamic_cast<DD4hep::DDSegmentation::CartesianGridXZ*>(readoutEndcap.segmentation().segmentation());
  if (nullptr == segmentationXZ) {
    error() << "Could not retrieve segmentation!" << endmsg;
    return StatusCode::FAILURE;
  }
  double l_segGridSizeXEndcap = segmentationXZ->gridSizeX();
  double l_segGridSizeZEndcap = segmentationXZ->gridSizeZ();
  std::cout << l_segGridSizeXBarrel << std::endl;

  DD4hep::Geometry::VolumeManager volman = m_geoSvc->lcdd()->volumeManager();

  const fcc::PositionedTrackHitCollection* hits = m_positionedTrackHits.get();
  double fcc_l1 = 0;
  double fcc_l2 = 0;
  // expect position from cellID to deviate at most two cell sizes from true energy deposit
  // use largest cell for simplicity 
  double l_tolerance = 1 * Gaudi::Units::mm;
    for (auto hit : (*hits)) {
    long long int theCellId = hit.core().cellId;
    debug() << theCellId << endmsg;
    m_decoderBarrel->setValue(theCellId);
    int system_id = (*m_decoderBarrel)["system"];
    if (system_id == 0 || system_id == 1) {

      auto detelement = volman.lookupDetElement(theCellId);
      debug() << "volIDfromDetElement: " << detelement.volumeID() << endmsg;
      const auto& transformMatrix = detelement.worldTransformation();
      double outGlobal[3] {0,0,0};
      fcc_l1 = (*m_decoderBarrel)["x"] * l_segGridSizeXBarrel;
      fcc_l2 = (*m_decoderBarrel)["z"] * l_segGridSizeZBarrel;
      double inLocal[] = {fcc_l1, 0, fcc_l2};
      transformMatrix.LocalToMaster(inLocal, outGlobal);
      auto edmPos = fcc::Point();
      edmPos.x = outGlobal[0] / dd4hep::mm;
      edmPos.y = outGlobal[1] / dd4hep::mm;
      edmPos.z = outGlobal[2] / dd4hep::mm;

      debug() << " hit in system: " << system_id;
      int layer_id = (*m_decoderBarrel)["layer"];
      debug() << "\t layer " << layer_id;
      int module_id = (*m_decoderBarrel)["module"];
      debug() << "\t module " << module_id;
      debug() << endmsg;

      debug() << "Hit with Cell Id: " << theCellId << endmsg;
      debug() << "PositionedHit [mm]: x: " << hit.position().x << "\t y: " << hit.position().y << "\t z: " << hit.position().z
              << endmsg;
      debug() << "CellId       [mm]: x: " << edmPos.x << "\t y: " << edmPos.y << "\t z: " << edmPos.z << endmsg;
      if ((abs(edmPos.x - hit.position().x) > l_tolerance) || (abs(edmPos.y - hit.position().y) > l_tolerance) ||
          (abs(edmPos.z - hit.position().z) > l_tolerance)) {
        error() << "positionedHit and position from CellId are not compatible!" << endmsg;
        return StatusCode::FAILURE;
      }

    } else if (system_id == 2 || system_id == 3 || system_id == 4) {
      m_decoderEndcap->setValue(theCellId);
      int posneg_id = (*m_decoderEndcap)["posneg"];
      debug() << "\t posneg " << posneg_id;
      int layer_id = (*m_decoderEndcap)["disc"];
      debug() << "\t disc " << layer_id;
      int module_id = (*m_decoderEndcap)["component"];
      debug() << "\t module " << module_id;
      debug() << endmsg;
      auto detelement = volman.lookupDetElement((*m_decoderEndcap).getValue());
      debug() << "volId from Detelement: " << detelement.volumeID() << endmsg;
      const auto& transformMatrix = detelement.worldTransformation();
      double outGlobal[3] {0, 0, 0};
      fcc_l1 = (*m_decoderEndcap)["x"] * l_segGridSizeXEndcap;
      fcc_l2 = (*m_decoderEndcap)["z"] * l_segGridSizeZEndcap;
      double inLocal[] = {fcc_l1, 0, fcc_l2};
      transformMatrix.LocalToMaster(inLocal, outGlobal);
      auto edmPos = fcc::Point();
      edmPos.x = outGlobal[0] / dd4hep::mm;
      edmPos.y = outGlobal[1] / dd4hep::mm;
      edmPos.z = outGlobal[2] / dd4hep::mm;

      debug() << "Positioned Hit [mm]: x: " << hit.position().x << "\t y: " << hit.position().y << "\t z: " << hit.position().z
              << endmsg;
      debug() << "CellID        [mm]: x: " << edmPos.x << "\t y: " << edmPos.y << "\t z: " << edmPos.z << endmsg;

      if ((abs(edmPos.x - hit.position().x) > l_tolerance) || (abs(edmPos.y - hit.position().y) > l_tolerance) ||
          (abs(edmPos.z - hit.position().z) > 1. /*mm*/)) {
        error() << "positionedHit and position from CellId are not compatible!" << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode CompareTrackHitPositionAndCellId::finalize() { return GaudiAlgorithm::finalize(); }
