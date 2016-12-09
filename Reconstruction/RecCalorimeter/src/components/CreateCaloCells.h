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
  Gaudi::Property<bool> m_doCellCalibration{this, "doCellCalibration", true, "Calibrate to EM scale?"};
  /// Add noise to cells?
  Gaudi::Property<bool> m_addCellNoise{this, "addCellNoise", true, "Add noise to cells?"};
  /// Save only cells with energy above threshold?
  Gaudi::Property<bool> m_filterCellNoise{this, "filterCellNoise", false, "Save only cells with energy above threshold?"};
  /// Handle for calo hits (input collection)
  DataHandle<fcc::CaloHitCollection> m_hits;
  /// Handle for calo cells (output collection)
  DataHandle<fcc::CaloHitCollection> m_cells;
  /// Name of the detector readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "ECalHitsPhiEta", "Name of the detector readout"};
  /// Name of active volumes (material name)
  Gaudi::Property<std::string> m_activeVolumeName{this, "activeVolumeName", "_sensitive", "Name of the active volumes (material name)"};
  /// Name of active layers for sampling calorimeter
  Gaudi::Property<std::string> m_activeFieldName{this, "activeFieldName", "active_layer", "Name of active layers for sampling calorimeter"};
  /// Name of the bit-fields (in the readout) that contain the volume to segment
  Gaudi::Property<std::vector<std::string>> m_fieldNames{this, "fieldNames", {},
                                         "Name of the bit-fields (in the readout) that contains the volume to segment"};
  /// Values of the fields that identify the volume to be segmented (e.g. ID of the ECal)
  Gaudi::Property<std::vector<int>> m_fieldValues{this, "fieldValues", {},
                                 "Value of the field that identifies the volume to be segmented (e.g. ID of the ECal)"};

  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Vector of noise hits
  std::vector<fcc::CaloHit*> m_edmHitsNoiseVector;

};

#endif /* RECCALORIMETER_CREATECALOCELLS_H */
