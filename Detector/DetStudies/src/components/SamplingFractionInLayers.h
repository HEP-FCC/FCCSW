#ifndef DETSTUDIES_SAMPLINGFRACTIONINLAYERS_H
#define DETSTUDIES_SAMPLINGFRACTIONINLAYERS_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;

// datamodel
namespace fcc {
class PositionedCaloHitCollection;
}

class TH1F;
class ITHistSvc;
/** @class SamplingFractionInLayers SamplingFractionInLayers.h
 *
 *  Histograms of energy deposited in active material and total energy deposited in the calorimeter.
 *  Passive material needs to be marked as sensitive. It needs to be divided into layers (cells) as active material.
 *  Layers (cells) are numbered starting at 1 so that energy depoisted in cryostat and bath could be easily recognised.
 *  Sampling fraction is calculated for each layer as the ratio of energy deposited in active material to energy
 *  deposited in the layer (also in passive material).
 *
 *  @author Anna Zaborowska
 */

class SamplingFractionInLayers : public GaudiAlgorithm {
public:
  explicit SamplingFractionInLayers(const std::string&, ISvcLocator*);
  virtual ~SamplingFractionInLayers();
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
  /// Name of the active field
  Gaudi::Property<std::string> m_activeFieldName{this, "activeFieldName", "", "Identifier of active material"};
  /// Value of the active material
  Gaudi::Property<int> m_activeFieldValue{this, "activeFieldValue", 0, "Value of identifier for active material"};
  /// Name of the layer/cell field
  Gaudi::Property<std::string> m_layerFieldName{this, "layerFieldName", "", "Identifier of layers"};
  /// Number of layers/cells
  Gaudi::Property<uint> m_numLayers{this, "numLayers", 8, "Number of layers"};
  /// Id of the first layer
  Gaudi::Property<uint> m_firstLayerId{this, "firstLayerId", 0, "ID of first layer"};
  /// Name of the detector readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "", "Name of the detector readout"};
  // Maximum energy for the axis range
  Gaudi::Property<double> m_energy{this, "energyAxis", 500, "Maximum energy for axis range"};
  // Histograms of total deposited energy within layer
  // Layers are numbered starting at 1. Layer 0 includes total energy deposited in cryostat and bath (in front and
  // behind calo)
  std::vector<TH1F*> m_totalEnLayers;
  // Histogram of total deposited energy in the calorimeter (in active and passive material, excluding cryostat and
  // bath)
  TH1F* m_totalEnergy;
  // Histograms of energy deposited in the active material within layer
  std::vector<TH1F*> m_activeEnLayers;
  // Histogram of energy deposited in the active material of the calorimeter
  TH1F* m_totalActiveEnergy;
  // Histograms of sampling fraction (active/total energy) calculated within layer
  std::vector<TH1F*> m_sfLayers;
  // Histogram of sampling fraction (active/total energy) calculated for the calorimeter (excluding cryostat and bath)
  TH1F* m_sf;
};
#endif /* DETSTUDIES_SAMPLINGFRACTIONINLAYERS_H */
