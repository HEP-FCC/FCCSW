#ifndef RECONSTRUCTION_MERGEHITSTOCALOCELLSTOOL_H
#define RECONSTRUCTION_MERGEHITSTOCALOCELLSTOOL_H

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "RecInterface/IMergeHitsToCaloCellsTool.h"
class IGeoSvc;

// our EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloClusterCollection.h"

// DD4hep
#include "DD4hep/Readout.h"
namespace DD4hep {
namespace DDSegmentation {
  class Segmentation;
}
}


/** @class MergeHitsToCaloCellsTool
 *
 *  Tool for merging Geant4 hits with the same cellID to a given cell
 *  TODO: 
 *     - use map instead of the second for-loop
 *       (std::unordered_map<int, fcc::CaloHit> cellIdMap;)
 *  @author Jana Faltova
 *  @date   2016-08
 */


class MergeHitsToCaloCellsTool : public GaudiTool, virtual public IMergeHitsToCaloCellsTool  {
public:
  MergeHitsToCaloCellsTool(const std::string& type,const std::string& name, const IInterface* parent);
  ~MergeHitsToCaloCellsTool();
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  //Merge Geant4 energy deposits (calohits) into cells (m_vectorCells)
  virtual void DoMerge(const fcc::CaloHitCollection& aHits, std::vector<fcc::CaloHit*>& aCells) final;
private:
  /// Name of the detector readout
  std::string m_readoutName;
  std::string m_fieldName;

  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  std::unique_ptr<DD4hep::DDSegmentation::BitField64> m_decoder;

};

#endif /* RECONSTRUCTION_MERGEHITSTOCALOCELLSTOOL_H */
