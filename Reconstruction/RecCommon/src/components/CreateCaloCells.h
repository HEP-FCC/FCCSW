#ifndef RECONSTRUCTION_CREATECALOCELLS_H
#define RECONSTRUCTION_CREATECALOCELLS_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "FWCore/DataHandle.h"

#include "RecInterface/IPrepareEmptyCaloCellsTool.h"
#include "RecInterface/IMergeHitsToCaloCellsTool.h"
#include "RecInterface/ICalibrateCaloCellsTool.h"
#include "RecInterface/INoiseCaloCellsTool.h"

// our EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloHit.h"


class CreateCaloCells : public GaudiAlgorithm 
{
public:
  CreateCaloCells(const std::string& name, ISvcLocator* svcLoc);
 
  ~CreateCaloCells();

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  //Tool for preparing empty calo cells
  ToolHandle<IPrepareEmptyCaloCellsTool> m_prepareTool;
  //Tool for merging Geant4 hits to cells
  ToolHandle<IMergeHitsToCaloCellsTool> m_mergeTool;
  //Handle for calibration Geant4 energy to EM scale tool
  ToolHandle<ICalibrateCaloCellsTool> m_calibTool;
  /// Handle for the calorimeter cells noise tool
  ToolHandle<INoiseCaloCellsTool> m_noiseTool;

  bool m_doCellCalibration;
  bool m_addCellNoise;
  /// Handle for calo hits (input collection)
  DataHandle<fcc::CaloHitCollection> m_caloHits;
  /// Handle for calo cells (output collection)
  DataHandle<fcc::CaloHitCollection> m_caloCells;
};

#endif /* RECONSTRUCTION_CREATECALOCELLS_H */
