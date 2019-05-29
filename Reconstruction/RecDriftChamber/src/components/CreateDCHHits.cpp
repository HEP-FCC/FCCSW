#include "CreateDCHHits.h"

#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"

#include "DD4hep/Detector.h"
#include "DetSegmentation/GridDriftChamber.h"

DECLARE_ALGORITHM_FACTORY(CreateDCHHits)

CreateDCHHits::CreateDCHHits(const std::string& name, ISvcLocator* svcLoc) : 
GaudiAlgorithm(name, svcLoc)
{
  declareProperty("mergedHits", m_mergedTrackHits, "Merged Tracker hits (Output)");
  declareProperty("positionedHits", m_positionedHits, "Positioned hits (Input)");
}

StatusCode CreateDCHHits::initialize() {
  m_geoSvc = service("GeoSvc");
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
  file=new TFile(TString(m_outFileName), "RECREATE");
  m_tree = new TTree("analysis", "Analysis tree");
  m_tree->Branch("layerId", &layerId, "layerId/I");
  m_tree->Branch("wireId", &wireId, "wireId/I");
  m_tree->Branch("Edep", &Edep, "Edep/D");
  m_tree->Branch("DCA", &DCA, "DCA/D");
  m_tree->Branch("MC_x", &MC_x, "MC_x/D");
  m_tree->Branch("MC_y", &MC_y, "MC_y/D");
  m_tree->Branch("MC_z", &MC_z, "MC_z/D");
  m_tree->Branch("trackNum", &trackNum, "trackNum/I");
  m_tree->Branch("CELLID", &CELLID, "CELLID/I");

  m_tree->Branch("hitLength", &debug_hitLength, "hitLength/D");
  m_tree->Branch("radius", &debug_radius, "radius/D");
  m_tree->Branch("debug_zpos", &debug_zpos, "debug_zpos/D");


  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) 
    {
      return sc;
    }

  return sc;
}

StatusCode CreateDCHHits::execute() {
  // First empty the map: Very important step
  m_track_cell_hit.clear();
  m_wiresHit.clear();
  // get hits from event store
  const fcc::PositionedTrackHitCollection* hits = m_positionedHits.get();
  for (const auto& hit : *hits) {
      const fcc::BareHit& hitCore = hit.core();
      auto x = hit.position().x;
      auto y = hit.position().y;
      auto z = hit.position().z;  
      auto cellID = hitCore.cellId;
      auto Edep_sum = hitCore.energy;
      auto time = hitCore.time;
      auto trackID = hitCore.bits;
      auto l = m_decoder->get(cellID, "layer"); 
      auto w = m_decoder->get(cellID, "phi");


      // TVector3 hitPos(x*MM_2_CM, y*MM_2_CM, z*MM_2_CM);
      //      auto DCA = m_segmentation->distanceClosestApproach(cellID, hitPos); // cm //*CM_2_MM;

      /*
      m_cellsMap[cellID] += Edep_sum;
      m_hit[cellID].push_back(hitPos);
      m_hit_time[cellID].push_back(time);
      m_hit_trackId[cellID].push_back(trackID);
      */

      
      m_track_cell_hit[trackID][cellID].push_back(hit);
    }


  for (const auto& track : m_track_cell_hit) {
    auto trackid = track.first;
    for (const auto& cell : track.second) {
      auto cellid = cell.first;
      int temp_layerId = m_decoder->get(cellid, "layer");
      int temp_wireId = m_decoder->get(cellid, "phi");
      auto hits = cell.second;
      auto hit_start = hits[0];
      auto hit_end = hits[hits.size()-1];
      TVector3 h_start(hit_start.position().x*MM_2_CM, hit_start.position().y*MM_2_CM, hit_start.position().z*MM_2_CM);
      TVector3 h_end(hit_end.position().x*MM_2_CM, hit_end.position().y*MM_2_CM, hit_end.position().z*MM_2_CM);
      double closestDist = 0.0; // = m_segmentation->distanceTrackWire(cellid, h_start, h_end);
      TVector3 vec_DCA;
      if (hits.size() == 1) {
	      // Discard hits with only one Edep calculation
	      vec_DCA=m_segmentation->distanceClosestApproach(cellid, h_start);
	      closestDist = vec_DCA.Mag();
	    }
      else {
	      closestDist=m_segmentation->distanceTrackWire(cellid, h_start, h_end);
	      vec_DCA = m_segmentation->IntersectionTrackWire(cellid, h_start, h_end);
	      hitINFO hinfo;
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
  int index = 0;
  for (const auto& cell : m_wiresHit) {
    auto cellid = cell.first;
    int temp_layerId = m_decoder->get(cellid, "layer");
    int temp_wireId = m_decoder->get(cellid, "phi");
    index ++;
    auto hit_info_vec = m_wiresHit[cellid];
    std::sort(hit_info_vec.begin(), hit_info_vec.end(), sortByTime);
    auto time_max = std::max_element(hit_info_vec.begin(), hit_info_vec.end(), sortByTime);
    auto time_min = std::min_element(hit_info_vec.begin(), hit_info_vec.end(), sortByTime);
    if ((time_max-time_min)<400) {
	    Edep = std::accumulate(hit_info_vec.begin(), hit_info_vec.end(), 0.0, sumEdep2);
	    if (hit_info_vec.size()>1) {
        for (int i = 0; i< hit_info_vec.size(); ++i) {
	      }
      }
      if (Edep>m_EdepCut && hit_info_vec[0].DCA<m_DCACut) {
        layerId = temp_layerId;
        wireId = temp_wireId;
        DCA = hit_info_vec[0].DCA;
        trackNum = eventnumber;
        MC_x = hit_info_vec[0].MC_x;
        MC_y = hit_info_vec[0].MC_y;
        MC_z = hit_info_vec[0].MC_z;
        CELLID = cellid;
        debug_hitLength =  (hit_info_vec[0].hit_end- hit_info_vec[0].hit_start).Mag();
        debug_radius = (hit_info_vec[0].hit_start).Perp();
        debug_zpos = (hit_info_vec[0].hit_start).Z();
        m_tree->Fill();
      }
    } else {
      debug() << "GREATER than the integration time" << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode CreateDCHHits::finalize() {
  m_tree->Write();
  file->Close();
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
