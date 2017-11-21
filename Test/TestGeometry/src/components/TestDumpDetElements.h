#ifndef TESTRECONSTRUCTION_TESTCELLCOUNTING_H
#define TESTRECONSTRUCTION_TESTCELLCOUNTING_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;


class TestDumpDetElements : public GaudiAlgorithm {
public:
  explicit TestDumpDetElements(const std::string&, ISvcLocator*);
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
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "Name of the detecotr readout"};
  /// Name of the bit-fields (in the readout) that contain the volume to segment
  Gaudi::Property<std::vector<std::string>> m_fieldNames{
      this, "fieldNames", {}, "Name of the bit-fields (in the readout) that contains the volume to segment"};
  /// Values of the fields that identify the volume to be segmented (e.g. ID of the ECal)
  Gaudi::Property<std::vector<int>> m_fieldValues{
      this, "fieldValues", {}, "Value of the field that identifies the volume to be segmented (e.g. ID of the ECal)"};
  /// Name (or its part) for the volume counting
  Gaudi::Property<std::string> m_volumeMatchName{this, "volumeMatchName", "Silicon", "Name for the volume counting"};
  /// Volume ID of the volume with cells to calculate
  uint64_t m_volumeId;
};
#endif /* TESTRECONSTRUCTION_TESTCELLCOUNTING_H */
