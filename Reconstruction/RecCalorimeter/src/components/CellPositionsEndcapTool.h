#ifndef RECCALORIMETER_CELLPOSITIONSEndcapTOOL_H
#define RECCALORIMETER_CELLPOSITIONSEndcapTOOL_H

// GAUDI
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ServiceHandle.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"
#include "DetSegmentation/GridPhiEta.h"
#include "FWCore/DataHandle.h"
#include "RecInterface/ICellPositionsTool.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DD4hep/Readout.h"
#include "DD4hep/Volumes.h"
#include "DDSegmentation/Segmentation.h"
#include "TGeoManager.h"

class IGeoSvc;
namespace DD4hep {
namespace DDSegmentation {
class Segmentation;
}
}

/** @class CellPositionsEndcapTool
 *
 */

class CellPositionsEndcapTool : public GaudiTool, virtual public ICellPositionsTool {

public:
  CellPositionsEndcapTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~CellPositionsEndcapTool() = default;
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  virtual void getPositions(const fcc::CaloHitCollection& aCells, fcc::PositionedCaloHitCollection& outputColl) final;

private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Name of the electromagnetic calorimeter readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "name of the readout"};
  /// Array of merged layers in EC
  Gaudi::Property<std::vector<int>> m_mergedLayers{this, "mergedLayers", {}, "array of merged layers"};
  DD4hep::DDSegmentation::GridPhiEta* m_segmentation;
  DD4hep::DDSegmentation::BitField64* m_decoder;
};
#endif /* RECCALORIMETER_CELLPOSITIONSENDCAPTOOL_H */
