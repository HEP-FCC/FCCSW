#ifndef RECCALORIMETER_CELLPOSITIONSHCALBTOOL_H
#define RECCALORIMETER_CELLPOSITIONSHCALBTOOL_H

// GAUDI
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ServiceHandle.h"

// FCCSW
#include "RecInterface/ICellPositionsTool.h"
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"
#include "FWCore/DataHandle.h"
#include "DetSegmentation/GridPhiEta.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DD4hep/Volumes.h"
#include "TGeoManager.h"
#include "DD4hep/Readout.h"
#include "DDSegmentation/Segmentation.h"

class IGeoSvc;
namespace DD4hep {
namespace DDSegmentation {
    class Segmentation;
}
}

/** @class CellPositionsHCalBTool
 *
 */

class CellPositionsHCalBTool : public GaudiTool, virtual public ICellPositionsTool {
public:
  CellPositionsHCalBTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~CellPositionsHCalBTool() = default;
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
#endif /* RECCALORIMETER_CELLPOSITIONSHCALBTOOL_H */
