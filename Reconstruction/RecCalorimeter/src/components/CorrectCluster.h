#ifndef RECCALORIMETER_CORRECTCLUSTER_H
#define RECCALORIMETER_CORRECTCLUSTER_H

// FCCSW
#include "FWCore/DataHandle.h"
#include "GaudiKernel/RndmGenerators.h"
class IGeoSvc;
class IRndmGenSvc;
class ITHistSvc;

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

namespace fcc {
 class CaloClusterCollection;
 class CaloHitCollection;
}

namespace dd4hep {
namespace DDSegmentation {
class FCCSWGridPhiEta;
class BitField64;
}
}

#include "TH1F.h"

/** @class CorrectCluster
 *
 *  Apply corrections to a reconstructed cluster
 *
 *  Possible corrections:
 *  ...
 *
 *  @author Anna Zaborowska
 *
 */

class CorrectCluster : public GaudiAlgorithm {

public:
  CorrectCluster(const std::string& name, ISvcLocator* svcLoc);

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  /** Open file and read noise histograms in the memory
   *  @return Status code if retriving histograms was successful
   */
  StatusCode initNoiseFromFile();
  /** Find the appropriate noise constant from the histogram
   *  @param[in] aEta Pseudorapidity value of the centre of the cluster
   *  @param[in] aNumCells Number of cells in a cluster
   *  @return Width of the Gaussian distribution of noise per cluster
   */
  double getNoiseConstantPerCluster(double aEta, uint numCells);
  /// Handle for clusters (input collection)
  DataHandle<fcc::CaloClusterCollection> m_inClusters{"clusters", Gaudi::DataHandle::Reader, this};
  /// Handle for corrected clusters (output collection)
  DataHandle<fcc::CaloClusterCollection> m_correctedClusters{"correctedClusters", Gaudi::DataHandle::Writer, this};
  /// Pointer to the interface of histogram service
  ServiceHandle<ITHistSvc> m_histSvc;
  /// Histogram of energy before any correction
  TH1F* m_hEnergyPreAnyCorrections;
  /// Histogram of energy after all corrections
  TH1F* m_hEnergyPostAllCorrections;
  /// Energy of the centre of energy distribution histograms
  Gaudi::Property<double> m_energy{this, "energyAxis", 50, "Energy of the centre of energy distribution histograms"};
  /// Weights for each detector layer for eta position log-weighting
  Gaudi::Property<std::vector<double>> m_etaRecalcLayerWeights{this, "etaRecalcWeights", {3.5,5.5,4.75,4,3.75,3.5,7,7}, "Weights for each detector layer for eta position log-weighting"};
  /// number of layers in the systems as in m_systemId
  Gaudi::Property<uint> m_numLayers{this, "numLayers", 8, "Number of layers for which the eta position is calculated"};
  /// Name of the layer/cell field
  Gaudi::Property<std::string> m_layerFieldName{this, "layerFieldName", "layer", "Identifier of layers"};
  /// Id of the first layer
  Gaudi::Property<uint> m_firstLayerId{this, "firstLayerId", 0, "ID of first layer"};
  /// Names of the detector readout
  Gaudi::Property<std::vector<uint>> m_systemId{this, "systemId", {5}, "IDs of systems"};
  /// Names of the detector readout, corresponding to system IDs in m_systemId
  Gaudi::Property<std::vector<std::string>> m_readoutName{this, "readoutName", {"ECalBarrelPhiEta"}, "Names of the detector readout, corresponding to systemId"};
  /// Pointer to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;
  /// map of system Id to segmentation, created based on m_readoutName and m_systemId
  std::map<uint, dd4hep::DDSegmentation::FCCSWGridPhiEta*> m_segmentation;
  /// map of system Id to decoder, created based on m_readoutName and m_systemId
  std::map<uint, dd4hep::DDSegmentation::BitField64*> m_decoder;
  /// Histogram of pileup noise added to energy of clusters
  TH1F* m_hPileupEnergy;
  /// Random Number Service
  IRndmGenSvc* m_randSvc;
  /// Gaussian random number generator used for the generation of random noise hits
  Rndm::Numbers m_gauss;
  /// Name of the file with noise constants
  Gaudi::Property<std::string> m_noiseFileName{this, "noiseFileName", "TestPileup_Cluster_mu200_700files.root", "Name of the file with noise constants"};
  /// Name of pileup histogram
  Gaudi::Property<std::string> m_pileupHistoName{this, "pileupHistoName", "histFitToClusterDependence_Measured_p", "Name of pileup histogram"};
  /// Histograms with pileup constants (index in array - radial layer)
  std::vector<TH1F> m_histoPileupConst;
  /// Values of eta corresponding to the upstream correction parameters
  Gaudi::Property<std::vector<double>> m_etaValues{this, "etaValues", {0}, "Eta values"};
  /// Borders of the eta bins for the upstream correction (middle between eta values)
  std::vector<double> m_etaBorders;
  /// Upstream correction parameter P00 in E_up = (P00 + P01 * E) + (P10 + P11 / sqrt(E) ) * E
  Gaudi::Property<std::vector<double>> m_presamplerShiftP0{this, "presamplerShiftP0", {0}, "Upstream material param 00 as fnc of eta"};
  /// Upstream correction parameter P10 in E_up = (P00 + P01 * E) + (P10 + P11 / sqrt(E) ) * E
  Gaudi::Property<std::vector<double>> m_presamplerScaleP0{this, "presamplerScaleP0", {0}, "Upstream material param 10 as fnc of eta"};
  /// Upstream correction parameter P01 in E_up = (P00 + P01 * E) + (P10 + P11 / sqrt(E) ) * E
  Gaudi::Property<std::vector<double>> m_presamplerShiftP1{this, "presamplerShiftP1", {0}, "Upstream material param 01 as fnc of eta"};
  /// Upstream correction parameter P11 in E_up = (P00 + P01 * E) + (P10 + P11 / sqrt(E) ) * E
  Gaudi::Property<std::vector<double>> m_presamplerScaleP1{this, "presamplerScaleP1", {0}, "Upstream material param 11 as fnc of eta"};
  /// Histogram of upstream energy added to energy of clusters
  TH1F* m_hUpstreamEnergy;
};

#endif /* RECCALORIMETER_CORRECTCLUSTER_H */
