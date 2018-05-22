#ifndef RECCALORIMETER_CELLPOSITIONSHCALBARRELNOSEGTOOL_H
#define RECCALORIMETER_CELLPOSITIONSHCALBARRELNOSEGTOOL_H

// GAUDI
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ServiceHandle.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"
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

/** @class CellPositionsHCalBarrelNoSegTool Reconstruction/RecFCChhCalorimeter/src/components/CellPositionsHCalBarrelNoSegTool.h
 *  CellPositionsHCalBarrelNoSegTool.h
 *
 *  Tool to determine each Calorimeter cell position.
 *
 *  For the FCChh Barrel and extended Barrel HCAL, determined from the placed volumes.   
 * 
 *  @author Coralie Neubueser
 */

class CellPositionsHCalBarrelNoSegTool : public GaudiTool, virtual public ICellPositionsTool {
public:
  CellPositionsHCalBarrelNoSegTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~CellPositionsHCalBarrelNoSegTool() = default;

  virtual StatusCode initialize() final;

  virtual StatusCode finalize() final;

  virtual void getPositions(const fcc::CaloHitCollection& aCells, fcc::PositionedCaloHitCollection& outputColl) final;

  virtual dd4hep::Position xyzPosition(const uint64_t& aCellId) const final;

  virtual int layerId(const uint64_t& aCellId) final;

private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Name of the electromagnetic calorimeter readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "HCalBarrelReadout"};
  /// Cellid decoder
  dd4hep::DDSegmentation::BitFieldCoder* m_decoder;
  /// Volume manager
  dd4hep::VolumeManager m_volman;
};
#endif /* RECCALORIMETER_CELLPOSITIONSHCALBARRELNOSEGTOOL_H */
