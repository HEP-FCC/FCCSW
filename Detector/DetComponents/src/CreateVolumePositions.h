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

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  /// Handle for hits (input collection with cellID)
  DataHandle<H> m_hits;
  /// Handle for positioned hits (output collection)
  DataHandle<P> m_positionedHits;
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
};

#endif /* DETCOMPONENTS_CREATEVOLUMEPOSITIONS_H */
