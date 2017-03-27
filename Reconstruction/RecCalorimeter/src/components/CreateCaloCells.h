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

class CreateCaloCells : public GaudiAlgorithm
{

public:
  CreateCaloCells(const std::string& name, ISvcLocator* svcLoc);

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  /// Handle for tool to calibrate Geant4 energy to EM scale tool
  ToolHandle<ICalibrateCaloHitsTool> m_calibTool{"CalibrateCaloHitsTool", this};
  /// Handle for the calorimeter cells noise tool
  ToolHandle<INoiseCaloCellsTool> m_noiseTool{"NoiseCaloCellsFlatTool", this};
  /// Handle for the geometry tool
  ToolHandle<ICalorimeterTool> m_geoTool{"TubeLayerPhiEtaCaloTool", this};

  /// Calibrate to EM scale?
  Gaudi::Property<bool> m_doCellCalibration{this, "doCellCalibration", true, "Calibrate to EM scale?"};
  /// Add noise to cells?
  Gaudi::Property<bool> m_addCellNoise{this, "addCellNoise", true, "Add noise to cells?"};
  /// Save only cells with energy above threshold?
  Gaudi::Property<bool> m_filterCellNoise{this, "filterCellNoise", false, "Save only cells with energy above threshold?"};
  /// Handle for calo hits (input collection)
  DataHandle<fcc::CaloHitCollection> m_hits{"hits", Gaudi::DataHandle::Reader, this};
  /// Handle for calo cells (output collection)
  DataHandle<fcc::CaloHitCollection> m_cells{"cells", Gaudi::DataHandle::Writer, this};
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
