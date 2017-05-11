#ifndef RECTRACKER_FASTGAUSSSMEARDIGI_H
#define RECTRACKER_FASTGAUSSSMEARDIGI_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"

class IGeoSvc;

namespace fcc {
class TrackHitCollection;
class PositionedTrackHitCollection;
}

class FastGaussSmearDigi : public GaudiAlgorithm {
public:
  FastGaussSmearDigi(const std::string& name, ISvcLocator* svcLoc);

  ~FastGaussSmearDigi();

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;

  float m_segGridSizeZ;
  float m_segGridSizeX;
  DD4hep::DDSegmentation::BitField64* m_decoder;
  DD4hep::Geometry::VolumeManager m_volman;

  DataHandle<fcc::TrackHitCollection> m_trackHits{"trackHits", Gaudi::DataHandle::Reader, this};
  DataHandle<fcc::PositionedTrackHitCollection> m_smearedTrackHits{"smearedHits", Gaudi::DataHandle::Writer, this};
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "", "Name of the readout (hits collection) to save"};

  Rndm::Numbers m_gaussDist;
};

#endif /* RECTRACKER_FASTGAUSSSMEARDIGI_H */
