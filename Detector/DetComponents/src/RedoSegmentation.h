#ifndef DETCOMPONENTS_REDOSEGMENTATION_H
#define DETCOMPONENTS_REDOSEGMENTATION_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "DetInterface/ISegmentationTool.h"
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
 *  Redo the segmentation.
 *  True positions of the deposits are required!
 *  Positions of the energy deposits are saved in clusters. Cell ID is taken from hit.
 *  Old bitfield needs to be passed as '\b oldReadout'. (TODO take from GeoSvc if empty)
 *  Old segmentation identifiers need to be passed as a vector '\b oldSegmentationIds'. (TODO take from GeoSvc if empty)
 *  New bitfields is set in '\b newReadout'.
 *  New segmentation is taken from tool '\b m_newSegmentationTool'.
 *  [For more information please see](@ref md_rec_doc_commonUtils).
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
  uint64_t volumeID(uint64_t aCellId) const;
private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  ToolHandle<ISegmentationTool> m_segmentationTool;
  //
  DataHandle<fcc::CaloHitCollection> m_inHits;
  DataHandle<fcc::CaloClusterCollection> m_inClusters;
  //
  DataHandle<fcc::CaloHitCollection> m_outHits;
  /* // */
  std::unique_ptr<DD4hep::DDSegmentation::Segmentation> m_segmentation;
  std::string m_readoutName;
  std::string m_oldDecoderString;
  std::vector<std::string> m_oldIdentifiers;
  std::vector<std::string> m_detectorIdentifiers;
  std::unique_ptr<DD4hep::DDSegmentation::BitField64> m_oldDecoder;
};
#endif /* DETCOMPONENTS_REDOSEGMENTATION_H */
