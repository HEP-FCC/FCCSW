#ifndef RECCALORIMETER_TUBELAYERPHIETACALOTOOL_H
#define RECCALORIMETER_TUBELAYERPHIETACALOTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "RecInterface/ICalorimeterTool.h"

class IGeoSvc;

class TubeLayerPhiEtaCaloTool : public GaudiTool, virtual public ICalorimeterTool
{
public:
  TubeLayerPhiEtaCaloTool(const std::string& type,const std::string& name, const IInterface* parent);
  virtual ~TubeLayerPhiEtaCaloTool();
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;
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
