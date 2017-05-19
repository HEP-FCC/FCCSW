#ifndef EXAMPLES_CREATESAMPLEJET_H
#define EXAMPLES_CREATESAMPLEJET_H

#include "FWCore/DataHandle.h"
#include "GaudiAlg/GaudiAlgorithm.h"

// forward declarations:
namespace fcc {
class JetCollection;
}

class CreateSampleJet : public GaudiAlgorithm {
  friend class AlgFactory<CreateSampleJet>;

public:
  /// Constructor.
  CreateSampleJet(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  StatusCode initialize() override;
  /// Execute.
  StatusCode execute() override;
  /// Finalize.
  StatusCode finalize() override;

private:
  /// Handle for the jets to be written
  DataHandle<fcc::JetCollection> m_jethandle{"podioJets", Gaudi::DataHandle::Writer, this};
};

#endif /* EXAMPLES_CREATESAMPLEJET_H */
