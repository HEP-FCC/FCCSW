#ifndef RECCALORIMETER_CONSTNOISETOOL_H
#define RECCALORIMETER_CONSTNOISETOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
class IRndmGenSvc;

// FCCSW
#include "DetSegmentation/FCCSWGridPhiEta.h"
#include "RecInterface/INoiseConstTool.h"
class IGeoSvc;

#include <map>

/** @class ConstNoiseTool
 *
 *  Tool to set noise constant for all cells in a calorimeters
 *
 *  @author Coralie Neubueser
 *  @date   2018-01
 *
 */

class ConstNoiseTool : public GaudiTool, virtual public INoiseConstTool {
public:
  ConstNoiseTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~ConstNoiseTool() = default;

  virtual StatusCode initialize() final;

  virtual StatusCode finalize() final;

  /// Find the appropriate noise constant from the histogram
  double getNoiseConstantPerCell(int64_t aCellID);

private:
  /// Name of the detector readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "ECalHitsPhiEta", "Name of the detector readout"};
  /// Name of active layers for sampling calorimeter
  Gaudi::Property<std::string> m_systemFieldName{this, "systemFieldName", "system",
      "Name of field describing detector system"};
  std::map<uint,double> m_systemNoiseConstMap;
  
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// PhiEta segmentation
  dd4hep::DDSegmentation::FCCSWGridPhiEta* m_segmentation;
  // Decoder
  dd4hep::DDSegmentation::BitField64* m_decoder;

};

#endif /* RECCALORIMETER_CONSTNOISETOOL_H */
