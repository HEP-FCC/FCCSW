#ifndef RECONSTRUCTION_PREPAREEMPTYCALOCELLSTOOL_H
#define RECONSTRUCTION_PREPAREEMPTYCALOCELLSTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "RecInterface/IPrepareEmptyCaloCellsTool.h"
#include "DetSegmentation/GridPhiEta.h"
class IGeoSvc;

// DD4hep
#include "DD4hep/Readout.h"
namespace DD4hep {
namespace DDSegmentation {
  class Segmentation;
}
}

/** @class PrepareEmptyCaloCellsTool
 *
 *  Tool for preparing a collection of all calorimeter cells with their cellID
 *  TODO: 
 *    - get the total number of cells in the calorimeter
 *    - create a new cell - set its cellID, everything else set to 0
 *  @author Jana Faltova
 *  @date   2016-08
 */

class PrepareEmptyCaloCellsTool : public GaudiTool, virtual public IPrepareEmptyCaloCellsTool 
{
public:
  PrepareEmptyCaloCellsTool(const std::string& type,const std::string& name, const IInterface* parent);
  ~PrepareEmptyCaloCellsTool();
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  virtual std::vector<fcc::CaloHit*> PrepareEmptyCells() final;

private:
    /// Name of the detector readout
  std::string m_readoutName;
  std::string m_fieldName;

  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  DD4hep::DDSegmentation::GridPhiEta* m_segmentation;
  std::unique_ptr<DD4hep::DDSegmentation::BitField64> m_decoder;
  /// Volume ID of the volume with cells to calculate
  uint64_t m_volumeId;
  uint64_t m_cellId;

  std::vector<fcc::CaloHit*> m_caloCellsCollection;

};

#endif /* RECONSTRUCTION_PREPAREEMPTYCALOCELLSTOOL_H */
