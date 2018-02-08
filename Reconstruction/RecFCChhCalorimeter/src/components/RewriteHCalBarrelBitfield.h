#ifndef DETCOMPONENTS_REWRITEHCALBARRELBITFIELD_H
#define DETCOMPONENTS_REWRITEHCALBARRELBITFIELD_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "DetSegmentation/FCCSWGridPhiEta.h"
#include "FWCore/DataHandle.h"
class IGeoSvc;

// DD4hep
#include "DD4hep/Readout.h"
#include "DDSegmentation/Segmentation.h"
namespace DD4hep {
namespace DDSegmentation {
class Segmentation;
}
}

// datamodel
namespace fcc {
class CaloHitCollection;
}

/** @class RewriteHCalBarrelBitfield Detector/DetComponents/src/RewriteHCalBarrelBitfield.h RewriteHCalBarrelBitfield.h
 *
 *  @author Anna Zaborowska, Coralie Neubueser
 */

class RewriteHCalBarrelBitfield : public GaudiAlgorithm {
public:
  explicit RewriteHCalBarrelBitfield(const std::string&, ISvcLocator*);
  virtual ~RewriteHCalBarrelBitfield();
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
  /// Handle for the EDM hits to be read
  DataHandle<fcc::CaloHitCollection> m_inHits{"hits/caloInHits", Gaudi::DataHandle::Reader, this};
  /// Handle for the EDM hits to be written
  DataHandle<fcc::CaloHitCollection> m_outHits{"hits/caloOutHits", Gaudi::DataHandle::Writer, this};
  /// Name of the detector readout used in simulation
  Gaudi::Property<std::string> m_oldReadoutName{this, "oldReadoutName", "",
                                                "Name of the detector readout used in simulation"};
  /// Name of the new detector readout
  Gaudi::Property<std::string> m_newReadoutName{this, "newReadoutName", "", "Name of the new detector readout"};
  /// Old bitfield decoder
  dd4hep::DDSegmentation::BitField64* m_oldDecoder;
  /// New bitfield decoder
  dd4hep::DDSegmentation::BitField64* m_newDecoder;
  /// Segmentation fields that are going to be removed from the readout
  dd4hep::DDSegmentation::FCCSWGridPhiEta* m_segmentation;
  Gaudi::Property<std::vector<std::string>> m_oldIdentifiers{
      this, "removeIds", {}, "Segmentation fields that are going to be removed"};
  /// Detector fields that are going to be rewritten ( = old field - to be removed)
  std::vector<std::string> m_detectorIdentifiers;
  /// Limit of debug printing
  Gaudi::Property<uint> m_debugPrint{this, "debugPrint", 10, "Limit of debug printing"};
};
#endif /* DETCOMPONENTS_REWRITEHCALBARRELBITFIELD_H */
