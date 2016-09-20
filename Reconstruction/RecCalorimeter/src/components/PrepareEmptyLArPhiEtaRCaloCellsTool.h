#ifndef RECCALORIMETER_PREPAREEMPTYLARPHIETARCALOCELLSTOOL_H
#define RECCALORIMETER_PREPAREEMPTYLARPHIETARCALOCELLSTOOL_H

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

/** @class PrepareEmptyLArPhiEtaRCaloCellsTool
 *
 *  Tool for preparing a collection of all calorimeter cells with their cellID
 *  Loops through all cells in the calorimeter with PhiEta segmentation and
 *  create a vector of cells (caloHits) with cellID, everything else set to 0
 *  Only for Tube shapes with PhiEta segmentation! (OK for calorimeter) 
 *  LAr ECAL specific
 *
 *  @author Jana Faltova
 *  @date   2016-09
 *
 */

class PrepareEmptyLArPhiEtaRCaloCellsTool : public GaudiTool, virtual public IPrepareEmptyCaloCellsTool 
{
public:
  PrepareEmptyLArPhiEtaRCaloCellsTool(const std::string& type,const std::string& name, const IInterface* parent);
  ~PrepareEmptyLArPhiEtaRCaloCellsTool();
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  virtual std::vector<fcc::CaloHit*> PrepareEmptyCells() final;

private:
    /// Name of the detector readout
  std::string m_readoutName;
  /// Name of active volumes (material name)
  std::string m_activeVolumeName;
  /// Number of volumes with same name as active layer (not to be counted as a layer - ECAL LAr bath in cryostat)
  unsigned m_numVolumesRemove;
  /// Name of active layers for sampling calorimeter
  std::string m_activeFieldName;
  /// Name of the fields describing the segmented volume
  std::vector<std::string> m_fieldNames;
  /// Values of the fields describing the segmented volume
  std::vector<int> m_fieldValues;

  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// PhiEta segmentation
  DD4hep::DDSegmentation::GridPhiEta* m_segmentation;
 
  std::vector<fcc::CaloHit*> m_caloCellsCollection;

};

#endif /* RECCALORIMETER_PREPAREEMPTYLARPHIETACALOCELLSTOOL_H */
