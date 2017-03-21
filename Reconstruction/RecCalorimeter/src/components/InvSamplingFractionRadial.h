#ifndef RECCALORIMETRY_INVSAMPLINGFRACTIONRADIAL_H
#define RECCALORIMETRY_INVSAMPLINGFRACTIONRADIAL_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "DetSegmentation/GridPhiEta.h"
class IGeoSvc;

// datamodel
namespace fcc {
class MCParticleCollection;
class PositionedCaloHitCollection;
}

class TH1F;
class ITHistSvc;

/** @class InvSamplingFractionRadial InvSamplingFractionRadial.h
 *
 *  @author Anna Zaborowska
 */

class InvSamplingFractionRadial: public GaudiAlgorithm {
public:
  explicit InvSamplingFractionRadial(const std::string&, ISvcLocator*);
  virtual ~InvSamplingFractionRadial();
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
  double m_energy;
  /// Handle for the true particles
  DataHandle<fcc::MCParticleCollection> m_MCparticles;
  /// Handle for the energy deposits
  DataHandle<fcc::PositionedCaloHitCollection> m_deposits;
  /// Pointer to the interface of histogram service
  SmartIF<ITHistSvc> m_histSvc;
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  std::vector<TH1F*> m_radialEnergy;
  TH1F* m_totalEnergy;
  std::vector<TH1F*> m_radialActiveEnergy;
  TH1F* m_totalActiveEnergy;
  std::vector<TH1F*> m_radialSF;
  TH1F* m_SF;
  /// Name of the detector readout
  std::string m_readoutName;
  /// PhiEta segmentation (owned by DD4hep)
  DD4hep::DDSegmentation::GridPhiEta* m_segmentation;
};
#endif /* RECCALORIMETRY_INVSAMPLINGFRACTIONRADIAL_H */
