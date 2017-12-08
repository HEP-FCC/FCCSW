#include "CompareTrackHitPositionAndCellId.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DD4hep/Volumes.h"
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

StatusCode CompareTrackHitPositionAndCellId::initialize() {
  return GaudiAlgorithm::initialize();
}

StatusCode CompareTrackHitPositionAndCellId::execute() {
    auto lcdd = m_geoSvc->lcdd();
    auto allReadouts = lcdd->readouts();
    auto readoutBarrel = lcdd->readout("TrackerBarrelReadout");
    auto m_decoderBarrel = readoutBarrel.idSpec().decoder();
    auto readoutEndcap = lcdd->readout("TrackerEndcapReadout");
auto m_decoderEndcap = readoutEndcap.idSpec().decoder();
DD4hep::Geometry::VolumeManager volman = m_geoSvc->lcdd()->volumeManager();

  const fcc::PositionedTrackHitCollection* hits = m_positionedTrackHits.get();
  double fcc_l1 = 0;
  double fcc_l2 = 0;
  for (auto hit: (*hits)) {
  long long int theCellId = hit.core().cellId;
  debug() << theCellId << endmsg;
  m_decoderBarrel->setValue(theCellId);
  int system_id = (*m_decoderBarrel)["system"];
  if (system_id == 0 || system_id == 1) {

    auto detelement = volman.lookupDetElement(theCellId);
    const auto& transformMatrix = detelement.worldTransformation();
    double outGlobal[3];
    fcc_l1 = (*m_decoderBarrel)["x"] * 0.0005;
    fcc_l2 = (*m_decoderBarrel)["z"] * 0.001;
    double inLocal[] = {fcc_l1, 0, fcc_l2};
    transformMatrix.LocalToMaster(inLocal, outGlobal);
    auto edmPos = fcc::Point();
    edmPos.x = outGlobal[0] / dd4hep::mm;
    edmPos.y = outGlobal[1] / dd4hep::mm;
    edmPos.z = outGlobal[2] / dd4hep::mm;

  debug() << theCellId << endmsg;
  debug() << "position: x: " << hit.position().x << "\t y: " << hit.position().y << "\t z: " << hit.position().z << endmsg; 
  debug() << "position: x: " << edmPos.x << "\t y: " << edmPos.y << "\t z: " << edmPos.z << endmsg; 
  
  debug() << "phi: " << std::atan2(hit.position().y, hit.position().x) << endmsg;
  debug() << " hit in system: " << system_id;
  int layer_id = (*m_decoderBarrel)["layer"];
  debug() << "\t layer " << layer_id;
  int module_id = (*m_decoderBarrel)["module"];
  debug() << "\t module " << module_id;
  debug() << endmsg;
  debug() << "position: x: " << hit.position().x << "\t y: " << hit.position().y << "\t z: " << hit.position().z << endmsg; 
  debug() << "position: x: " << edmPos.x + fcc_l1 << "\t y: " << edmPos.y << "\t z: " << edmPos.z + fcc_l2 << endmsg; 
  } else if (system_id == 2 || system_id == 3 || system_id == 29) {
  m_decoderEndcap->setValue(theCellId);
  int posneg_id = (*m_decoderEndcap)["posneg"];
  debug() << "\t posneg " << posneg_id;
  int layer_id = (*m_decoderEndcap)["disc"];
  debug() << "\t disc " << layer_id;
  int module_id = (*m_decoderEndcap)["component"];
  debug() << "\t module " << module_id;
  (*m_decoderEndcap)["x"] = 0; // workaround for broken `volumeID` method --
  (*m_decoderEndcap)["z"] = 0; // set everything not connected with the VolumeID to zero,
    auto detelement = volman.lookupDetElement((*m_decoderEndcap).getValue());
    const auto& transformMatrix = detelement.worldTransformation();
    double outGlobal[3];
    fcc_l1 = (*m_decoderEndcap)["x"] * 0.0005;
    fcc_l2 = (*m_decoderEndcap)["z"] * 0.001;
    double inLocal[] = {fcc_l1, 0, fcc_l2};
    transformMatrix.LocalToMaster(inLocal, outGlobal);
    auto edmPos = fcc::Point();
    edmPos.x = outGlobal[0] / dd4hep::mm;
    edmPos.y = outGlobal[1] / dd4hep::mm;
    edmPos.z = outGlobal[2] / dd4hep::mm;

  debug() << "position: x: " << hit.position().x << "\t y: " << hit.position().y << "\t z: " << hit.position().z << endmsg; 
  debug() << "position: x: " << edmPos.x << "\t y: " << edmPos.y << "\t z: " << edmPos.z << endmsg; 
  
  debug() << "phi: " << std::atan2(hit.position().y, hit.position().x) << endmsg;
  debug() << " hit in system: " << system_id;
  layer_id = (*m_decoderEndcap)["disc"];
  debug() << "\t layer " << layer_id;
  module_id = (*m_decoderEndcap)["component"];
  debug() << "\t module " << module_id;
  debug() << endmsg;
  debug() << "position: x: " << hit.position().x << "\t y: " << hit.position().y << "\t z: " << hit.position().z << endmsg; 
  debug() << "position: x: " << edmPos.x + fcc_l1 << "\t y: " << edmPos.y << "\t z: " << edmPos.z + fcc_l2 << endmsg; 

  }
  }


  return StatusCode::SUCCESS;
}

StatusCode CompareTrackHitPositionAndCellId::finalize() {
  return GaudiAlgorithm::finalize();
}
