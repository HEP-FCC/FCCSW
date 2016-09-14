#ifndef TESTRECONSTRUCTION_TESTCELLCOUNTING_H
#define TESTRECONSTRUCTION_TESTCELLCOUNTING_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;

/** @class TestCellCounting TestReconstruction/src/TestCellCounting.h TestCellCounting.h
 *
 *  Algorithm testing the cell counting.
 *  It uses the functions from Detector/DetCommon/DetUtils.h to count the number of volumes and cells
 *  for various segmentations.
 *  Example job options can be found in Test/TestReconstruction/tests/options.
 *
 *  @author Anna Zaborowska
 *  @author Valentin Volkl
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
  /// Name of the fields describing the segmented volume
  std::vector<std::string> m_fieldNames;
  /// Values of the fields describing the segmented volume
  std::vector<int> m_fieldValues;
  /// Name (or its part) for the volume counting
  std::string m_volumeMatchName;
  /// Volume ID of the volume with cells to calculate
  uint64_t m_volumeId;
};
#endif /* TESTRECONSTRUCTION_TESTCELLCOUNTING_H */
