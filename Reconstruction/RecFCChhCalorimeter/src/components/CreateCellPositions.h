#ifndef DETCOMPONENTS_CREATECELLPOSITIONS_H
#define DETCOMPONENTS_CREATECELLPOSITIONS_H

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ICellPositionsTool.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "datamodel/CaloHit.h"
#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHit.h"
#include "datamodel/PositionedCaloHitCollection.h"

class IGeoSvc;

/** @class CreateCellPositions Reconstruction/RecCalorimeter/src/components/CreateCellPositions.h CreateCellPositions.h
 *
 *  Retrieve positions of the cells from cell ID.
 *  This algorithm saves the centre position of the volume. The segmentation of volume is taken into account.
 *  Transformation matrix from global coordinates to local is taken from DD4hep::Geometry::DetElement.
 *  Full hierarchy of DetElements (for each sensitive volume) is required.
 *
 *  @author Anna Zaborowska, Coralie Neubueser
 *
 */

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
  /// Handle for tool to get positions
  ToolHandle<ICellPositionsTool> m_cellPositionsTool;
  /// Input collection
  DataHandle<fcc::CaloHitCollection> m_hits{"hits/hits", Gaudi::DataHandle::Reader, this};
  /// Output collection
  DataHandle<fcc::PositionedCaloHitCollection> m_positionedHits{"hits/positionedHits", Gaudi::DataHandle::Writer, this};
};

#endif /* DETCOMPONENTS_CREATECELLPOSITIONS_H */
