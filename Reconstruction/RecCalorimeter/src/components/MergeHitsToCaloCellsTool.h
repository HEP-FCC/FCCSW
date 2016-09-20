#ifndef RECCALORIMETER_MERGEHITSTOCALOCELLSTOOL_H
#define RECCALORIMETER_MERGEHITSTOCALOCELLSTOOL_H

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
 *  Tool for merging Geant4 hits with the same cellID to a cell
 *  Geant4 energy deposits are added
 *  TODO
 *    - how do we want to calculate time? Average?
 *  @author Jana Faltova
 *  @date   2016-09
 *
 */


class MergeHitsToCaloCellsTool : public GaudiTool, virtual public IMergeHitsToCaloCellsTool  {
public:
  MergeHitsToCaloCellsTool(const std::string& type,const std::string& name, const IInterface* parent);
  ~MergeHitsToCaloCellsTool();
  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  /// Merge Geant4 energy deposits (aHits) into cells (aCells)
  virtual void DoMerge(const fcc::CaloHitCollection& aHits, std::vector<fcc::CaloHit*>& aCells) final;
private:
  /// Name of the detector readout
  std::string m_readoutName;
  std::string m_fieldName;

  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;

};

#endif /* RECCALORIMETER_MERGEHITSTOCALOCELLSTOOL_H */
