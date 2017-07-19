#ifndef TESTFWCORE_CREATEEXAMPLEEVENTDATA
#define TESTFWCORE_CREATEEXAMPLEEVENTDATA

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/Property.h"

// FCCSW
#include "FWCore/DataHandle.h"



// datamodel 
namespace fcc {
class MCParticleCollection;
class GenVertexCollection;
class CaloHitCollection;
class PositionedCaloHitCollection;
class TrackHitCollection;
class PositionedTrackHitCollection;
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
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Execute.
   *   @return status code
   */
  virtual StatusCode execute() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;

private:
  /// integer to add to the dummy values written to the edm
  Gaudi::Property<int> m_magicNumberOffset{this, "magicNumberOffset", 0, "Integer to add to the dummy values written to the edm"};
  /// Handle for the genparticles to be written
  DataHandle<fcc::MCParticleCollection> m_genParticleHandle{"genParticles", Gaudi::DataHandle::Writer, this};
  /// Handle for the genvertices to be written
  DataHandle<fcc::GenVertexCollection> m_genVertexHandle{"genVertices", Gaudi::DataHandle::Writer, this};
  /// Handle for the EDM Hits to be read
  DataHandle<fcc::CaloHitCollection> m_caloHitHandle{"caloHits", Gaudi::DataHandle::Writer, this};
  /// Handle for the EDM Hits to be read
  DataHandle<fcc::PositionedCaloHitCollection> m_posCaloHitHandle{"positionedCaloHits", Gaudi::DataHandle::Writer, this};
  /// Handle for the EDM Hits to be read
  DataHandle<fcc::TrackHitCollection> m_trackHitHandle{"trackHits", Gaudi::DataHandle::Writer, this};
  /// Handle for the EDM Hits to be read
  DataHandle<fcc::PositionedTrackHitCollection> m_posTrackHitHandle{"positionedTrackHits", Gaudi::DataHandle::Writer, this};
};
#endif /* TESTFWCORE_CREATEEXAMPLEEVENTDATA */
