#ifndef RECCALORIMETER_CREATECALOCELLS_H
#define RECCALORIMETER_CREATECALOCELLS_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "FWCore/DataHandle.h"

#include "RecInterface/IMergeCaloHitsTool.h"
#include "RecInterface/ICalibrateCaloHitsTool.h"
#include "RecInterface/INoiseCaloCellsTool.h"

class IGeoSvc;

/** @class CreateCaloCells
 *
 *  Algorithm for creating calorimeter cells from Geant4 hits. 
 *  Tube geometry with PhiEta segmentation expected.
 * 
 *  Flow of the program:
 *  1/ Merge Geant4 hits with same cellID
 *  2/ Calibrate to electromagnetic scale (if calibration switched on)
 *  3/ Prepare random noise hits for each cell (if noise switched on)
 *  4/ Merge signal with noise hits (if noise switched on)
 *  5/ Filter cell energy and accept only cells with energy above threshold (if noise + filtering switched on)
 * 
 *  Tools caled:
 *    - MergeCaloHitsTool
 *    - CalibrateCaloHitsTool
 *    - NoiseCaloCellsTool
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

  StatusCode prepareEmptyCells(std::vector<fcc::CaloHit*>& caloCells);

  /// Handle for merging Geant4 hits tool
  ToolHandle<IMergeCaloHitsTool> m_mergeTool;
  /// Handle for calibration Geant4 energy to EM scale tool
  ToolHandle<ICalibrateCaloHitsTool> m_calibTool;
  /// Handle for the calorimeter cells noise tool
  ToolHandle<INoiseCaloCellsTool> m_noiseTool;

  /// Calibrate to EM scale?
  bool m_doCellCalibration;
  /// Add noise to cells?
  bool m_addCellNoise;
  /// Save only cells with energy above threshold?
  bool m_filterCellNoise;
  /// Handle for calo hits (input collection)
  DataHandle<fcc::CaloHitCollection> m_hits;
  /// Handle for calo cells (output collection)
  DataHandle<fcc::CaloHitCollection> m_cells;
  /// Name of the detector readout
  std::string m_readoutName;
  /// Name of active volumes (material name)
  std::string m_activeVolumeName;
  /// Number of volumes with active material which are not readout(e.g. ECAL: LAr bath in cryostat)
  unsigned m_numVolumesRemove;
  /// Name of active layers for sampling calorimeter
  std::string m_activeFieldName;
  /// Name of the fields describing the segmented volume
  std::vector<std::string> m_fieldNames;
  /// Values of the fields describing the segmented volume
  std::vector<int> m_fieldValues;

  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Vector of noise hits
  std::vector<fcc::CaloHit*> m_edmHitsNoiseVector;

};

#endif /* RECCALORIMETER_CREATECALOCELLS_H */
