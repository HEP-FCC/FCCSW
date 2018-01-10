#ifndef DETSTUDIES_UPSTREAMMATERIAL_H
#define DETSTUDIES_UPSTREAMMATERIAL_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;

// datamodel
namespace fcc {
class CaloHitCollection;
class MCParticleCollection;
}

class TH2F;
class TH1F;
class ITHistSvc;

/** @class UpstreamMaterial UpstreamMaterial.h
 *
 *  Histograms of energy deposited in the dead material of the the calorimeter.
 *  Cryostat material needs to be marked as sensitive (and therefore ID 'cryo'==1).
 *  Dependence of the energy deposited in the dead material on the energy deposited in each calorimeter layer is
 * plotted.
 *  Dependence of the energy deposited in the dead material on the azimuthal angle of the incoming particle (MC truth)
 * is plotted.
 *
 *  @author Anna Zaborowska
 */

class UpstreamMaterial : public GaudiAlgorithm {
public:
  explicit UpstreamMaterial(const std::string&, ISvcLocator*);
  virtual ~UpstreamMaterial();
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
  // Energy range in the histogram axis
  Gaudi::Property<double> m_energy{this, "energyAxis", 100, "Max energy for the axis of plot"};
  // Phi in the histogram axis
  Gaudi::Property<double> m_phi{this, "phiAxis", M_PI, "Max azimuthal angle for the axis of plot"};
  /// Handle for the energy deposits
  DataHandle<fcc::CaloHitCollection> m_deposits{"det/caloDeposits", Gaudi::DataHandle::Reader, this};
  /// Handle for the particle
  DataHandle<fcc::MCParticleCollection> m_particle{"det/particles", Gaudi::DataHandle::Reader, this};
  /// Pointer to the interface of histogram service
  SmartIF<ITHistSvc> m_histSvc;
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  std::vector<TH2F*> m_upstreamEnergyCellEnergy;
  std::vector<TH1F*> m_cellEnergyPhi;
  /// Name of the active field
  Gaudi::Property<std::string> m_activeFieldName{this, "activeFieldName", "active", "Name of active field"};
  /// Name of the cells/layer field
  Gaudi::Property<std::string> m_layerFieldName{this, "layerFieldName", "layer", "Name of layer"};
  /// Number of layers/cells cells
  Gaudi::Property<uint> m_numLayers{this, "numLayers", 8, "Number of layers"};
  /// Id of the first layer (current design starts layer ids at 0)
  Gaudi::Property<uint> m_firstLayerId{this, "firstLayerId", 0, "ID of first layer"};
  /// Values of sampling fraction
  Gaudi::Property<std::vector<double>> m_samplingFraction{
      this, "samplingFraction", {}, "Values of sampling fraction per layer"};
  /// Name of the detector readout
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "", "Name of the readout"};
};
#endif /* DETSTUDIES_UPSTREAMMATERIAL_H */
