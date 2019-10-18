#ifndef RECCALORIMETER_CELLPOSITIONSDUMMYTOOL_H
#define RECCALORIMETER_CELLPOSITIONSDUMMYTOOL_H

// GAUDI
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ServiceHandle.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"
#include "FWCore/DataHandle.h"
#include "RecInterface/ICellPositionsTool.h"

class IGeoSvc;

/** @class CellPositionsDummyTool Reconstruction/RecFCChhCalorimeter/src/components/CellPositionsDummyTool.h
 *   CellPositionsDummyTool.h
 *
 *  Tool that returns (0,0,0) for each Calorimeter cell.
 * 
 *  @author Coralie Neubueser
*/

class CellPositionsDummyTool : public GaudiTool, virtual public ICellPositionsTool {

public:
  CellPositionsDummyTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~CellPositionsDummyTool() = default;

  virtual StatusCode initialize() final;

  virtual StatusCode finalize() final;

  virtual void getPositions(const fcc::CaloHitCollection& aCells, fcc::PositionedCaloHitCollection& outputColl) final;

  virtual dd4hep::Position xyzPosition(const uint64_t& aCellId) const final;

  virtual int layerId(const uint64_t& aCellId) final;

private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
};
#endif /* RECCALORIMETER_CELLPOSITIONSDUMMYTOOL_H */
