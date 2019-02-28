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
class MCParticleCollection;
class GenVertexCollection;
}

namespace dd4hep {
namespace DDSegmentation {
class FCCSWGridPhiEta;
class MultiSegmentation;
class BitFieldCoder;
}
}

#include "TH1F.h"

/** @class CorrectCluster
 *
 *  Apply corrections to a reconstructed cluster.
 *  Corrections for eta position and upstream energy are based on the cells in cluster, therefore they do not
 * acknowledge the energy sharing between neighbouring clusters.
 *
 *  Corrections:
 *  1) Pseudorapidity position (correction for finite granularity of detector using log-weighting)
 *      Correction uses only cells from the first defined detector in m_systemId (default ECal Barrel). The number of
 *      layers used for this correction is specified in *numLayers*. Weights are defined for each layer in *etaRecalcWeights*.
 *  2) Energy correction for pileup noise, which is parametrised per cluster as  P0 * number_of_cells ^P1. Parameters
 *      are eta-dependent and read from file *noiseFileName* as histograms called as in *pileupHistoName*
 *      (+ 0/1 respectively).
 *  3) Energy correction for the upstream material. The energy upstream is calculated as (P00 + P01 * E_clu) + (P10 +
 *      P11 * sqrt(E_clu) ) * E_firstLayer. Parameters P00, P01, P10 and P11 are eta-dependent and specified in
 *      *presamplerShiftP0*, *presamplerShiftP1*, *presamplerScaleP0* and *presamplerScaleP1*, respectively.
 *      The eta values for which the parameters are extracted are defined in *etaValues*.
 *      Energy deposited in the first layer is the uncalibrated energy, therefore sampling fraction used for calibration
 *      needs to be given in *samplingFraction*.
 *
 *  Several histograms are filled in, to monitor the upstream correction, the pileup noise, and the energy prior to the
 * corrections, as well as afterwards, and the position (eta, phi) resolution.
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
  /**  Correct way to access the neighbour of the phi tower, taking into account the full coverage in phi.
   *   Full coverage means that first tower in phi, with ID = 0 is a direct neighbour
   *   of the last tower in phi with ID = m_nPhiTower - 1).
   *   @param[in] aIPhi requested ID of a phi tower, may be < 0 or >= aMaxPhi
   *   @param[in] aMaxPhi maximum phi
   *   @return  ID of a tower - shifted and corrected (in [0, aMaxPhi) range)
   */
  unsigned int phiNeighbour(int aIPhi, int aMaxPhi) const;
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
  /// Handle for particles with truth position and energy information: for SINGLE PARTICLE EVENTS (input collection)
  DataHandle<fcc::MCParticleCollection> m_particle{"particles", Gaudi::DataHandle::Reader, this};
  /// Handle for the genvertices to read vertex position information
  DataHandle<fcc::GenVertexCollection> m_vertex{"vertices", Gaudi::DataHandle::Reader, this};
  /// Pointer to the interface of histogram service
  ServiceHandle<ITHistSvc> m_histSvc;
  /// Histogram of energy before any correction
  TH1F* m_hEnergyPreAnyCorrections;
  /// Histogram of energy after all corrections
  TH1F* m_hEnergyPostAllCorrections;
  /// Histogram of energy after all corrections and scaled to restore response = 1
  TH1F* m_hEnergyPostAllCorrectionsAndScaling;
  /// Ratio of energy in layers
  TH1F* m_hEnergyFractionInLayers;
  /// Histogram of eta resolution
  TH1F* m_hDiffEta;
  TH1F* m_hDiffEtaResWeight;
  TH1F* m_hDiffEtaResWeight2point;
  /// Histogram of eta resolution per layer
  std::vector<TH1F*> m_hDiffEtaLayer;
  /// Histogram of phi resolution
  TH1F* m_hDiffPhi;
  /// Histogram of eta
  TH1F* m_hEta;
  /// Histogram of phi
  TH1F* m_hPhi;
  /// Number of cells inside cluster
  TH1F* m_hNumCells;
  /// Energy of the centre of energy distribution histograms
  Gaudi::Property<double> m_response{this, "response", 0.95, "Reconstructed energy (in the cluster) used for scaling"};
  /// Energy of the centre of energy distribution histograms
  Gaudi::Property<double> m_energy{this, "energyAxis", 500, "Energy of the centre of energy distribution histograms"};
  /// Weights for each detector layer for eta position log-weighting
  Gaudi::Property<std::vector<double>> m_etaRecalcLayerWeights{
    this,
      "etaRecalcWeights",
        {3.5, 5.5, 4.75, 4, 3.75, 3.5, 7, 7},
      "Weights for each detector layer for eta position log-weighting"};
  Gaudi::Property<std::vector<double>> m_etaLayerResolutionSampling{
    this,
      "etaLayerResolutionSampling",
        {0, 1.405, 2.051, 3.195, 5.791, 10.28, 17.54, 30.5},
      "sampling term of eta resolution"};
  Gaudi::Property<std::vector<double>> m_etaLayerResolutionConst{
    this,
      "etaLayerResolutionConst",
        {2.623, 0.1083, 0.1152, 0, 0, 0, 0, 0},
      "const term of eta resolution"};
  /// number of layers in the systems as in m_systemId
  Gaudi::Property<uint> m_numLayers{this, "numLayers", 8, "Number of layers for which the eta position is calculated"};
  /// Name of the layer/cell field
  Gaudi::Property<std::string> m_layerFieldName{this, "layerFieldName", "layer", "Identifier of layers"};
  /// Id of the first layer
  Gaudi::Property<uint> m_firstLayerId{this, "firstLayerId", 0, "ID of first layer"};
  /// IDs of the detectors
  Gaudi::Property<std::vector<uint>> m_systemId{this, "systemId", {5}, "IDs of systems"};
  /// Names of the detector readout, corresponding to system IDs in m_systemId
  Gaudi::Property<std::vector<std::string>> m_readoutName{
      this, "readoutName", {"ECalBarrelPhiEta"}, "Names of the detector readout, corresponding to systemId"};
  /// Pointer to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;
  /// map of system Id to segmentation, created based on m_readoutName and m_systemId
  std::map<uint, dd4hep::DDSegmentation::FCCSWGridPhiEta*> m_segmentationPhiEta;
  std::map<uint, dd4hep::DDSegmentation::MultiSegmentation*> m_segmentationMulti;
  /// map of system Id to decoder, created based on m_readoutName and m_systemId
  std::map<uint, dd4hep::DDSegmentation::BitFieldCoder*> m_decoder;
  /// Histogram of pileup noise added to energy of clusters
  TH1F* m_hPileupEnergy;
  /// Random Number Service
  IRndmGenSvc* m_randSvc;
  /// Gaussian random number generator used for the generation of random noise hits
  Rndm::Numbers m_gauss;
  /// Name of the file with noise constants
  Gaudi::Property<std::string> m_noiseFileName{this, "noiseFileName", "TestPileup_Cluster_mu200_700files.root",
                                               "Name of the file with noise constants"};
  /// Name of pileup histogram for correction of "pileupHistoName"+0 * num_cells ^ "pileupHistoName"+1
  Gaudi::Property<std::string> m_pileupHistoName{this, "pileupHistoName", "histFitToClusterDependence_Measured_p",
                                                 "Name of pileup histogram"};
  /// Pile-up scenario for pileup noise scaling (with sqrt(mu))
  Gaudi::Property<int> m_mu{this, "mu", 1000, "Pileup scenario"};
  /// Histograms with pileup constants (index in array - parameter number: 0 or 1)
  std::vector<TH1F> m_histoPileupConst;
  /// Values of eta corresponding to the upstream correction parameters
  Gaudi::Property<std::vector<double>> m_etaValues{this, "etaValues", {0}, "Eta values"};
  /// Borders of the eta bins for the upstream correction (middle between eta values)
  std::vector<double> m_etaBorders;
  /// Upstream correction parameter P00 in E_up = (P00 + P01 * E) + (P10 + P11 * sqrt(E) ) * E
  Gaudi::Property<std::vector<double>> m_presamplerShiftP0{
      this, "presamplerShiftP0", {0}, "Upstream material param 00 as fnc of eta"};
  /// Upstream correction parameter P10 in E_up = (P00 + P01 * E) + (P10 + P11 * sqrt(E) ) * E
  Gaudi::Property<std::vector<double>> m_presamplerScaleP0{
      this, "presamplerScaleP0", {0}, "Upstream material param 10 as fnc of eta"};
  /// Upstream correction parameter P01 in E_up = (P00 + P01 * E) + (P10 + P11 * sqrt(E) ) * E
  Gaudi::Property<std::vector<double>> m_presamplerShiftP1{
      this, "presamplerShiftP1", {0}, "Upstream material param 01 as fnc of eta"};
  /// Upstream correction parameter P11 in E_up = (P00 + P01 * E) + (P10 + P11 * sqrt(E) ) * E
  Gaudi::Property<std::vector<double>> m_presamplerScaleP1{
      this, "presamplerScaleP1", {0}, "Upstream material param 11 as fnc of eta"};
  /// Values of sampling fraction used for energy calibration
  Gaudi::Property<std::vector<double>> m_samplingFraction{this,
                                                          "samplingFraction",
                                                          {0.299041341789, 0.1306220735, 0.163243999965, 0.186360269398,
                                                           0.203778124831, 0.216211280314, 0.227140796653,
                                                           0.243315422934},
                                                          "Values of sampling fraction used in energy calibration"};
  /// maximum eta value of generated particles (for ranges in histograms)
  Gaudi::Property<double> m_etaMax{this, "etaMaxAxis", 1.6805, "Max eta value"};
  /// maximum phi value of generated particles (for ranges in histograms)
  Gaudi::Property<double> m_phiMax{this, "phiMaxAxis", M_PI, "Max phi value"};
  /// segmentation of detetor in eta (for number of bins in histograms)
  Gaudi::Property<double> m_dEta{this, "dEta", 0.01, "Segmentation in eta"};
  /// segmentation of detetor in phi (for number of bins in histograms)
  Gaudi::Property<double> m_dPhi{this, "dPhi", 2*M_PI/704, "Segmentation in phi"};
  /// Histogram of upstream energy added to energy of clusters
  TH1F* m_hUpstreamEnergy;
  /// Size of the window in phi for the final cluster building, optimised for each layer  (in units of cell size)
  /// If empty use same size for each layer, as in *nPhiFinal*
  Gaudi::Property<std::vector<int>> m_nPhiFinal{this, "nPhiOptimFinal", {}};
  // Recalculate to half size N (window size = 2*N+1)
  std::vector<int> m_halfPhiFin;
  /// Size of the window in eta for the final cluster building, optimised for each layer  (in units of cell size)
  /// If empty use same size for each layer, as in *nEtaFinal*
  Gaudi::Property<std::vector<int>> m_nEtaFinal{this, "nEtaOptimFinal", {}};
  // Recalculate to half size N (window size = 2*N+1)
  std::vector<int> m_halfEtaFin;
  /// Flag for the ellipse used in the final cluster instead of the rectangle
  Gaudi::Property<bool> m_ellipseFinalCluster{this, "ellipse", false};
  /// Use this value of pileup noise (to overwrite the value read from file and calculated based on num of cells)
  Gaudi::Property<double> m_constPileupNoise{this, "constPileupNoise", 0};
};

#endif /* RECCALORIMETER_CORRECTCLUSTER_H */
