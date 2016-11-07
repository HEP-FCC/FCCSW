#ifndef DETCOMPONENTS_MERGELAYERS_H
#define DETCOMPONENTS_MERGELAYERS_H

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

/** @class MergeLayers Detector/DetComponents/src/MergeLayers.h MergeLayers.h
 *
 *  Merge layers (volume ids).
 *  Merging is performed on a collection of hits and as a result only the cellID of hit is changed
 *  (in particular, the part of the volume ID). There is no real change of the geometry.
 *  GeoSvc is required (to access the detector readout).
 *  Common part of the name of the volumes to be merged is defined be property '\b volumeName'.
 *  Name of the field in the readout that corresponds to the volume is defined be property '\b identifier'.
 *  Property '\b merge' describes how many adjacent volumes should be merged.
 *  E.g. '\b merge = [2,3,2]' means that first 2 volumes are merged into new cell (id=0), next 3 volumes become new cell (id=1),
 *  and finally last 2 layers are merged into last cell (id=2).
 *  The sum of all sizes from the list should correspond to the total number of volumes named as indicated in '\b volumeName'.
 *  For an example see Detector/DetComponents/tests/options/mergeLayers.py
 *
 *  @author Anna Zaborowska
 */

class MergeLayers: public GaudiAlgorithm {
public:
  explicit MergeLayers(const std::string&, ISvcLocator*);
  virtual ~MergeLayers();
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
  /// Limit of debug printing
  uint m_debugPrint;
};
#endif /* DETCOMPONENTS_MERGECELLS_H */
