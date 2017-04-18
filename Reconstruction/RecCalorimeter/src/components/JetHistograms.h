#ifndef RECO_JETHISTOGRAMS_H
#define RECO_JETHISTOGRAMS_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "datamodel/JetCollection.h"

#include "FWCore/DataHandle.h"
#include "TH1F.h"

class JetHistograms : public GaudiAlgorithm {
  friend class AlgFactory<JetHistograms>;

public:
  /// Constructor.
  JetHistograms(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// Handle for the HepMC to be read
  DataHandle<fcc::JetCollection> m_jethandle{"jets", Gaudi::DataHandle::Reader, this};

  ITHistSvc* m_ths{nullptr};  ///< THistogram service

  TH1F* m_E{nullptr};  ///< histogram for energy of jets
  TH1F* m_n{nullptr};  ///< histogram for number of jets
};

#endif
