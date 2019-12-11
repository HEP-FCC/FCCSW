#ifndef TESTFWCORE_CREATEEXAMPLEEVENTDATA
#define TESTFWCORE_CREATEEXAMPLEEVENTDATA

#include "GaudiAlg/GaudiAlgorithm.h"
#include "FWCore/DataHandle.h"

// datamodel 
namespace fcc {
class MCParticleCollection;
class GenVertexCollection;
class CaloHitCollection;
class PositionedCaloHitCollection;
class TrackHitCollection;
class PositionedTrackHitCollection;
class FloatValueData;
}

/** @class CreateExampleEventData
 *  Lightweight producer for edm data for tests that do not depend on the actual
 *  data content and therefore do not need the simulation machinery.
 *  Fills data members with increasing integers, together with some offset so that different
 *  events can be easily distinguished. 
 *
 */
class CreateExampleEventData : public GaudiAlgorithm {
public:
  explicit CreateExampleEventData(const std::string&, ISvcLocator*);
  virtual ~CreateExampleEventData();
  virtual StatusCode initialize() final;
  virtual StatusCode execute() final;
  virtual StatusCode finalize() final;

private:
  /// integer to add to the dummy values written to the edm
  Gaudi::Property<int> m_magicNumberOffset{this, "magicNumberOffset", 0, "Integer to add to the dummy values written to the edm"};
  /// Handle for the genparticles to be written
  DataHandle<fcc::MCParticleCollection> m_genParticleHandle{"GenParticles", Gaudi::DataHandle::Writer, this};
  /// Handle for the genvertices to be written
  DataHandle<fcc::GenVertexCollection> m_genVertexHandle{"GenVertices", Gaudi::DataHandle::Writer, this};
  /// Handle for the EDM Hits to be read
  DataHandle<fcc::CaloHitCollection> m_caloHitHandle{"SimCaloHits", Gaudi::DataHandle::Writer, this};
  /// Handle for the EDM Hits to be read
  DataHandle<fcc::PositionedCaloHitCollection> m_posCaloHitHandle{"SimCaloHitsPositions", Gaudi::DataHandle::Writer, this};
  /// Handle for the EDM Hits to be read
  DataHandle<fcc::TrackHitCollection> m_trackHitHandle{"SimTrackHits", Gaudi::DataHandle::Writer, this};
  /// Handle for the EDM Hits to be read
  DataHandle<fcc::PositionedTrackHitCollection> m_posTrackHitHandle{"SimTrackHitsPositions", Gaudi::DataHandle::Writer, this};

  DataHandle<std::vector<fcc::FloatValueData>> m_somefloatHandle{"SomeFloatValueDataVec", Gaudi::DataHandle::Writer, this};
  DataHandle<std::vector<float>> m_somefloatHandle2{"SomeFloatVec", Gaudi::DataHandle::Writer, this};
  DataHandle<std::vector<int>> m_someinthandle{"SomeIntVec", Gaudi::DataHandle::Writer, this};
  DataHandle<Int_t> m_intHandle{"SomeInt_t", Gaudi::DataHandle::Writer, this};
  DataHandle<float> m_floatHandle{"SomeFloat", Gaudi::DataHandle::Writer, this};
  DataHandle<double> m_doubleHandle{"someDouble", Gaudi::DataHandle::Writer, this};
};
#endif /* TESTFWCORE_CREATEEXAMPLEEVENTDATA */
