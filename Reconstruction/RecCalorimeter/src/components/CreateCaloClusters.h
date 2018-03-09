#ifndef RECCALORIMETER_CREATECALOCLUSTERS_H
#define RECCALORIMETER_CREATECALOCLUSTERS_H

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ICalorimeterTool.h"
#include "RecInterface/ICaloReadPileUpNoise.h"

// DD4hep
#include "DDSegmentation/Segmentation.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "datamodel/CaloHit.h"
#include "datamodel/CaloCluster.h"
#include "datamodel/CaloClusterCollection.h"

class IGeoSvc;
namespace DD4hep {
namespace DDSegmentation {
class Segmentation;
}
}

/** @class CreateCaloClusters
 *
 *  Tools called:
 *    - ReadPileupParameters
 *
 *  @author Coralie Neubueser
 *  @date   2018-03
 *
 */

class CreateCaloClusters : public GaudiAlgorithm {

public:
  CreateCaloClusters(const std::string& name, ISvcLocator* svcLoc);

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:

  /// Handle for calo hits (input collection)
  DataHandle<fcc::CaloClusterCollection> m_clusters{"clusters", Gaudi::DataHandle::Reader, this};

//  /// Handle for the cells noise tool in ECal
//  ToolHandle<ICaloReadPileUpNoise> m_ecalNoiseTool{"ReadPileUpNoiseFromFileTool", this};
//  /// Handle for the cells noise tool in HCal
//  ToolHandle<ICaloReadPileUpNoise> m_hcalNoiseTool{"ReadPileUpNoiseFromFileTool", this};

  /// bool if calibration is applied
  bool m_doCalibration;
  /// e/h of ECal
  double m_ehECal;
  /// e/h of HCal
  double m_ehHCal;

  dd4hep::DDSegmentation::BitField64* m_decoder = new dd4hep::DDSegmentation::BitField64("system:4");
  /// System id by default Barrel, EC(6,7), Fwd(10,11)
  Gaudi::Property<int> m_systemIdECal{this, "systemECal", 5, "System id of ECal"};
  Gaudi::Property<int> m_systemIdHCal{this, "systemHCal", 8, "System id of HCal"};
  Gaudi::Property<double> m_fractionECal{this, "fractionECal", 0.7, "Fraction of clsuter energy in ECal to be flagged as EM"};

};

#endif /* RECCALORIMETER_CREATECALOCLUSTERS_H */
