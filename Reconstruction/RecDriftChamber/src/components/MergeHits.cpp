
#include "DetInterface/IGeoSvc.h"

#include "datamodel/PositionedTrackHitCollection.h"


#include "DD4hep/Detector.h"
#include "DD4hep/Volumes.h"
#include "DetSegmentation/GridDriftChamber.h"

#include <cmath>
#include <random>

#include "MergeHits.h"



DECLARE_ALGORITHM_FACTORY(MergeHits)



MergeHits::MergeHits(const std::string& name, ISvcLocator* svcLoc) : 
GaudiAlgorithm(name, svcLoc), m_geoSvc("GeoSvc", name)
{
  declareProperty("positionedHits", m_positionedHits, "Positioned hits (Input)");
  declareProperty("mergedHits", m_mergedHits, "Merged Tracker hits (Output)");
  declareProperty("trackHits", m_trackHitHandle, "Dummy Hit collection (output)");
}

StatusCode MergeHits::initialize() {
  info() << "MergeHits initialize" << endmsg;

  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the "
               "configuration."
            << endmsg;
    return StatusCode::FAILURE;
  }


  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) 
    {
      return sc;
    }
  return sc;
}

StatusCode MergeHits::execute() {
  m_track_cell_hit.clear();

  // get hits from event store
  const fcc::PositionedTrackHitCollection* hits = m_positionedHits.get();
  
  for (const auto& hit : *hits)
    {
      const fcc::BareHit& hitCore = hit.core();

      auto x = hit.position().x;
      auto y = hit.position().y;
      auto z = hit.position().z;  
      auto cellID = hitCore.cellId;
      auto Edep_sum = hitCore.energy;
      auto time = hitCore.time;
      auto trackID = hitCore.bits;

      m_track_cell_hit[trackID][cellID].push_back(hit);
    }
  fcc::TrackHitCollection* trackhits = m_trackHitHandle.createAndPut();
  fcc::PositionedTrackHitCollection* positionedtrackhits = m_mergedHits.createAndPut();

  for (const auto& track : m_track_cell_hit)
    {
      auto trackid = track.first;
      //  fcc::TrackHitCollection* edmCellsCollection = new fcc::TrackHitCollection();
      for (const auto& cell : track.second)
	{
	  auto cellid = cell.first;
	  auto hits = cell.second;
	  
	  double totalEdep = 0;
	  double average_x = 0.0;
	  double average_y = 0.0;
	  double average_z = 0.0;
	  for (int i = 0; i<hits.size(); ++i)
	    {
	      totalEdep += hits[i].core().energy;
	      average_x += hits[i].position().x;
	      average_y += hits[i].position().y;
	      average_z += hits[i].position().z;
	    }
	  average_x = average_x / double(hits.size());
	  average_y = average_y / double(hits.size());
	  average_z = average_z / double(hits.size());

	  if (totalEdep > 1e-6) // 1 keV
	    {
	      auto trackHitPosition = fcc::Point();
	      trackHitPosition.x = average_x;
	      trackHitPosition.y = average_y;
	      trackHitPosition.z = average_z;
	      auto edmTrackHit = trackhits->create();
	      auto& edmTrackHitCore = edmTrackHit.core();
	      edmTrackHitCore.cellId = cellid;
	      edmTrackHitCore.energy = totalEdep;
	      positionedtrackhits->create(trackHitPosition, edmTrackHitCore);
	    }
	  
	}
    }


//m_mergedTrackHits.put(edmCellsCollection);
  return StatusCode::SUCCESS;
}

StatusCode MergeHits::finalize() {


  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
