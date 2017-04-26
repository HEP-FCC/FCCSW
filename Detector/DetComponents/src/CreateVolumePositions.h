#ifndef DETCOMPONENTS_CREATEVOLUMEPOSITIONS_H
#define DETCOMPONENTS_CREATEVOLUMEPOSITIONS_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;

/** @class CreateVolumePositions DetComponents/src/CreateVolumePositions.h CreateVolumePositions.h
 *
 *  Retrieve positions of the volumes from cell ID.
 *  This algorithm saves the centre position of the volume. No segmentation of volume is taken into account.
 *  Transformation matrix from global coordinates to local is taken from DD4hep::Geometry::DetElement.
 *  Full hierarchy of DetElements (for each sensitive volume) is required.
 *
 *  @author Anna Zaborowska
 *
 */

template <class Hits, class PositionedHit>
class CreateVolumePositions : public GaudiAlgorithm {
public:
  CreateVolumePositions(const std::string& name, ISvcLocator* svcLoc);
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
  /// Handle for hits (input collection with cellID)
  DataHandle<Hits> m_hits{"hits/hits", Gaudi::DataHandle::Reader, this};
  /// Handle for positioned hits (output collection)
  DataHandle<PositionedHit> m_positionedHits{"hits/positionedHits", Gaudi::DataHandle::Writer, this};
};

#endif /* DETCOMPONENTS_CREATEVOLUMEPOSITIONS_H */
