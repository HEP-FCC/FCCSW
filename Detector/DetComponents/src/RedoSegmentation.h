#ifndef DETCOMPONENTS_REDOSEGMENTATION_H
#define DETCOMPONENTS_REDOSEGMENTATION_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;

// DD4hep
#include "DD4hep/Readout.h"
namespace DD4hep {
namespace DDSegmentation {
  class Segmentation;
}
}

// datamodel
namespace fcc {
class CaloClusterCollection;
class CaloHitCollection;
}

/** @class RedoSegmentation Detector/DetComponents/src/RedoSegmentation.h RedoSegmentation.h
 *
 *  Redo the segmentation after the simulation has ended.
 *  True positions of the deposits are required!
 *  Positions of the energy deposits are saved in clusters. Cell ID is taken from bits in cluster.
 *  New readout (with new segmentation) has to be added to <readouts> tag in the detector description xml.
 *  Cell IDs are goign to be rewritten from the old readout (`\b oldReadoutName`) to the new readout (`\b newReadoutName`).
 *  Names of the old segmentation fields need to be passed as a vector '\b oldSegmentationIds'.
 *  Those fields are going to be replaced by the new segmentation.
 *  For an example see Detector/DetComponents/tests/options/redoSegmentationXYZ.py
 *  and Detector/DetComponents/tests/options/redoSegmentationRPhi.py.
 *
 *  @author Anna Zaborowska
 */

class RedoSegmentation: public GaudiAlgorithm {
public:
  explicit RedoSegmentation(const std::string&, ISvcLocator*);
  virtual ~RedoSegmentation();
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
  /**  Get ID of the volume that contains the cell.
   *   @param[in] aCellId ID of the cell.
   *   @return ID of the volume.
   */
  uint64_t volumeID(uint64_t aCellId) const;
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Handle for the EDM clusters to be read
  DataHandle<fcc::CaloClusterCollection> m_inClusters;
  /// Handle for the EDM hits to be written
  DataHandle<fcc::CaloHitCollection> m_outHits;
  /// New segmentation
  DD4hep::DDSegmentation::Segmentation* m_segmentation;
  /// Name of the detector readout used in simulation
  std::string m_oldReadoutName;
  /// Name of the new detector readout
  std::string m_newReadoutName;
  /// Old bitfield decoder
  std::shared_ptr<DD4hep::DDSegmentation::BitField64> m_oldDecoder;
  /// Segmentation fields that are going to be replaced by the new segmentation
  std::vector<std::string> m_oldIdentifiers;
  /// Detector fields that are going to be rewritten
  std::vector<std::string> m_detectorIdentifiers;
};
#endif /* DETCOMPONENTS_REDOSEGMENTATION_H */
