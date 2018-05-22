#ifndef RECCALORIMETER_CELLPOSITIONSTAILCATCHERTOOL_H
#define RECCALORIMETER_CELLPOSITIONSTAILCATCHERTOOL_H

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

/** @class CellPositionsTailCatcherTool Reconstruction/RecFCChhCalorimeter/src/components/CellPositionsTailCatcherTool.h
 *   CellPositionsTailCatcherTool.h
 *
  *  Tool to determine each Calorimeter cell position.
 *
 *  For the FCChh Tail Catcher, determined from the placed volumes and the FCCSW eta-phi segmentation.   
 * 
 *  @author Coralie Neubueser
*/

class CellPositionsTailCatcherTool : public GaudiTool, virtual public ICellPositionsTool {
public:
  CellPositionsTailCatcherTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~CellPositionsTailCatcherTool() = default;

  virtual StatusCode initialize() final;

  virtual StatusCode finalize() final;

  virtual void getPositions(const fcc::CaloHitCollection& aCells, fcc::PositionedCaloHitCollection& outputColl) final;

  virtual dd4hep::Position xyzPosition(const uint64_t& aCellId) const final;

  virtual int layerId(const uint64_t& aCellId) final;

private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Name of the electromagnetic calorimeter readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "name of the readout"};
  /// Radius of central Tail Catcher cylinder
  Gaudi::Property<double> m_centralRadius{this, "centralRadius", {}, "radius in cm"};
  /// Eta-phi segmentation
  dd4hep::DDSegmentation::FCCSWGridPhiEta* m_segmentation;
  /// Cellid decoder
  dd4hep::DDSegmentation::BitFieldCoder* m_decoder;
  /// Volume manager
  dd4hep::VolumeManager m_volman;
};
#endif /* RECCALORIMETER_CELLPOSITIONSTAILCATCHERTOOL_H */
