#ifndef RECDRIFTCHAMBER_CREATEDCHHITS_H
#define RECDRIFTCHAMBER_CREATEDCHHITS_H

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
  double MC_x;
  double MC_y;
  double MC_z;
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

class CreateDCHHits : public GaudiAlgorithm {
public:
  CreateDCHHits(const std::string& name, ISvcLocator* svcLoc);
  ~CreateDCHHits() = default;
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
  bool sortBasedOnZ(const TVector3 v1, const TVector3 v2) const {
    return v1.Z() < v2.Z();
  }

  sumEdep(double val, const fcc::PositionedTrackHit& h)
  {
    double sum = val + h.core().energy;
    return sum;
  }

  static double sumEdep2(double val, const hitINFO& h)
  {
    return val + h.EdepSum;
  }

  static bool sortByTime(hitINFO h1, hitINFO h2)
  {
    return h1.TOF < h2.TOF;
  }

private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;

  /// Map of cell IDs (corresponding to dd4hep IDs) and energy
  /*
  std::unordered_map<uint64_t, double> m_cellsMap;
  std::unordered_map<uint64_t, std::vector<TVector3>> m_hit;
  std::unordered_map<uint64_t, std::vector<double>> m_hit_time;
  std::unordered_map<uint64_t, std::vector<uint32_t>> m_hit_trackId;
  */

  std::unordered_map<uint32_t, std::unordered_map<uint64_t, std::vector<fcc::PositionedTrackHit> > > m_track_cell_hit;
  std::unordered_map<uint64_t, std::vector<hitINFO>> m_wiresHit;
  /*
  std::unordered_map<uint32_t, std::unordered_map<uint64_t, std::vector<double> > > m_track_cell_time;
  std::unordered_map<uint32_t, std::unordered_map<uint64_t, std::vector<double> > > m_track_cell_Edep;
  */

  /* float m_segGridSizeZ; */
  /* float m_segGridSizeX; */
  /*
  dd4hep::VolumeManager m_volman;
  */
  dd4hep::DDSegmentation::GridDriftChamber* m_segmentation;
  dd4hep::DDSegmentation::BitFieldCoder* m_decoder;

  DataHandle<fcc::PositionedTrackHitCollection> m_positionedHits{"positionedHits", Gaudi::DataHandle::Reader, this};
  DataHandle<fcc::TrackHitCollection> m_mergedTrackHits{"mergedHits", Gaudi::DataHandle::Writer, this};
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "", "Name of the readout (hits collection) to save"};
  Gaudi::Property<std::string> m_outFileName{this, "outFileName", "", "Output filename"};
  Gaudi::Property<double> m_EdepCut{this, "EdepCut", 0.1, "Edep Cut"};
  Gaudi::Property<double> m_DCACut{this, "DCACut", 0.1, "DCACut Cut"};

  TFile* file;
  TTree* m_tree;
  int layerId;
  int wireId;
  int nbTimes_wireXhit;
  double DCA;
  int trackNum;
  double MC_x;
  double MC_y;
  double MC_z;
  double debug_hitLength;
  double debug_radius;
  double debug_zpos;
  long long int CELLID;

  double dist_track_wire;
  double Edep;
  double hitTime;
  double Radius;
  double zpos_tree;

  int eventnumber;

};

#endif /* RECDRIFTCHAMBER_CREATEDCHHITS_H */
