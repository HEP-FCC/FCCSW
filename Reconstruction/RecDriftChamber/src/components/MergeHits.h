#ifndef RECDRIFTCHAMBER_MERGEHITS_H
#define RECDRIFTCHAMBER_MERGEHITS_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolHandle.h"

#include "datamodel/TrackHit.h"
#include "datamodel/TrackHitCollection.h"

// FCCSW
#include "FWCore/DataHandle.h"

// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TVector3.h"

struct hitINFO {
  double DCA;
  double DCA_x;
  double DCA_y;
  double DCA_z;
  double EdepSum;
  double TOF;
  TVector3 hit_start;
  TVector3 hit_end;
}; 

class IGeoSvc;
/*
namespace fcc {
class TrackHitCollection;
class PositionedTrackHitCollection;
}
*/

class MergeHits : public GaudiAlgorithm {
public:
  MergeHits(const std::string& name, ISvcLocator* svcLoc);

  ~MergeHits() = default;

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();


private:
  /// Pointer to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;


  std::unordered_map<uint32_t, std::unordered_map<uint64_t, std::vector<fcc::PositionedTrackHit> > > m_track_cell_hit;

  DataHandle<fcc::PositionedTrackHitCollection> m_positionedHits{"positionedHits", Gaudi::DataHandle::Reader, this};
  DataHandle<fcc::PositionedTrackHitCollection> m_mergedHits{"mergedHits", Gaudi::DataHandle::Writer, this};
  /// Handle for the EDM Hits to be read
  DataHandle<fcc::TrackHitCollection> m_trackHitHandle{"trackHits", Gaudi::DataHandle::Writer, this};



};

#endif /* RECDRIFTCHAMBER_MERGEHITS_H */
