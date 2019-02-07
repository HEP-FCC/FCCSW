
#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"


#include "DD4hep/Detector.h"
#include "DD4hep/Volumes.h"
// #include "DDRec/API/IDDecoder.h"
//#include "DDSegmentation/BitField64.h"
#include "DetSegmentation/GridDriftChamber.h"

#include <cmath>
#include <random>

#include "CreateDCHHits.h"
//#include "RecTracker/TrackingUtils.h"


DECLARE_ALGORITHM_FACTORY(CreateDCHHits)



CreateDCHHits::CreateDCHHits(const std::string& name, ISvcLocator* svcLoc) : 
GaudiAlgorithm(name, svcLoc)
{
  declareProperty("mergedHits", m_mergedTrackHits, "Merged Tracker hits (Output)");
  declareProperty("positionedHits", m_positionedHits, "Positioned hits (Input)");
}

StatusCode CreateDCHHits::initialize() {
  info() << "CreateDCHHits initialize" << endmsg;

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
  for (uint itField = 0; itField < m_decoder->size(); itField++) {
    fields.push_back((*m_decoder)[itField].name());
  }
  auto iter = std::find(fields.begin(), fields.end(), "layer");
  if (iter == fields.end()) {
    error() << "Readout does not contain field: 'layer'" << endmsg;
    //    addLayerRestriction = false;
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
  m_tree->Branch("CELLID", &CELLID, "CELLID/I");

  m_tree->Branch("hitLength", &debug_hitLength, "hitLength/D");
  m_tree->Branch("radius", &debug_radius, "radius/D");
  m_tree->Branch("debug_zpos", &debug_zpos, "debug_zpos/D");

  /*
  m_tree->Branch("Radius", &Radius, "Radius/D");
  m_tree->Branch("zpos_tree", &zpos_tree, "zpos_tree/D");
  */
  //  m_tree->Branch("dist_track_wire", &dist_track_wire, "dist_track_wire/D");

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) 
    {
      return sc;
    }

  return sc;
}

StatusCode CreateDCHHits::execute() {
  //  std::cout << "================================== EVENT ==================================" << std::endl;
  // First empty the map: Very important step
  m_track_cell_hit.clear();
  m_wiresHit.clear();

  // get hits from event store
  const fcc::PositionedTrackHitCollection* hits = m_positionedHits.get();
  
  for (const auto& hit : *hits)
    {
      const fcc::BareHit& hitCore = hit.core();

      // std::cout << "TEST1: " << hit.position() << std::endl;

      auto x = hit.position().x;
      auto y = hit.position().y;
      auto z = hit.position().z;  
      auto cellID = hitCore.cellId;
      auto Edep_sum = hitCore.energy;
      auto time = hitCore.time;
      auto trackID = hitCore.bits;

      //      m_decoder->setValue(cellID);
      auto l = m_decoder->get(cellID, "layer"); //(*m_decoder)["layer"].value();
      auto w = m_decoder->get(cellID, "phi");// (*m_decoder)["phi"].value();


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

  // std::cout << "******************* *******************" << std::endl;
  for (const auto& track : m_track_cell_hit)
    {
      auto trackid = track.first;
      for (const auto& cell : track.second)
	{
	  auto cellid = cell.first;

	  int temp_layerId = m_decoder->get(cellid, "layer"); // (*m_decoder)["layer"];
	  int temp_wireId = m_decoder->get(cellid, "phi"); // (*m_decoder)["phi"];


	  auto hits = cell.second;
	  
	  auto hit_start = hits[0];
	  auto hit_end = hits[hits.size()-1];
	  
	  TVector3 h_start(hit_start.position().x*MM_2_CM, hit_start.position().y*MM_2_CM, hit_start.position().z*MM_2_CM);
	  TVector3 h_end(hit_end.position().x*MM_2_CM, hit_end.position().y*MM_2_CM, hit_end.position().z*MM_2_CM);
	  double closestDist = 0.0; // = m_segmentation->distanceTrackWire(cellid, h_start, h_end);
	  TVector3 vec_DCA;

	  if (hits.size() == 1)
	    {
	      // Discard hits with only one Edep calculation
	      vec_DCA=m_segmentation->distanceClosestApproach(cellid, h_start);
	      closestDist = vec_DCA.Mag();
	    }
	  else
	    {
	      closestDist=m_segmentation->distanceTrackWire(cellid, h_start, h_end);
	      vec_DCA = m_segmentation->IntersectionTrackWire(cellid, h_start, h_end);
	      // std::cout << "***** nalipourTest *****" << std::endl;
	      //std::cout << vec_DCA.X() << std::endl;
	      
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

    //std::cout << "index: " << index << ", layerID=" << temp_layerId << ", wireID=" << temp_wireId << std::endl;
    index ++;

    auto hit_info_vec = m_wiresHit[cellid];

    // if (hit_info_vec.size()>1)
    //   {
    // 	std::cout << "SIZE GREATER THAN 1" << std::endl;
    //   }

    std::sort(hit_info_vec.begin(), hit_info_vec.end(), sortByTime);
    auto time_max = std::max_element(hit_info_vec.begin(), hit_info_vec.end(), sortByTime);
    auto time_min = std::min_element(hit_info_vec.begin(), hit_info_vec.end(), sortByTime);

    // std::cout << "size: " << hit_info_vec.size() <<  ", time_min: " << (*time_min).TOF << ", time_max: " << (*time_max).TOF << std::endl;
    if ((time_max-time_min)<400)
      {
	Edep = std::accumulate(hit_info_vec.begin(), hit_info_vec.end(), 0.0, sumEdep2);

	if (hit_info_vec.size()>1)
	  {
	    // std::cout << "size = " << hit_info_vec.size() << std::endl;
	    for (int i = 0; i< hit_info_vec.size(); ++i)
	      {
		// std::cout << "DCA = " << hit_info_vec[i].DCA << std::endl;
	      }
	  }
	if (Edep>m_EdepCut && hit_info_vec[0].DCA<m_DCACut)
	  {
	    layerId = temp_layerId;
	    wireId = temp_wireId;
	    DCA = hit_info_vec[0].DCA;
	    MC_x = hit_info_vec[0].MC_x;
	    MC_y = hit_info_vec[0].MC_y;
	    MC_z = hit_info_vec[0].MC_z;
	    CELLID = cellid;
	    debug_hitLength =  (hit_info_vec[0].hit_end- hit_info_vec[0].hit_start).Mag();
	    debug_radius = (hit_info_vec[0].hit_start).Perp();
	    debug_zpos = (hit_info_vec[0].hit_start).Z();
	    m_tree->Fill();
	  }
      }
    else
      {
	std::cout << "nalipourTEST: GREATER than the integration time" << std::endl;
      }
  }

  //  fcc::TrackHitCollection* edmCellsCollection = new fcc::TrackHitCollection();
  /*
  for (const auto& cell : m_hit) {

    auto cellid = cell.first;
    auto hitpos = m_hit[cellid];
    auto hit_time = m_hit_time[cellid];
    auto track_id = m_hit_trackId[cellid];
 
    m_decoder->setValue(cellid);
    int temp_layerId=(*m_decoder)["layer"];
    int temp_wireId=(*m_decoder)["phi"];


    std::sort(hitpos.begin(), hitpos.end(), sortBasedOnZ);
    auto it_max_z = std::max_element(hitpos.begin(), hitpos.end(), sortBasedOnZ);
    auto it_min_z = std::min_element(hitpos.begin(), hitpos.end(), sortBasedOnZ);
    auto max_z = (*it_max_z).Z();
    auto min_z = (*it_min_z).Z();


    int deltaZ = ceil((max_z - min_z)/1.); // the number of particles (1 cm of time resolution)
    layerId = temp_layerId;
    wireId = temp_wireId;
    nbTimes_wireXhit = deltaZ;
    m_tree->Fill();
  }
  */

//m_mergedTrackHits.put(edmCellsCollection);
  return StatusCode::SUCCESS;
}

StatusCode CreateDCHHits::finalize() {

  m_tree->Write();
  file->Close();

  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
