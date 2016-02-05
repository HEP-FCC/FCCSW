#ifndef _PODIOWRITE_H_
#define _PODIOWRITE_H_

#include "GaudiAlg/GaudiAlgorithm.h"
#include "datamodel/JetCollection.h"
#include "FWCore/DataHandle.h"

class PodioWrite: public GaudiAlgorithm {
  friend class AlgFactory<PodioWrite> ;

public:
  /// Constructor.
  PodioWrite(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute.
  virtual StatusCode execute();
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// Handle for the jets to be written
  DataHandle<fcc::JetCollection> m_jethandle;

};

#endif
