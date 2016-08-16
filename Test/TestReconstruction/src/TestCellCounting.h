#ifndef TESTRECONSTRUCTION_TESTCELLCOUNTING_H
#define TESTRECONSTRUCTION_TESTCELLCOUNTING_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;

/** @class TestCellCounting TestGeometry/src/components/TestCellCounting.h TestCellCounting.h
 *
 *  Algorithm testing the cell counting.
 *
 *  @author Anna Zaborowska
 */

class TestCellCounting: public GaudiAlgorithm {
public:
  explicit TestCellCounting(const std::string&, ISvcLocator*);
  virtual ~TestCellCounting();
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
  /// Name of the detector readout
  std::string m_readoutName;
  /// Name of the detector readout
  std::vector<std::string> m_fieldNames;
  /// Name of the detector readout
  std::vector<int> m_fieldValues;
  std::string m_volumeMatchName;
  // Volume ID of the volume with cells to calculate
  uint64_t m_volumeId;
};
#endif /* TESTRECONSTRUCTION_TESTCELLCOUNTING_H */
