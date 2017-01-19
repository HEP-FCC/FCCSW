#ifndef RECCALORIMETER_NESTEDVOLUMESCALOTOOL_H
#define RECCALORIMETER_NESTEDVOLUMESCALOTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "RecInterface/ICalorimeterTool.h"

class IGeoSvc;

/** @class NestedVolumesCaloTool Reconstruction/RecCalorimeter/src/components/NestedVolumesCaloTool.h
 *NestedVolumesCaloTool.h
 *
 *  Tool for geometry-dependent settings of the digitisation.
 *  It assumes no segmentation is used. It may be used for nested volumes.
 *   For more explanation please [see reconstruction documentation](@ref md_reconstruction_doc_reccalorimeter).
 *
 *  @author Anna Zaborowska
 */

class NestedVolumesCaloTool : public GaudiTool, virtual public ICalorimeterTool {
public:
  NestedVolumesCaloTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~NestedVolumesCaloTool() = default;
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;
  /** Prepare a map of all existing cells in current geometry.
   *   Active volumes are looked in the geometry manager by name ('\b activeVolumeName').
   *   Corresponding bitfield name is given in '\b activeFieldName'.
   *   If more than one name is given, it is assumed that volumes are nested.
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
  /// Name of active volumes (if different than all)
  std::vector<std::string> m_activeVolumeName;
  /// Name of active layers for sampling calorimeter
  std::vector<std::string> m_activeFieldName;
  /// Name of the fields describing the segmented volume
  std::vector<std::string> m_fieldNames;
  /// Values of the fields describing the segmented volume
  std::vector<int> m_fieldValues;
};

#endif /* RECCALORIMETER_NESTEDVOLUMESCALOTOOL_H */
