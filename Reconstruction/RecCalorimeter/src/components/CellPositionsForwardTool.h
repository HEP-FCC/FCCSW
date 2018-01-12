#ifndef RECCALORIMETER_CELLPOSITIONSFORWARDTOOL_H
#define RECCALORIMETER_CELLPOSITIONSFORWARDTOOL_H

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

/** @class CellPositionsForwardTool
 *
 */

class CellPositionsForwardTool : public GaudiTool, virtual public ICellPositionsTool {
public:
  CellPositionsForwardTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~CellPositionsForwardTool() = default;
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  virtual void getPositions(const fcc::CaloHitCollection& aCells, fcc::PositionedCaloHitCollection& outputColl) final;

private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Name of the electromagnetic calorimeter readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "name of the readout"};
  DD4hep::DDSegmentation::GridPhiEta* m_segmentation;
  DD4hep::DDSegmentation::BitField64* m_decoder;
};
#endif /* RECCALORIMETER_CELLPOSITIONSFORWARDTOOL_H */
