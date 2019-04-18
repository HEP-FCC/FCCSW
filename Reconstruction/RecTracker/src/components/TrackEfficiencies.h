#ifndef RECTRACKER_TRACKEFFICIENCIES
#define RECTRACKER_TRACKEFFICIENCIES

#include "GaudiAlg/GaudiAlgorithm.h"

#include "FWCore/DataHandle.h"

class ITHistSvc;

namespace fcc {
  class ParticleCollection;
  class MCParticleCollection;
}

class TH1F;
class TH2F;

/** 
 *
 *  Fills performance histograms.
 *
 */

class TrackEfficiencies : public GaudiAlgorithm {
public:
  // standard GaudiAlgorithm methods
  explicit TrackEfficiencies(const std::string&, ISvcLocator*);
  virtual ~TrackEfficiencies();
  virtual StatusCode initialize() final;
  virtual StatusCode execute() final;
  virtual StatusCode finalize() final;

private:
  DataHandle<fcc::MCParticleCollection> m_simParticles{"SimParticles", Gaudi::DataHandle::Reader, this};
  DataHandle<fcc::ParticleCollection> m_recParticles{"RecParticles", Gaudi::DataHandle::Reader, this};

  /// Pointer to the interface of histogram service
  SmartIF<ITHistSvc> m_histSvc;

  TH2F* m_pt{nullptr};
  TH2F* m_eta{nullptr};
  TH2F* m_cosTheta{nullptr};
};
#endif /* RECTRACKER_TRACKEFFICIENCIES */
