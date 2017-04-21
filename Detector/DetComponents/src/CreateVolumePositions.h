#ifndef DETCOMPONENTS_CREATEVOLUMEPOSITIONS_H
#define DETCOMPONENTS_CREATEVOLUMEPOSITIONS_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;

/** @class CreateVolumePositions
 *
 *  Retrieve positions of the volumes.
 *  This algorithm saves the centre position of the volume. No segmentation of volume is taken into account.
 *  Transformation matrix from global coordinates to local is taken from DD4hep::Geometry::DetElement.
 *  Full hierarchy of DetElements (for each sensitive volume) is required.
 *
 *  @author Anna Zaborowska
 *
 */

template<class H, class P>
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
  SmartIF<IGeoSvc> m_geoSvc;
  /// Handle for hits (input collection with cellID)
  DataHandle<H> m_hits{"hits/hits", Gaudi::DataHandle::Reader, this};
  /// Handle for positioned hits (output collection)
  DataHandle<P> m_positionedHits{"hits/positionedHits", Gaudi::DataHandle::Writer, this};
};

#endif /* DETCOMPONENTS_CREATEVOLUMEPOSITIONS_H */
