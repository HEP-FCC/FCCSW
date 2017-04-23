#include "CreateVolumePositions.h"

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

typedef CreateVolumePositions<fcc::CaloHitCollection, fcc::PositionedCaloHitCollection> CreateVolumeCaloPositions;
typedef CreateVolumePositions<fcc::TrackHitCollection, fcc::PositionedTrackHitCollection> CreateVolumeTrackPositions;
DECLARE_ALGORITHM_FACTORY(CreateVolumeCaloPositions)
DECLARE_COMPONENT_WITH_ID(CreateVolumeCaloPositions, "CreateVolumeCaloPositions")
DECLARE_ALGORITHM_FACTORY(CreateVolumeTrackPositions)
DECLARE_COMPONENT_WITH_ID(CreateVolumeTrackPositions, "CreateVolumeTrackPositions")

template <class H, class P>
CreateVolumePositions<H, P>::CreateVolumePositions(const std::string& name, ISvcLocator* svcLoc)
    : GaudiAlgorithm(name, svcLoc) {
  declareProperty("hits", m_hits, "Hit collection (input)");
  declareProperty("positionedHits", m_positionedHits, "Positions of centres of volumes (output)");
}

template <class H, class P>
StatusCode CreateVolumePositions<H, P>::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. " << endmsg;
    return StatusCode::FAILURE;
  }
  return sc;
}

template <class H, class P>
StatusCode CreateVolumePositions<H, P>::execute() {

  // Get the input hit collection
  const H* hits = m_hits.get();
  debug() << "Input hit collection size: " << hits->size() << endmsg;

  // Initialize output collection
  auto edmPositionedHitCollection = m_positionedHits.createAndPut();

  uint64_t cellid = 0;
  DD4hep::Geometry::VolumeManager volman = m_geoSvc->lcdd()->volumeManager();

  // Loop though hits, retrieve volume position from cellID
  for (const auto& cell : *hits) {
    cellid = cell.core().cellId;
    auto detelement = volman.lookupDetElement(cellid);
    const auto& transformMatrix = detelement.worldTransformation();
    double outGlobal[3];
    double inLocal[] = {0, 0, 0};
    transformMatrix.LocalToMaster(inLocal, outGlobal);
    auto edmPos = fcc::Point();
    edmPos.x = outGlobal[0] / dd4hep::mm;
    edmPos.y = outGlobal[1] / dd4hep::mm;
    edmPos.z = outGlobal[2] / dd4hep::mm;

    auto positionedHit = edmPositionedHitCollection->create(edmPos, cell.core());

    // Debug information about volume position
    debug() << "Hit energy" << cell.core().energy << "\tcellID " << cellid << endmsg;
    debug() << "Position of volume (mm) : \t" << outGlobal[0] / dd4hep::mm << "\t" << outGlobal[1] / dd4hep::mm << "\t"
            << outGlobal[2] / dd4hep::mm << endmsg;
  }
  debug() << "Output positions collection size: " << edmPositionedHitCollection->size() << endmsg;

  return StatusCode::SUCCESS;
}

template <class H, class P>
StatusCode CreateVolumePositions<H, P>::finalize() {
  return GaudiAlgorithm::finalize();
}
