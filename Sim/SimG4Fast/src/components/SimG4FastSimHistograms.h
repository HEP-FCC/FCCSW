#ifndef SIMG4FAST_G4FASTSIMHISTOGRAMS_H
#define SIMG4FAST_G4FASTSIMHISTOGRAMS_H

// GAUDI
#include "GaudiAlg/GaudiAlgorithm.h"

// FCCSW
#include "FWCore/DataHandle.h"
class ITHistSvc;

// datamodel
namespace fcc {
class ParticleCollection;
class ParticleMCParticleAssociationCollection;
}

class TH1F;

/** @class SimG4FastSimHistograms SimG4Components/src/SimG4FastSimHistograms.h SimG4FastSimHistograms.h
 *
 *  Fast simulation histograms algorithm.
 *  Fills validation histograms.
 *  It takes ParticleCollection (\b'smearedParticles') as the input.
 *
 *  @author Anna Zaborowska
 */

class SimG4FastSimHistograms: public GaudiAlgorithm {
public:
  explicit SimG4FastSimHistograms(const std::string&, ISvcLocator*);
  virtual ~SimG4FastSimHistograms();
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
  /// Handle for the EDM MC particles to be read
  DataHandle<fcc::ParticleCollection> m_smParticles;
  /// Handle for the EDM particles and MC particles associations to be read
  DataHandle<fcc::ParticleMCParticleAssociationCollection> m_particlesMCparticles;
  /// Pointer to the interface of histogram service
  SmartIF<ITHistSvc> m_histSvc;
  // Histogram of the smeared particle's momentum
  TH1F* m_p;
  // Histogram of the smeared particle's pseudorapidity
  TH1F* m_eta;
  // Histogram of the difference between MC particle's and smeared particle's momentum
  TH1F* m_diffP;
  // Histogram of the smeared particle's PDG code
  TH1F* m_pdg;
};
#endif /* SIMG4FAST_G4FASTSIMHISTOGRAMS_H */
