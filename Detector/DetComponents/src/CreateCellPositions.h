#ifndef DETCOMPONENTS_CREATECELLPOSITIONS_H
#define DETCOMPONENTS_CREATECELLPOSITIONS_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;
namespace DD4hep {
namespace DDSegmentation {
    class Segmentation;
}
}

/** @class CreateCellPositions DetComponents/src/CreateCellPositions.h CreateCellPositions.h
 *
 *  Retrieve positions of the cells from cell ID.
 *  This algorithm saves the centre position of the volume. The segmentation of volume is taken into account.
 *  Transformation matrix from global coordinates to local is taken from DD4hep::Geometry::DetElement.
 *  Full hierarchy of DetElements (for each sensitive volume) is required.
 *
 *  @author Anna Zaborowska, Coralie Neubueser
 *
 */

template <class Hits, class PositionedHit>
class CreateCellPositions : public GaudiAlgorithm {
public:
  CreateCellPositions(const std::string& name, ISvcLocator* svcLoc);
  /**  Initialize.
   *   @return status code
   */
  StatusCode initialize();
  /**  Execute.
   *   @return status code
   */
  StatusCode execute();
  /**  Finalize.
   *   @return status code
   */
  StatusCode finalize();

private:
  /// Pointer to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;
  /// Name of the electromagnetic calorimeter readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "name of the readout"};
   /// Handle for hits (input collection with cellID)
  DataHandle<Hits> m_hits{"hits/hits", Gaudi::DataHandle::Reader, this};
  /// Handle for positioned hits (output collection)
  DataHandle<PositionedHit> m_positionedHits{"hits/positionedHits", Gaudi::DataHandle::Writer, this};
};

#endif /* DETCOMPONENTS_CREATECELLPOSITIONS_H */
