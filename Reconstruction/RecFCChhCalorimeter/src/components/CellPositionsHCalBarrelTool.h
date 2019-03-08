#ifndef RECCALORIMETER_CELLPOSITIONSHCALBARRELTOOL_H
#define RECCALORIMETER_CELLPOSITIONSHCALBARRELTOOL_H

// GAUDI
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ServiceHandle.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"
#include "DetSegmentation/FCCSWGridPhiEta.h"
#include "FWCore/DataHandle.h"
#include "RecInterface/ICellPositionsTool.h"

// DD4hep
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

  virtual dd4hep::Position xyzPosition(const uint64_t& aCellId) const final;

  virtual int layerId(const uint64_t& aCellId) final;

private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Name of the electromagnetic calorimeter readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "BarHCal_Readout_phieta"};
  Gaudi::Property<std::vector<double>> m_radii{this, "radii", {291.05, 301.05, 313.55, 328.55, 343.55, 358.55, 378.55, 413.55, 428.55, 453.55}};

  dd4hep::DDSegmentation::FCCSWGridPhiEta* m_segmentation;
  dd4hep::DDSegmentation::BitFieldCoder* m_decoder;
  dd4hep::VolumeManager m_volman;
};
#endif /* RECCALORIMETER_CELLPOSITIONSHCALBARRELTOOL_H */
