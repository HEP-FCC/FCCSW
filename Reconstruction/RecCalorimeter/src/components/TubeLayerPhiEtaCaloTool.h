#ifndef RECCALORIMETER_TUBELAYERPHIETACALOTOOL_H
#define RECCALORIMETER_TUBELAYERPHIETACALOTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "RecInterface/ICalorimeterTool.h"

class IGeoSvc;

/** @class TubeLayerPhiEtaCaloTool Reconstruction/RecCalorimeter/src/components/TubeLayerPhiEtaCaloTool.h
 *TubeLayerPhiEtaCaloTool.h
 *
 *  Tool for geometry-dependent settings of the digitisation.
 *  It assumes cylindrical geometry (layers) and phi-eta segmentation.
 *
 *  @author Anna Zaborowska
 */

class TubeLayerPhiEtaCaloTool : public GaudiTool, virtual public ICalorimeterTool {
public:
  TubeLayerPhiEtaCaloTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~TubeLayerPhiEtaCaloTool() = default;
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;
  /** Prepare a map of all existing cells in current geometry.
   *   Active layers (cylindrical tubes) are looked in the geometry manager by name ('\b activeVolumeName').
   *   Corresponding bitfield name is given in '\b activeFieldName'.
   *   If users wants to limit the number of active layers, it is possible by setting '\b activeVolumesNumber'.
   *   The total number of cells N = n_layer * n_eta * n_phi, where
   *   n_layer is number of layers (taken from geometry if activeVolumesNumber not set),
   *   n_eta is number of eta bins in that layer,
   *   n_phi is number of phi bins (the same for each layer).
   *   For more explanation please [see reconstruction documentation](@ref md_reconstruction_doc_reccalorimeter).
   *   @param[out] aCells map of existing cells (and deposited energy, set to 0)
   *   return Status code.
   */
  virtual StatusCode prepareEmptyCells(std::unordered_map<uint64_t, double>& aCells) final;

private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
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
  /// Temporary: for use with MergeLayer tool
  unsigned int m_activeVolumesNumber;
};

#endif /* RECCALORIMETER_TUBELAYERPHIETACALOTOOL_H */
