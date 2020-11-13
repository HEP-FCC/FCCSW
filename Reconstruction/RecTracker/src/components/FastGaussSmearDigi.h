#ifndef RECTRACKER_FASTGAUSSSMEARDIGI_H
#define RECTRACKER_FASTGAUSSSMEARDIGI_H


// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"

class IGeoSvc;

namespace edm4hep {
class SimTrackerHitCollection;
}




class FastGaussSmearDigi : public GaudiAlgorithm {
public:
  FastGaussSmearDigi(const std::string& name, ISvcLocator* svcLoc);

  ~FastGaussSmearDigi() = default;

  StatusCode initialize() override final;

  StatusCode execute() override final;

  StatusCode finalize() override final;

private:
  /// Pointer to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;

  float m_segGridSizeZ;
  float m_segGridSizeX;
  //dd4hep::DDSegmentation::BitField64* m_decoder;
  //dd4hep::VolumeManager m_volman;

  DataHandle<edm4hep::SimTrackerHitsCollection> m_trackHits{"TrackHits", Gaudi::DataHandle::Reader, this};
  DataHandle<edm4hep::SimTrackerHitsCollection> m_smearedTrackHits{"TrackHitsSmeared", Gaudi::DataHandle::Writer, this};
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "", "Name of the readout (hits collection) to save"};

  Rndm::Numbers m_gaussDist;
};

#endif /* RECTRACKER_FASTGAUSSSMEARDIGI_H */
