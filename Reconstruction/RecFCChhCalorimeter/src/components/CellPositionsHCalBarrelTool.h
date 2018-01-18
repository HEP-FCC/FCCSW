#ifndef RECCALORIMETER_CELLPOSITIONSHCALBARRELTOOL_H
#define RECCALORIMETER_CELLPOSITIONSHCALBARRELTOOL_H

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

/** @class CellPositionsHCalBarrelTool
 *
 */

class CellPositionsHCalBarrelTool : public GaudiTool, virtual public ICellPositionsTool {
public:
  CellPositionsHCalBarrelTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~CellPositionsHCalBarrelTool() = default;

  virtual StatusCode initialize() final;

  virtual StatusCode finalize() final;

  virtual void getPositions(const fcc::CaloHitCollection& aCells, fcc::PositionedCaloHitCollection& outputColl) final;

  virtual DD4hep::Geometry::Position getXYZPosition(const fcc::CaloHit& aCell) const final;

private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Name of the electromagnetic calorimeter readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "name of the readout"};
  DD4hep::DDSegmentation::GridPhiEta* m_segmentation;
  DD4hep::DDSegmentation::BitField64* m_decoder;
};
#endif /* RECCALORIMETER_CELLPOSITIONSHCALBARRELTOOL_H */
