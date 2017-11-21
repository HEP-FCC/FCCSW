#ifndef DETSTUDIES_SiWAnalysis_H
#define DETSTUDIES_SiWAnalysis_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "TH2F.h"
#include "TTree.h"
#include "TF1.h"
#include "TProfile.h"

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

class SiWAnalysis : public GaudiAlgorithm {
public:
  explicit SiWAnalysis(const std::string&, ISvcLocator*);
  virtual ~SiWAnalysis();
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
  DataHandle<fcc::CaloHitCollection> m_deposits{"rec/deposits", Gaudi::DataHandle::Reader, this};
  /// Name of the detector readout
  Gaudi::Property<std::string> m_padReadoutName{this, "padReadoutName", "", "Name of the detector readout"};

  
  DataHandle<fcc::MCParticleCollection> m_truth{"truth", Gaudi::DataHandle::Reader, this};



  // calibration factors to compensate non linearity
  float m_calP0, m_calP1, m_calP2;
  TF1* m_calFit;

  TTree* m_treeInfo;
  int m_treeEvtNumber;

  int m_truthEnergy;
  float m_sumEnergyDep;
  std::vector<float> m_sumEdepLayers;

  double m_fitE0;
  double m_fita;
  double m_fitb;
  double m_rising50;
  double m_rising90;
  double m_fwhm;

  TF1* m_fitLongProfile;
  TH1F* m_totalEdep;
};
#endif /* DETSTUDIES_SiWAnalysis_H */
