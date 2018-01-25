#ifndef RECCALORIMETER_CELLPOSITIONSECALBARRELTOOL_H
#define RECCALORIMETER_CELLPOSITIONSECALBARRELTOOL_H

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

/** @class CellPositionsECalBarrelTool
 *
 */

class CellPositionsECalBarrelTool : public GaudiTool, virtual public ICellPositionsTool {
public:
  CellPositionsECalBarrelTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~CellPositionsECalBarrelTool() = default;

  virtual StatusCode initialize() final;

  virtual StatusCode finalize() final;

  virtual void getPositions(const fcc::CaloHitCollection& aCells, fcc::PositionedCaloHitCollection& outputColl) final;

  virtual DD4hep::Geometry::Position getXYZPosition(const fcc::CaloHit& aCell) const final;
  
private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Name of the electromagnetic calorimeter readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "name of the readout"};
  /// Array of layer Radii for inclined EcalB
  Gaudi::Property<std::vector<double>> m_layerRadius{this, "layerRadii", {}, "radii of ECalB layers"};
  DD4hep::DDSegmentation::GridPhiEta* m_segmentation;
  DD4hep::DDSegmentation::BitField64* m_decoder;
};
#endif /* RECCALORIMETER_CELLPOSITIONSECALBARRELTOOL_H */
