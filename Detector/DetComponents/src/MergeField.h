#ifndef RECONSTRUCTION_MERGEIDENTIFIER_H
#define RECONSTRUCTION_MERGEIDENTIFIER_H

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

/** @class MergeField Reconstruction/src/MergeField.h MergeField.h
 *
 *  Merge cells in one dimension.
 *  GeoSvc is required (to access the detector readout).
 *  Name of the dimension to be merged is defined be property '\b identifier'.
 *  Property '\b numberOfCells' desccribes how many adjacent cells should be merged.
 *  If the identifier is described by an unsigned field, the number of cells to be merged can be any number.
 *  If the identifier is described by a signed field, however, the number of cells to be merged need to be an odd number (to keep the centre of the central bin in 0).
 *  [For more information please see](@ref md_rec_doc_commonUtils).
 *
 *  @author Anna Zaborowska
 */

class MergeField: public GaudiAlgorithm {
public:
  explicit MergeField(const std::string&, ISvcLocator*);
  virtual ~MergeField();
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
  DataHandle<fcc::CaloHitCollection> m_inHits;
  /// Handle for the EDM Hits to be written
  DataHandle<fcc::CaloHitCollection> m_outHits;
  // Handle to the detector ID descriptor
  DD4hep::Geometry::IDDescriptor m_descriptor;
  /// Name of the detector readout
  std::string m_readoutName;
  /// Identifier to be merged
  std::string m_idToMerge;
  /// Number of adjacent cells to be merged
  uint m_numToMerge;
};
#endif /* RECONSTRUCTION_MERGEIDENTIFIER_H */
