#ifndef RECCALORIMETER_CREATECALOCELLS_H
#define RECCALORIMETER_CREATECALOCELLS_H

// FCCSW
#include "RecInterface/ICalibrateCaloHitsTool.h"
#include "RecInterface/INoiseCaloCellsTool.h"
#include "RecInterface/ICalorimeterTool.h"
#include "FWCore/DataHandle.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloHit.h"

class IGeoSvc;

/** @class CreateCaloCells
 *
 *  Algorithm for creating calorimeter cells from Geant4 hits.
 *  Tube geometry with PhiEta segmentation expected.
 *
 *  Flow of the program:
 *  1/ Merge Geant4 energy deposits with same cellID
 *  2/ Calibrate to electromagnetic scale (if calibration switched on)
 *  3/ Add random noise to each cell (if noise switched on)
 *  4/ Filter cells and remove those with energy below threshold (if noise + filtering switched on)
 *
 *  Tools called:
 *    - CalibrateCaloHitsTool
 *    - NoiseCaloCellsTool
 *
 *  @author Jana Faltova
 *  @author Anna Zaborowska
 *  @date   2016-09
 *
 */

class CreateCaloCells : public GaudiAlgorithm {
public:
  CreateCaloCells(const std::string& name, ISvcLocator* svcLoc);

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  /// Handle for calibration Geant4 energy to EM scale tool
  ToolHandle<ICalibrateCaloHitsTool> m_calibTool;
  /// Handle for the calorimeter cells noise tool
  ToolHandle<INoiseCaloCellsTool> m_noiseTool;
  /// Handle for the geometry tool
  ToolHandle<ICalorimeterTool> m_geoTool;

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
  /// Name of active volumes
  std::string m_activeVolumeName;
  /// Name of active layers for sampling calorimeter
  std::string m_activeFieldName;
  /// Name of the fields describing the segmented volume
  std::vector<std::string> m_fieldNames;
  /// Values of the fields describing the segmented volume
  std::vector<int> m_fieldValues;
  /** Temporary: for use with MergeLayer tool
   * MergeLayer is going to be replaced by RedoSegmentation once we can define segmentation with variable cell (layer)
   * size.
   * This property won't be needed anymore.
   */
  unsigned int m_activeVolumesNumber;
  /// Use only volume ID? If false, using PhiEtaSegmentation
  bool m_useVolumeIdOnly;
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Map of cell IDs (corresponding to DD4hep IDs) and energy
  std::unordered_map<uint64_t, double> m_cellsMap;
};

#endif /* RECCALORIMETER_CREATECALOCELLS_H */
