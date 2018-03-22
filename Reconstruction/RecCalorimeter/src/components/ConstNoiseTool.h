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
  double getNoiseConstantPerCell(uint64_t aCellID);
  double getNoiseOffsetPerCell(uint64_t aCellID);

private:
  std::map<uint,double> m_systemNoiseConstMap;
  std::map<uint,double> m_systemNoiseOffsetMap;

  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  // Decoder
  dd4hep::DDSegmentation::BitField64* m_decoder = new dd4hep::DDSegmentation::BitField64("system:4");

};

#endif /* RECCALORIMETER_CONSTNOISETOOL_H */
