#ifndef TESTRECONSTRUCTION_TESTNEIGHBOURS_H
#define TESTRECONSTRUCTION_TESTNEIGHBOURS_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/Property.h"

// FCCSW
#include "FWCore/DataHandle.h"

class IGeoSvc;

// DD4hep
namespace DD4hep {
namespace DDSegmentation {
class BitField64;
}
}

// datamodel
namespace fcc {
class CaloHitCollection;
}

/** @class TestNeighbours TestReconstruction/src/TestNeighbours.h TestNeighbours.h
 *
 *  Algorithm testing lookup of neighbours
 *
 *  @author Anna Zaborowska
 */

class TestNeighbours : public GaudiAlgorithm {
public:
  explicit TestNeighbours(const std::string&, ISvcLocator*);
  virtual ~TestNeighbours();
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
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Handle for the EDM Hits to be read
  DataHandle<fcc::CaloHitCollection> m_inHits{"hits/caloInHits", Gaudi::DataHandle::Reader, this};
  /// Name of the detector readout
  Gaudi::Property<std::string> m_readoutName{this, "readout", "", "Name of the detector readout"};
  /// Pointer to the bitfield decoder
  std::shared_ptr<DD4hep::DDSegmentation::BitField64> m_decoder;
  /// Names of the fields for which neighbours are found
  std::vector<std::string> m_fieldNames;
  /// Minimal and maximal values of the fields for which neighbours are found
  std::vector<std::pair<int, int>> m_fieldExtremes;
  /// Defines how many steps
  Gaudi::Property<int> m_steps{this, "range", {1}, "Defines the range to look for neighbours"};
};
#endif /* TESTRECONSTRUCTION_TESTNEIGHBOURS_H */
