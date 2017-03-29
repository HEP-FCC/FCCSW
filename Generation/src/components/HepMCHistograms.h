#ifndef GENERATION_HEPMCHISTOGRAMS_H
#define GENERATION_HEPMCHISTOGRAMS_H

#include "FWCore/DataHandle.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "HepMC/GenEvent.h"

#include "TH1F.h"

class HepMCHistograms : public GaudiAlgorithm {
  friend class AlgFactory<HepMCHistograms>;

public:
  /// Constructor.
  HepMCHistograms(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// Handle for the HepMC to be read
  DataHandle<HepMC::GenEvent> m_hepmchandle{"HepMC", Gaudi::DataHandle::Reader, this};

  ITHistSvc* m_ths{nullptr};  ///< THistogram service

  TH1F* m_pt{nullptr};   ///< histogram for pT of particles
  TH1F* m_eta{nullptr};  ///< histogram for pseudorapidity of particles

  TH1F* m_d0{nullptr};  ///< histogram for transversal IP
  TH1F* m_z0{nullptr};  ///< histogram for longidudinal IP
};

#endif  // GENERATION_HEPMCHISTOGRAMS_H
