#include "CreateDCHHits.h"

#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"

#include "DD4hep/Detector.h"
#include "DetSegmentation/GridDriftChamber.h"


// todo: review unit handling
#ifdef HAVE_GEANT4_UNITS
#define MM_2_CM 1.0
#define CM_2_MM 1.0
#else
#define MM_2_CM 0.1
#define CM_2_MM 10.0
#endif




DECLARE_COMPONENT(CreateDCHHits)

CreateDCHHits::CreateDCHHits(const std::string& name, ISvcLocator* svcLoc) : 
GaudiAlgorithm(name, svcLoc), m_geoSvc("GeoSvc", name)
{
  declareProperty("mergedHits", m_mergedTrackHits, "Merged Tracker hits (Output)");
  declareProperty("positionedHits", m_positionedHits, "Positioned hits (Input)");
  declareProperty("DCHitInfo", m_hitInfoHandle, "Debug DC Hit Info (Output)");
}

StatusCode CreateDCHHits::initialize() {
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the "
               "configuration."
            << endmsg;
    return StatusCode::FAILURE;
  }
  // check if readouts exist
  if (m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout <<" << m_readoutName << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  // retrieve  segmentation
  m_segmentation = dynamic_cast<dd4hep::DDSegmentation::GridDriftChamber*>(
      m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if (m_segmentation == nullptr) {
    error() << "There is no drift chamber segmentation." << endmsg;
    return StatusCode::FAILURE;
  }
  // Take readout bitfield decoder from GeoSvc
  m_decoder =
      dynamic_cast<dd4hep::DDSegmentation::BitFieldCoder*>(m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder());
  // check if decoder contains "layer"
  std::vector<std::string> fields;
  for (size_t itField = 0; itField < m_decoder->size(); itField++) {
    fields.push_back((*m_decoder)[itField].name());
  }
  auto iter = std::find(fields.begin(), fields.end(), "layer");
  if (iter == fields.end()) {
    error() << "Readout does not contain field: 'layer'" << endmsg;
  }
  // Initialise the output file


  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) 
    {
      return sc;
    }

  return sc;
}

StatusCode CreateDCHHits::execute() {
  std::vector<hitINFO>* hitInfos = m_hitInfoHandle.createAndPut();
  m_track_cell_hit.clear();
  m_wiresHit.clear();
  const fcc::PositionedTrackHitCollection* hits = m_positionedHits.get();
  for (const auto& hit : *hits) {
    const fcc::BareHit& hitCore = hit.core();
    auto cellID = hitCore.cellId;
    auto trackID = hitCore.bits;
    m_track_cell_hit[trackID][cellID].push_back(hit);
  }
  for (const auto& track : m_track_cell_hit) {
    auto trackid = track.first;
    for (const auto& cell : track.second) {
      auto cellid = cell.first;
      auto hits = cell.second;
      auto hit_start = hits[0];
      auto hit_end = hits[hits.size()-1];
      TVector3 h_start(hit_start.position().x*MM_2_CM, hit_start.position().y*MM_2_CM, hit_start.position().z*MM_2_CM);
      TVector3 h_end(hit_end.position().x*MM_2_CM, hit_end.position().y*MM_2_CM, hit_end.position().z*MM_2_CM);
      double closestDist = 0.0;
      TVector3 vec_DCA;
      if (hits.size() == 1) {
	      // Discard hits with only one Edep calculation
	      vec_DCA=m_segmentation->distanceClosestApproach(cellid, h_start);
	      closestDist = vec_DCA.Mag();
	    } else {
	      closestDist=m_segmentation->distanceTrackWire(cellid, h_start, h_end);
	      vec_DCA = m_segmentation->IntersectionTrackWire(cellid, h_start, h_end);
	      hitINFO hinfo;
        hinfo.trackNum = trackid;
	      hinfo.DCA = closestDist;
	      hinfo.MC_x = vec_DCA.X()*CM_2_MM;
	      hinfo.MC_y = vec_DCA.Y()*CM_2_MM;
	      hinfo.MC_z = vec_DCA.Z()*CM_2_MM;
	      hinfo.EdepSum = std::accumulate(hits.begin(), hits.end(), 0.0, sumEdep);
	      hinfo.TOF = hit_start.core().time;
	      hinfo.hit_start = h_start;
	      hinfo.hit_end = h_end;
	      m_wiresHit[cellid].push_back(hinfo);
	    }
	  }
  }
  for (const auto& cell : m_wiresHit) {
    auto cellid = cell.first;
    int temp_layerId = m_decoder->get(cellid, "layer");
    int temp_wireId = m_decoder->get(cellid, "phi");
    auto hit_info_vec = m_wiresHit[cellid];
    std::sort(hit_info_vec.begin(), hit_info_vec.end(), sortByTime);
    auto time_max = std::max_element(hit_info_vec.begin(), hit_info_vec.end(), sortByTime);
    auto time_min = std::min_element(hit_info_vec.begin(), hit_info_vec.end(), sortByTime);
    if ((time_max-time_min)<400) {
      auto hit_info0 = hit_info_vec[0];
	    double Edep = std::accumulate(hit_info_vec.begin(), hit_info_vec.end(), 0.0, sumEdep2);
	    if (hit_info_vec.size()>1) {
        // todo?
      }
      if (Edep > m_EdepCut && hit_info0.DCA < m_DCACut) {
        hit_info0.layerId = temp_layerId;
        hit_info0.wireId = temp_wireId;
        hit_info0.cellId = cellid;
        hit_info0.hitLength =  (hit_info0.hit_end- hit_info0.hit_start).Mag();
        hit_info0.radius = (hit_info0.hit_start).Perp();
        hit_info0.debug_zpos = (hit_info0.hit_start).Z();
        hitInfos->emplace_back(hit_info0);
      }
    } else {
      debug() << "GREATER than the integration time" << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode CreateDCHHits::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
