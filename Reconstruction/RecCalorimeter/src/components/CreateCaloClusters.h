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
#include "datamodel/MCParticleCollection.h"

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
 *    - Cell position tools
 *
 *  @author Coralie Neubueser
 *  @date   2019-03
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
  /// Handle for calo clusters (input collection)
  DataHandle<fcc::MCParticleCollection> m_genParticles{"calo/genParticles", Gaudi::DataHandle::Reader, this};
  /// Handle for calo clusters (output collection)
  DataHandle<fcc::CaloClusterCollection> m_newClusters{"calo/calibClusters", Gaudi::DataHandle::Writer, this};
  // Handle for calo cells (output collection)
  DataHandle<fcc::CaloHitCollection> m_newCells{"calo/calibClusterCells", Gaudi::DataHandle::Writer, this};

  /// Handle for tool to get positions in ECal Barrel
  ToolHandle<ICellPositionsTool> m_cellPositionsECalTool{"CellPositionsECalBarrelTool", this};
  /// Handle for tool to get positions in HCal Barrel and Ext Barrel, no Segmentation
  ToolHandle<ICellPositionsTool> m_cellPositionsHCalTool{"CellPositionsHCalBarrelTool", this};
  /// Handle for tool to get positions in HCal Barrel and Ext Barrel, no Segmentation
  ToolHandle<ICellPositionsTool> m_cellPositionsHCalNoSegTool{"CellPositionsHCalBarrelNoSegTool", this};
  
  const char *types[2] = {"EM", "HAD"};

  TH1F* m_energyScale;
  TH1F* m_benchmark;
  TH1F* m_fractionEMcluster;
  TH2F* m_energyScaleVsClusterEnergy;
  TH1F* m_totEnergy;
  TH1F* m_totCalibEnergy;
  TH1F* m_totBenchmarkEnergy;
  TH1F* m_clusterEnergy;
  TH1F* m_sharedClusterEnergy;
  TH1F* m_clusterEnergyCalibrated;
  TH1F* m_clusterEnergyBenchmark;
  TH1F* m_nCluster;
  TH1F* m_nCluster_1GeV;
  TH1F* m_nCluster_halfTrueEnergy;
  TH1F* m_energyCalibCluster_1GeV;
  TH1F* m_energyCalibCluster_halfTrueEnergy;

  /// bool if calibration is applied
  bool m_doCalibration =  true;

  /// e/h of ECal
  double m_ehECal;
  /// e/h of HCal
  double m_ehHCal;
  /// bool if energy loss needs correction is applied
  bool m_doCryoCorrection = false;
  /// bool if energy dependent correction for energy loss is applied  
  bool m_doEdepCryoCorrection = true;

  dd4hep::DDSegmentation::BitFieldCoder* m_decoder = new dd4hep::DDSegmentation::BitFieldCoder("system:4");
  dd4hep::DDSegmentation::BitFieldCoder* m_decoderECal;
  dd4hep::DDSegmentation::BitFieldCoder* m_decoderHCal;

  /// System id by default Barrel, EC(6,7), Fwd(10,11)
  /// with .. x^b2 .. : -5.53466e-07,4.73147e-11,-1.73903e-05,1515.84,0.823583,-4.87235,150252,9.8425e+09,0.326512  
  /// from chi2 minimisation, no C: 0.975799,-2.54738e-06,0.822663,-0.140975,-2.18657e-05,-0.0193682
  Gaudi::Property<float> m_a1{this, "a1", 0.957, "scaling of ECal energy"}; // no Bfield: 0.9867
  Gaudi::Property<float> m_a2{this, "a2", 3.772e-08, "scaling of ECal energy"}; // no Bfield: 0.9867
  Gaudi::Property<float> m_a3{this, "a3", 1.028, "scaling of ECal energy"}; // no Bfield: 0.9867
  Gaudi::Property<float> m_b1{this, "b1", 0.243, "scaling of energy loss in cryostat"};// no Bfield: 0.432
  Gaudi::Property<float> m_b2{this, "b2", 0.097, "scaling of energy loss in cryostat"};// no Bfield: 0.432
  Gaudi::Property<float> m_b3{this, "b3", -0.217, "scaling of energy loss in cryostat"};// no Bfield: 0.432
  Gaudi::Property<float> m_c1{this, "c1", -1.546e-10, "scaling of energy loss in cryostat"};// no Bfield: -5.567E-6   
  Gaudi::Property<float> m_c2{this, "c2", -4.519e+06, "scaling of energy loss in cryostat"};// no Bfield: -5.567E-6
  Gaudi::Property<float> m_c3{this, "c3", 7.026, "scaling of energy loss in cryostat"};// no Bfield: -5.567E-6                                                                                                                     
  /// no segmentation used in HCal
  Gaudi::Property<bool> m_noSegmentationHCal{this, "noSegmentationHCal", true, "HCal readout w/o eta-phi segementation?"};
  Gaudi::Property<int> m_lastECalLayer{this, "lastECalLayer", 7, "Layer id of last ECal layer"};
  Gaudi::Property<int> m_firstHCalLayer{this, "firstHCalLayer", 0, "Layer id of first HCal layer"};

  Gaudi::Property<uint> m_systemIdECal{this, "systemECal", 5, "System id of ECal"};
  Gaudi::Property<uint> m_systemIdHCal{this, "systemHCal", 8, "System id of HCal"};
  Gaudi::Property<std::string> m_readoutECal{this, "readoutECal", "Readout of ECal"};
  Gaudi::Property<std::string> m_readoutHCal{this, "readoutHCal", "Readout of HCal"};

  Gaudi::Property<double> m_fractionECal{this, "fractionECal", 0.7, "Fraction of clsuter energy in ECal to be flagged as EM"};

};

#endif /* RECCALORIMETER_CREATECALOCLUSTERS_H */
