#ifndef RECCALORIMETER_CREATECALOCELLS_H
#define RECCALORIMETER_CREATECALOCELLS_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "FWCore/DataHandle.h"

#include "RecInterface/IPrepareEmptyCaloCellsTool.h"
#include "RecInterface/IMergeHitsToCaloCellsTool.h"
#include "RecInterface/ICalibrateCaloCellsTool.h"
#include "RecInterface/INoiseCaloCellsTool.h"

/** @class CreateCaloCells
 *
 *  Algorithm for creating calorimeter cells from Geant4 hits. 
 *  Calls these tools:
 *    - PrepareEmptyCaloCellsTool (prepares a vector of all calorimeter cells with their cellID)
 *    - MergeHitsToCaloCellsTool (merges Geant4 hits into calorimeter cells with same cellID)
 *    - CalibrateCaloCellsTool (calibrate Geant4 energy to EM scale, switch available for this tool)
 *    - NoiseCaloCellsTool (adds noise to calorimeter cells)
 *
 *  @author Jana Faltova
 *  @date   2016-09
 *
 */

class CreateCaloCells : public GaudiAlgorithm 
{
public:
  CreateCaloCells(const std::string& name, ISvcLocator* svcLoc);
 
  ~CreateCaloCells();

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  /// Handle for preparing empty calo cells tool
  ToolHandle<IPrepareEmptyCaloCellsTool> m_prepareCellsTool;
  /// Handle for merging Geant4 hits to cells tool
  ToolHandle<IMergeHitsToCaloCellsTool> m_mergeTool;
  /// Handle for calibration Geant4 energy to EM scale tool
  ToolHandle<ICalibrateCaloCellsTool> m_calibTool;
  /// Handle for the calorimeter cells noise tool
  ToolHandle<INoiseCaloCellsTool> m_noiseTool;

  /// Calibrate to EM scale?
  bool m_doCellCalibration;
  /// Add noise to cells?
  bool m_addCellNoise;
  /// Handle for calo hits (input collection)
  DataHandle<fcc::CaloHitCollection> m_hits;
  /// Handle for calo cells (output collection)
  DataHandle<fcc::CaloHitCollection> m_cells;
};

#endif /* RECCALORIMETER_CREATECALOCELLS_H */
