#ifndef _JETHISTOGRAMS_H_
#define _JETHISTOGRAMS_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ITHistSvc.h"
#include "DataObjects/PseudoJetEntry.h"

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
  DataObjectHandle<PseudoJetEntry> m_jethandle;

  ITHistSvc* m_ths; ///< THistogram service

  TH1F* m_E; ///< histogram for energy of jets
  TH1F* m_n; ///< histogram for number of jets

};

#endif
