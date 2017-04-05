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
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "ECalHitsPhiEta", "Name of the detector readout"};
  /// Name of active volumes (if different than all)
  Gaudi::Property<std::vector<std::string>> m_activeVolumeName{
      this, "activeVolumeName", {"LAr_sensitive"}, "Name of active volumes (if different than all)"};
  /// Name of active layers for sampling calorimeter
  Gaudi::Property<std::vector<std::string>> m_activeFieldName{
      this, "activeFieldName", {"active_layer"}, "Name of active layers for sampling calorimeter"};
  /// Name of the fields describing the segmented volume
  Gaudi::Property<std::vector<std::string>> m_fieldNames{
      this, "fieldNames", {}, "Name of the fields describing the segmented volume"};
  /// Values of the fields describing the segmented volume
  Gaudi::Property<std::vector<int>> m_fieldValues{
      this, "fieldValues", {}, "Values of the fields describing the segmented volume"};
};

#endif /* RECCALORIMETER_NESTEDVOLUMESCALOTOOL_H */
