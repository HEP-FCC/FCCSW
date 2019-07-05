#ifndef RECDRIFTCHAMBER_CREATEDCHHITS_H
#define RECDRIFTCHAMBER_CREATEDCHHITS_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ServiceHandle.h"

#include "FWCore/DataHandle.h"

#include  "datamodel/PositionedTrackHit.h"

#include "DD4hep/Detector.h"
#include "DetSegmentation/GridDriftChamber.h"

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
  int layerId;
  int wireId;
  int trackNum;
  unsigned long cellId;
  double hitLength;
  double radius;
  double debug_zpos;
}; 

class IGeoSvc;

namespace fcc {
class TrackHitCollection;
class PositionedTrackHitCollection;
}

class CreateDCHHits : public GaudiAlgorithm {
public:
  CreateDCHHits(const std::string& name, ISvcLocator* svcLoc);
  ~CreateDCHHits() = default;
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
  static bool sortBasedOnZ(const TVector3 v1, const TVector3 v2)  {
    return v1.Z() < v2.Z();
  }

  static double sumEdep(const double val, const fcc::PositionedTrackHit& h) {
    return  val + h.energy();
  }

  static double sumEdep2(const double val, const hitINFO& h) {
    return val + h.EdepSum;
  }

  static bool sortByTime(const hitINFO h1, const hitINFO h2) {
    return h1.TOF < h2.TOF;
  }

private:
  ServiceHandle<IGeoSvc> m_geoSvc;

  std::unordered_map<uint32_t, std::unordered_map<uint64_t, std::vector<fcc::PositionedTrackHit> > > m_track_cell_hit;
  std::unordered_map<uint64_t, std::vector<hitINFO>> m_wiresHit;

  dd4hep::DDSegmentation::GridDriftChamber* m_segmentation;
  dd4hep::DDSegmentation::BitFieldCoder* m_decoder;

  DataHandle<fcc::PositionedTrackHitCollection> m_positionedHits{"positionedHits", Gaudi::DataHandle::Reader, this};
  DataHandle<fcc::TrackHitCollection> m_mergedTrackHits{"mergedHits", Gaudi::DataHandle::Writer, this};
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "", "Name of the readout (hits collection) to save"};
  Gaudi::Property<std::string> m_outFileName{this, "outFileName", "", "Output filename"};
  Gaudi::Property<double> m_EdepCut{this, "EdepCut", 0.1, "Edep Cut"};
  Gaudi::Property<double> m_DCACut{this, "DCACut", 0.1, "DCACut Cut"};

  DataHandle<std::vector<hitINFO>> m_hitInfoHandle{"DCHitInfo", Gaudi::DataHandle::Writer, this};




};

#endif /* RECDRIFTCHAMBER_CREATEDCHHITS_H */
