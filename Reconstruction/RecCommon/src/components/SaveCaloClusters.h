#ifndef RECONSTRUCTION_SAVECALOCLUSTERS_H
#define RECONSTRUCTION_SAVECALOCLUSTERS_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "DetSegmentation/GridPhiEta.h"
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

class SaveCaloClusters : public GaudiAlgorithm 
{
public:
  SaveCaloClusters(const std::string& name, ISvcLocator* svcLoc);
 
  ~SaveCaloClusters();

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  /// Handle for calo cells (input collection with cellID)
  DataHandle<fcc::CaloHitCollection> m_caloCells;
  /// Handle for calo cells - cluster (output collection)
  DataHandle<fcc::CaloClusterCollection> m_caloClusters;
  /// Name of the detector readout
  std::string m_readoutName;
  std::string m_fieldName;

  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  DD4hep::DDSegmentation::GridPhiEta* m_segmentation;
  std::unique_ptr<DD4hep::DDSegmentation::BitField64> m_decoder;

};

#endif /* RECONSTRUCTION_SAVECALOCLUSTERS_H */
