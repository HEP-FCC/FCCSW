#ifndef DETSTUDIES_DECalAnalysis_H
#define DETSTUDIES_DECalAnalysis_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "TH2F.h"
#include "TTree.h"
#include "TF1.h"

class IGeoSvc;

// datamodel
namespace fcc {
class PositionedCaloHitCollection;
class CaloHitCollection;
class MCParticleCollection;
}

class TH1F;
class ITHistSvc;
/** @class DECalAnalysis DECalAnalysis.h
 *
 *  Histograms of energy deposited in active material and total energy deposited in the calorimeter.
 *  Passive material needs to be marked as sensitive. It needs to be divided into layers (cells) as active material.
 *  Layers (cells) are numbered starting at 1 so that energy depoisted in cryostat and bath could be easily recognised.
 *  Sampling fraction is calculated for each layer as the ratio of energy deposited in active material to energy
 *  deposited in the layer (also in passive material).
 *
 *  @author Anna Zaborowska
 */

class DECalAnalysis : public GaudiAlgorithm {
public:
  explicit DECalAnalysis(const std::string&, ISvcLocator*);
  virtual ~DECalAnalysis();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Fills the histograms.
   *   @return status code
   */
  virtual StatusCode execute() final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;

  Double_t FitLongProfile( Double_t *x, Double_t *par);

private:
  /// Pointer to the interface of histogram service
  ServiceHandle<ITHistSvc> m_histSvc;
  /// Pointer to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;
  /// Number of layers/cells
  Gaudi::Property<uint> m_numLayers{this, "numLayers", 50, "Number of layers"};
  /// Name of the layer/cell field
  Gaudi::Property<std::string> m_layerFieldName{this, "layerFieldName", "", "Identifier of layers"};

  /// The Pixel analysis
  /// Handle for the energy deposits
  DataHandle<fcc::PositionedCaloHitCollection> m_deposits{"rec/pixels", Gaudi::DataHandle::Reader, this};
  /// Name of the detector readout
  Gaudi::Property<std::string> m_pixelReadoutName{this, "pixelReadoutName", "", "Name of the detector readout"};

  /// The Pad analysis
  /// Handle for the energy deposits
  DataHandle<fcc::CaloHitCollection> m_pads{"rec/pads", Gaudi::DataHandle::Reader, this};
  /// Name of the detector readout
  Gaudi::Property<std::string> m_padReadoutName{this, "padReadoutName", "", "Name of the detector readout"};

  DataHandle<fcc::MCParticleCollection> m_truth{"truth", Gaudi::DataHandle::Reader, this};


  // Histograms of total deposited energy within layer
  // Histograms for the number of hits per layer
  std::vector<TH1F*> m_totalHitsLayers;

  // Histogram of total hits in the calorimeter
  TH1F* m_totalHits;
  // Histogram of number of hits per layer in single histogram
  TH1F* m_totalHitsVsLayer;
  TH1F* m_hitsVsLayerEvent[100];

  TH1F* m_totalPadsVsLayer;
  std::vector<TH1F*> m_totalPadsLayers;
  std::vector<TH1F*> m_totalPixelsPerPadLayers;
  TH1F* m_totalPixelsPerPad;
  TH1F* m_totalPads;
  TH1F* m_totalPadsAbovePixelThreshold;

  TH2F* m_XYEvent0;
  TH2F* m_EtaPhiEvent0;
  TH1F* m_EtaPhiSeparation; 

  // Histogram of the number of hits in the layer with most hits
  TH1F* m_hitsInMaxLayer;
  // Histogram of the layer with maximum hits
  TH1F* m_maxLayer;

  TTree* m_treeInfo;
  int m_treeEvtNumber;

  int m_nEventsFilled;
  int m_truthEnergy;
  int m_sumPixels;
  std::vector<int> m_sumPixelsLayers;
  int m_sumPads;
  std::vector<int> m_sumPadsLayers;

  std::vector<double> m_meanEta;
  std::vector<double> m_meanPhi;

  double m_fitE0;
  double m_fita;
  double m_fitb;
  double m_rising50;
  double m_rising90;
  double m_fwhm;

  TF1* m_fitLongProfile;
};
#endif /* DETSTUDIES_DECalAnalysis_H */
