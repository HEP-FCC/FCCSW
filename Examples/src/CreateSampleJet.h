#ifndef EXAMPLES_CREATESAMPLEJET_H
#define EXAMPLES_CREATESAMPLEJET_H


#include "GaudiAlg/GaudiAlgorithm.h"
#include "datamodel/JetCollection.h"
#include "FWCore/DataHandle.h"

class CreateSampleJet: public GaudiAlgorithm {
  friend class AlgFactory<CreateSampleJet> ;

public:
  /// Constructor.
  CreateSampleJet(const std::string& name, ISvcLocator* svcLoc);
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

#endif /* EXAMPLES_CREATESAMPLEJET_H */
