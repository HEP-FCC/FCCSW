#ifndef DETCOMPONENTS_REWRITEBITFIELD_H
#define DETCOMPONENTS_REWRITEBITFIELD_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;

// DD4hep
#include "DD4hep/Readout.h"
namespace dd4hep {
namespace DDSegmentation {
class Segmentation;
}
}

// datamodel
namespace fcc {
class CaloHitCollection;
}

/** @class RewriteBitfield Detector/DetComponents/src/RewriteBitfield.h RewriteBitfield.h
 *
 *  Rewrite the readout bitfield.
 *  This can be used if the order of fields in the bitfield changed, or if some field should be removed.
 *  Removal may be needed for some types of reconstruction.
 *  New readout bitfield has to be added to <readouts> tag in the detector description xml.
 *  Cell IDs are rewritten from the old readout (`\b oldReadoutName`) to the new readout (`\b newReadoutName`).
 *  Names of the fields to be removed (for verification) are passed as a vector '\b removeIds'.
 *
 *  For an example see Detector/DetComponents/tests/options/rewriteBitfield.py
 *
 *  @author Anna Zaborowska
 */

class RewriteBitfield : public GaudiAlgorithm {
public:
  explicit RewriteBitfield(const std::string&, ISvcLocator*);
  virtual ~RewriteBitfield();
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
  dd4hep::DDSegmentation::BitFieldCoder* m_oldDecoder;
  /// New bitfield decoder
  dd4hep::DDSegmentation::BitFieldCoder* m_newDecoder;
  /// Segmentation fields that are going to be removed from the readout
  Gaudi::Property<std::vector<std::string>> m_oldIdentifiers{
      this, "removeIds", {}, "Segmentation fields that are going to be removed"};
  /// Detector fields that are going to be rewritten ( = old field - to be removed)
  std::vector<std::string> m_detectorIdentifiers;
  /// Limit of debug printing
  Gaudi::Property<uint> m_debugPrint{this, "debugPrint", 10, "Limit of debug printing"};
};
#endif /* DETCOMPONENTS_REWRITEBITFIELD_H */
