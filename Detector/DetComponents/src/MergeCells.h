#ifndef DETCOMPONENTS_MERGECELLS_H
#define DETCOMPONENTS_MERGECELLS_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;

#include "DD4hep/IDDescriptor.h"

// datamodel
namespace fcc {
class CaloHitCollection;
}

/** @class MergeCells Detector/DetComponents/src/MergeCells.h MergeCells.h
 *
 *  Merge cells for one field of the segmentation.
 *  Merging is performed on a collection of hits and as a result the cellID is changed.
 *  GeoSvc is required (to access the detector readout).
 *  Name of the field to be merged is defined be property '\b identifier'.
 *  Property '\b merge' describes how many adjacent cells should be merged.
 *  If the identifier describes an unsigned field, the number of cells to be merged can be any number.
 *  If the identifier describes a signed field, however, the number of cells to be merged need to be an odd number (to
 * keep the centre of the central bin in 0).
 *  For an example see Detector/DetComponents/tests/options/mergeCells.py
 *
 *  @author Anna Zaborowska
 */

class MergeCells : public GaudiAlgorithm {
public:
  explicit MergeCells(const std::string&, ISvcLocator*);
  virtual ~MergeCells();
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
  /// Handle for the EDM Hits to be written
  DataHandle<fcc::CaloHitCollection> m_outHits{"hits/caloOutHits", Gaudi::DataHandle::Writer, this};
  // Handle to the detector ID descriptor
  DD4hep::Geometry::IDDescriptor m_descriptor;
  /// Name of the detector readout
  Gaudi::Property<std::string> m_readoutName{this, "readout", "", "Name of the detector readout"};
  /// Identifier to be merged
  Gaudi::Property<std::string> m_idToMerge{this, "identifier", "", "Identifier to be merged"};
  /// Number of adjacent cells to be merged
  Gaudi::Property<uint> m_numToMerge{this, "merge", 0, "Number of adjacent cells to be merged"};
  /// Limit of debug printing
  Gaudi::Property<uint> m_debugPrint{this, "debugPrint", 10, "Limit of debug printing"};
};
#endif /* DETCOMPONENTS_MERGECELLS_H */
