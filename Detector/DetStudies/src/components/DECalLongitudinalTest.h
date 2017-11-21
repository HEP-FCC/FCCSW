#ifndef DETSTUDIES_DECALLONGITUDINALTEST_H
#define DETSTUDIES_DECALLONGITUDINALTEST_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "TH2F.h"
class IGeoSvc;

// datamodel
namespace fcc {
class PositionedCaloHitCollection;
class CaloHitCollection;
}

class TH1F;
class ITHistSvc;
/** @class DECalLongitudinalTest DECalLongitudinalTest.h
 *
 *  Histograms of energy deposited in active material and total energy deposited in the calorimeter.
 *  Passive material needs to be marked as sensitive. It needs to be divided into layers (cells) as active material.
 *  Layers (cells) are numbered starting at 1 so that energy depoisted in cryostat and bath could be easily recognised.
 *  Sampling fraction is calculated for each layer as the ratio of energy deposited in active material to energy
 *  deposited in the layer (also in passive material).
 *
 *  @author Anna Zaborowska
 */

class DECalLongitudinalTest : public GaudiAlgorithm {
public:
  explicit DECalLongitudinalTest(const std::string&, ISvcLocator*);
  virtual ~DECalLongitudinalTest();
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

private:
  /// Pointer to the interface of histogram service
  ServiceHandle<ITHistSvc> m_histSvc;
  /// Pointer to the geometry service
  ServiceHandle<IGeoSvc> m_geoSvc;
  /// Handle for the energy deposits
  DataHandle<fcc::PositionedCaloHitCollection> m_deposits{"rec/caloHits", Gaudi::DataHandle::Reader, this};
  /// Name of the layer/cell field
  Gaudi::Property<std::string> m_layerFieldName{this, "layerFieldName", "", "Identifier of layers"};
  /// Number of layers/cells
  Gaudi::Property<uint> m_numLayers{this, "numLayers", 8, "Number of layers"};
  /// Name of the detector readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "", "Name of the detector readout"};

  // Histograms of total deposited energy within layer
  // Histograms for the number of hits per layer
  std::vector<TH1F*> m_totalHitsLayers;
  // Histogram of total hits in the calorimeter
  TH1F* m_totalHits;
  // Histogram of number of hits per layer in single histogram
  TH1F* m_totalHitsVsLayer;

  TH2F* m_XYEvent0;
  TH2F* m_EtaPhiEvent0;
  TH1F* m_EtaPhiSeparation;  
};
#endif /* DETSTUDIES_DECALLONGITUDINALTEST_H */
