#ifndef RECCALORIMETER_CREATECALOCLUSTERS_H
#define RECCALORIMETER_CREATECALOCLUSTERS_H

// FCCSW
#include "FWCore/DataHandle.h"
#include "RecInterface/ICalorimeterTool.h"
#include "RecInterface/INoiseConstTool.h"
#include "RecInterface/ICellPositionsTool.h"

// DD4hep
#include "DDSegmentation/Segmentation.h"

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ITHistSvc.h"

#include "datamodel/CaloHit.h"
#include "datamodel/CaloCluster.h"
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloClusterCollection.h"

class IGeoSvc;
namespace DD4hep {
namespace DDSegmentation {
class Segmentation;
}
}

class TH2F;
class TH1F;

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
  /// Pointer to the interface of histogram service
  ITHistSvc* m_histSvc{nullptr};
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;

  /// Handle for calo clusters (input collection)
  DataHandle<fcc::CaloClusterCollection> m_clusters{"calo/clusters", Gaudi::DataHandle::Reader, this};
  /// Handle for calo clusters (output collection)
  DataHandle<fcc::CaloClusterCollection> m_newClusters{"calo/calibClusters", Gaudi::DataHandle::Writer, this};
  // Handle for calo cells (output collection)
  DataHandle<fcc::CaloHitCollection> m_newCells{"calo/calibClusterCells", Gaudi::DataHandle::Writer, this};

  /// Handle for tool to get positions in ECal Barrel
  ToolHandle<ICellPositionsTool> m_cellPositionsECalTool{"CellPositionsECalBarrelTool", this};
  /// Handle for tool to get positions in HCal Barrel and Ext Barrel, no Segmentation
  ToolHandle<ICellPositionsTool> m_cellPositionsHCalTool{"CellPositionsHCalBarrelNoSegTool", this};

  /// ATTENTION!!!Used as input of pile-up noise scaling factors!!!
  /// Handle for the calorimeter cells noise tool
  ToolHandle<INoiseConstTool> m_noiseECalTool{"NoiseCaloCellsFlatTool", this};
  /// Handle for the calorimeter cells noise tool
  ToolHandle<INoiseConstTool> m_noiseHCalTool{"NoiseCaloCellsFlatTool", this};

  const char *types[2] = {"EM", "HAD"};

  TH1F* m_energyScale;
  TH2F* m_energyScaleVsClusterEnergy;
  TH1F* m_totEnergy;
  TH1F* m_totCalibEnergy;
  TH1F* m_totBenchmarkEnergy;
  TH1F* m_clusterEnergy;
  TH1F* m_clusterEnergyCalibrated;
  TH1F* m_clusterEnergyBenchmark;

  /// bool if calibration is applied
  bool m_doCalibration =  true;
  /// bool if calibration is applied                                                                                                                                                                                                      
  bool m_addNoise;

 /// e/h of ECal
  double m_ehECal;
  /// e/h of HCal
  double m_ehHCal;
   /// bool if energy loss needs correction is applied
  bool m_doCryoCorrection =  true;

  dd4hep::DDSegmentation::BitField64* m_decoder = new dd4hep::DDSegmentation::BitField64("system:4");
  dd4hep::DDSegmentation::BitField64* m_decoderECal;
  dd4hep::DDSegmentation::BitField64* m_decoderHCal;

  /// System id by default Barrel, EC(6,7), Fwd(10,11)
  Gaudi::Property<float> m_a{this, "a", 0.978, "scaling of ECal energy"};
  Gaudi::Property<float> m_b{this, "b", 0.479, "scaling of energy loss in cryostat"};
  Gaudi::Property<int> m_lastECalLayer{this, "lastECalLayer", 7, "Layer id of last ECal layer"};
  Gaudi::Property<int> m_firstHCalLayer{this, "firstHCalLayer", 0, "Layer id of first HCal layer"};

  Gaudi::Property<uint> m_systemIdECal{this, "systemECal", 5, "System id of ECal"};
  Gaudi::Property<uint> m_systemIdHCal{this, "systemHCal", 8, "System id of HCal"};
  Gaudi::Property<std::string> m_readoutECal{this, "readoutECal", "Readout of ECal"};
  Gaudi::Property<std::string> m_readoutHCal{this, "readoutHCal", "Readout of HCal"};

  Gaudi::Property<double> m_fractionECal{this, "fractionECal", 0.7, "Fraction of clsuter energy in ECal to be flagged as EM"};

};

#endif /* RECCALORIMETER_CREATECALOCLUSTERS_H */
