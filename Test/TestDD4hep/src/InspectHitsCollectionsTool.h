#ifndef TESTDD4HEP_INSPECTHITSCOLLECTIONSTOOL_H
#define TESTDD4HEP_INSPECTHITSCOLLECTIONSTOOL_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

//FCCSW
#include "SimG4Interface/IG4SaveOutputTool.h"

class InspectHitsCollectionsTool: public GaudiTool, virtual public IG4SaveOutputTool {
public:
  /// Constructor.
  explicit InspectHitsCollectionsTool(const std::string& type, const std::string& name, const IInterface* parent);
  ~InspectHitsCollectionsTool();
  /// Initialize.
  virtual StatusCode initialize() final;
  /// Finalize.
  virtual StatusCode finalize() final;
  /// inspect output
  virtual StatusCode saveOutput(const G4Event& aEvent) final;
};

#endif /* TESTDD4HEP_INSPECTHITSCOLLECTIONSTOOL_H */
