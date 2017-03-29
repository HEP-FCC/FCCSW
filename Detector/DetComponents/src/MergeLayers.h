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
 *  E.g. '\b merge = [2,3,2]' means that first 2 volumes are merged into new cell (id=0), next 3 volumes become new cell
 * (id=1),
 *  and finally last 2 layers are merged into last cell (id=2).
 *  The sum of all sizes from the list should correspond to the total number of volumes named as indicated in '\b
 * volumeName'.
 *  For an example see Detector/DetComponents/tests/options/mergeLayers.py
 *
 *  @author Anna Zaborowska
 */

class MergeLayers : public GaudiAlgorithm {
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
  DataHandle<fcc::CaloHitCollection> m_inHits{"hits/caloInHits", Gaudi::DataHandle::Reader, this};
  /// Handle for the EDM Hits to be written
  DataHandle<fcc::CaloHitCollection> m_outHits{"hits/caloOutHits", Gaudi::DataHandle::Writer, this};
  // Handle to the detector ID descriptor
  DD4hep::Geometry::IDDescriptor m_descriptor;
  /// Name of the detector readout
  Gaudi::Property<std::string> m_readoutName{this, "readout", "", "Name of the detector readout"};
  /// Identifier to be merged
  Gaudi::Property<std::string> m_idToMerge{this, "identifier", "", "Identifier to be merged"};
  /// Name (or its part) of the volume
  Gaudi::Property<std::string> m_volumeName{this, "volumeName", "", "Name (or its part) of the volume"};
  /// List with number of adjacent cells to be merged
  Gaudi::Property<std::vector<uint>> m_listToMerge{
      this, "merge", {}, "List with number of adjacent cells to be merged"};
  /// Maximum number of lines in debug output
  Gaudi::Property<uint> m_debugPrint{this, "debugPrint", 10, "Maximum number of lines in debug output"};
};
#endif /* DETCOMPONENTS_MERGECELLS_H */
