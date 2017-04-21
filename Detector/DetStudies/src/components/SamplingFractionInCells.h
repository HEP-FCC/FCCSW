#ifndef DETSTUDIES_SAMPLINGFRACTIONINCELLS_H
#define DETSTUDIES_SAMPLINGFRACTIONINCELLS_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;

// datamodel
namespace fcc {
class PositionedCaloHitCollection;
}

class TH1F;
class ITHistSvc;

/** @class SamplingFractionInCells SamplingFractionInCells.h
 *
 *  @author Anna Zaborowska
 */

class SamplingFractionInCells: public GaudiAlgorithm {
public:
  explicit SamplingFractionInCells(const std::string&, ISvcLocator*);
  virtual ~SamplingFractionInCells();
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
  SmartIF<ITHistSvc> m_histSvc;
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Handle for the energy deposits
  DataHandle<fcc::PositionedCaloHitCollection> m_deposits{"rec/caloHits", Gaudi::DataHandle::Reader, this};
  /// Name of the active field
  Gaudi::Property<std::string> m_activeFieldName{this, "activeFieldName", "", "Identifier of active material"};
  /// Value of the active material
  Gaudi::Property<int> m_activeFieldValue{this, "activeFieldValue", 0, "Value of identifier for active material"};
  /// Name of the cells/layer field
  Gaudi::Property<std::string> m_layerFieldName{this, "layerFieldName", "", "Identifier of layers"};
  /// Number of layers/cells cells
  Gaudi::Property<int> m_numLayers{this, "numLayers", 8, "Number of layers"};
  /// Name of the detector readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "", "Name of the detector readout"};
  // Maximum energy for the axis range
  Gaudi::Property<double> m_energy{this, "energyAxis", 500, "Maximum energy for axis range"};
  // histograms
  std::vector<TH1F*> m_cellsEnergy;
  TH1F* m_totalEnergy;
  std::vector<TH1F*> m_cellsActiveEnergy;
  TH1F* m_totalActiveEnergy;
  std::vector<TH1F*> m_cellsSF;
  TH1F* m_SF;
};
#endif /* DETSTUDIES_SAMPLINGFRACTIONINCELLS_H */
