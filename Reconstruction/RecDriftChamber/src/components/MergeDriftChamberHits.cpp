#include "MergeDriftChamberHits.h"

#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHit.h"
#include "datamodel/TrackHitCollection.h"

DECLARE_COMPONENT(MergeDriftChamberHits)

MergeDriftChamberHits::MergeDriftChamberHits(const std::string& name, ISvcLocator* svcLoc) : 
GaudiAlgorithm(name, svcLoc)
{
  declareProperty("DriftChamberHitsPositions", m_positionedHitsHandle_in, "Positioned hits (Input)");
  declareProperty("DriftChamberHitsMerged", m_mergedHitsHandle_out, "Merged TrackerHit collection (output)");
  declareProperty("DriftChamberHitsPositionsMerged", m_mergedHitsPositionsHandle_out, "Merged PositionedTrackerHits (Output)");
}

StatusCode MergeDriftChamberHits::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  return sc;
}

StatusCode MergeDriftChamberHits::execute() {
  m_track_cell_hit.clear();
  fcc::TrackHitCollection* trackhits = m_mergedHitsHandle_out.createAndPut();
  fcc::PositionedTrackHitCollection* positionedtrackhits = m_mergedHitsPositionsHandle_out.createAndPut();
  // get hits from event store
  const fcc::PositionedTrackHitCollection* hits = m_positionedHitsHandle_in.get();
  // fill map
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
        
        double totalEdep = 0;
        double average_x = 0.0;
        double average_y = 0.0;
        double average_z = 0.0;
        for (unsigned i = 0; i < hits.size(); ++i) {
            totalEdep += hits[i].core().energy;
            average_x += hits[i].position().x;
            average_y += hits[i].position().y;
            average_z += hits[i].position().z;
          }
        average_x = average_x / double(hits.size());
        average_y = average_y / double(hits.size());
        average_z = average_z / double(hits.size());
        if (totalEdep > 1e-6) { // 1 keV
	      auto trackHitPosition = fcc::Point();
	      trackHitPosition.x = average_x;
	      trackHitPosition.y = average_y;
	      trackHitPosition.z = average_z;
	      auto edmTrackHit = trackhits->create();
	      auto& edmTrackHitCore = edmTrackHit.core();
	      edmTrackHitCore.cellId = cellid;
        edmTrackHitCore.bits = trackid;
	      edmTrackHitCore.energy = totalEdep;
	      positionedtrackhits->create(trackHitPosition, edmTrackHitCore);
	    }
	  }
  }
  return StatusCode::SUCCESS;
}

StatusCode MergeDriftChamberHits::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
