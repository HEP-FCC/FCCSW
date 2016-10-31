#ifndef DETCOMPONENTS_MERGEVOLUMECELLS_H
#define DETCOMPONENTS_MERGEVOLUMECELLS_H

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

/** @class MergeVolumeCells Detector/DetComponents/src/MergeVolumeCells.h MergeVolumeCells.h
 *
 *  Merge cells represented by some volume (e.g. merge cylindrical layers).
 *  GeoSvc is required (to access the detector readout).
 *  Name of the field to be merged is defined be property '\b identifier'.
 *  Name of the volume that corresponds to the merged volumes is defined be property '\b volumeName'.
 *  Property '\b merge' describes how many adjacent cells should be merged, as a list of new cell sizes.
 *  E.g. '\b merge = [2,3,2]' means that first 2 cells are merged into new cell (id=0), next 3 cells become new cell (id=1),
 *  and finally last 2 layers are erged into new cell (id=2).
 *  The sum of all sizes from the list should correspond to the total number of volumes named as indicated in '\b volumeName'.
 *  For an example see Detector/DetComponents/tests/options/mergeCells.py
 *
 *  @author Anna Zaborowska
 */

class MergeVolumeCells: public GaudiAlgorithm {
public:
  explicit MergeVolumeCells(const std::string&, ISvcLocator*);
  virtual ~MergeVolumeCells();
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
  /// Name (or its part) of the volume
  std::string m_volumeName;
  /// List with number of adjacent cells to be merged
  std::vector<uint> m_listToMerge;
};
#endif /* DETCOMPONENTS_MERGECELLS_H */
