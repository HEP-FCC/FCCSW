#ifndef RECCALORIMETER_JETHISTOGRAMS_H
#define RECCALORIMETER_JETHISTOGRAMS_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "datamodel/JetCollection.h"

#include "FWCore/DataHandle.h"
#include "TH1F.h"

class JetHistograms: public GaudiAlgorithm {
  friend class AlgFactory<JetHistograms> ;

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
  DataHandle<fcc::JetCollection> m_jethandle;

  ITHistSvc* m_ths; ///< THistogram service

  TH1F* m_E; ///< histogram for energy of jets
  TH1F* m_n; ///< histogram for number of jets

};

#endif
