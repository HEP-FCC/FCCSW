#ifndef RECCALORIMETER_TUBELAYERPHIETACALOTOOL_H
#define RECCALORIMETER_TUBELAYERPHIETACALOTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "RecInterface/ICalorimeterTool.h"

class IGeoSvc;

/** @class LayerPhiEtaCaloTool Reconstruction/RecCalorimeter/src/components/LayerPhiEtaCaloTool.h
 *LayerPhiEtaCaloTool.h
 *
 *  Tool for geometry-dependent settings of the digitisation.
 *  Needs the eta range per layer as input to calculate the total number of calo cells, layers and phi-eta segmentation is taken from the geometry.
 *
 *  @author Anna Zaborowska, Coralie Neubueser
 */

class LayerPhiEtaCaloTool : public GaudiTool, virtual public ICalorimeterTool {
public:
  LayerPhiEtaCaloTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~LayerPhiEtaCaloTool() = default;
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
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "BarHCal_Readout_phieta"};
  /// Name of active volumes
  Gaudi::Property<std::string> m_activeVolumeName{this, "activeVolumeName", "layerVolume"};
  /// Name of active layers for sampling calorimeter
  Gaudi::Property<std::string> m_activeFieldName{this, "activeFieldName", "layer"};
  /// Name of the fields describing the segmented volume
  Gaudi::Property<std::vector<std::string>> m_fieldNames{this, "fieldNames", {"system"}};
  /// Values of the fields describing the segmented volume
  Gaudi::Property<std::vector<int>> m_fieldValues{this, "fieldValues", 8};
  /// Temporary: for use with MergeLayer tool
  Gaudi::Property<unsigned int> m_activeVolumesNumber{this, "activeVolumesNumber", 10};
  /// Temporary: for use with Tile Calo
  Gaudi::Property<std::vector<double>> m_activeVolumesEta{this, "activeVolumesEta", {1.2524, 1.2234, 1.1956, 1.15609, 1.1189, 1.08397, 1.0509, 0.9999, 0.9534, 0.91072}};
};

#endif /* RECCALORIMETER_TUBELAYERPHIETACALOTOOL_H */
