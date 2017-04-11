#include "CreateVolumePositions.h"

//FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"

// EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DD4hep/Volumes.h"
#include "TGeoManager.h"

DECLARE_ALGORITHM_FACTORY(CreateVolumePositions)

CreateVolumePositions::CreateVolumePositions(const std::string& name, ISvcLocator* svcLoc)
  : GaudiAlgorithm(name, svcLoc) {
  declareInput("caloCells", m_caloCells);
  declareOutput("caloPositionedHits", m_caloPositionedHits);
  declareProperty("readoutName", m_readoutName = "");
}

StatusCode CreateVolumePositions::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  m_geoSvc = service ("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  // check if readouts exist
  if(m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error()<<"Readout <<"<<m_readoutName<<">> does not exist."<<endmsg;
    return StatusCode::FAILURE;
  }

  return sc;
}

StatusCode CreateVolumePositions::execute() {

  //Get the input caloHits collection
  const fcc::CaloHitCollection* calocells = m_caloCells.get();
  debug() << "Input caloCells collection size: " << calocells->size() << endmsg;

  //Initialize output CaloClusterCollection
  auto edmPositionedHitCollection = m_caloPositionedHits.createAndPut();

  uint64_t cellid = 0;
  DD4hep::Geometry::VolumeManager volman = m_geoSvc->lcdd()->volumeManager();
  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();

  // Loop though hits, retrieve volume position from cellID
  for (const auto& cell : *calocells) {
    cellid = cell.core().cellId;
    const auto& transformMatrix = volman.worldTransformation(cellid);
    double outGlobal[3];
    double inLocal[] = {0,0,0};
    transformMatrix.LocalToMaster(inLocal, outGlobal);
    auto edmPos = fcc::Point();
    edmPos.x = outGlobal[0] / dd4hep::mm;
    edmPos.y = outGlobal[1] / dd4hep::mm;
    edmPos.z = outGlobal[2] / dd4hep::mm;

    auto positionedHit = edmPositionedHitCollection->create(edmPos, cell.core());

    // Debug information about cells
    if(msgLevel(MSG::DEBUG)) {
      decoder->setValue(cellid);
      verbose() << decoder->valueString() << " \tenergy "<< cell.core().energy << "\tcellID " << cellid << endmsg;
      debug() << "translation of cell volume (mm) : \t" << outGlobal[0] / dd4hep::mm << "\t" << outGlobal[1] / dd4hep::mm << "\t" << outGlobal[2] / dd4hep::mm << endmsg;
    }
  }
  debug() << "Output Hit Positions collection size: " << edmPositionedHitCollection->size() << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode CreateVolumePositions::finalize() {
  return GaudiAlgorithm::finalize();
}
