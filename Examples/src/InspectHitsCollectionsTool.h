#ifndef TESTDD4HEP_INSPECTHITSCOLLECTIONSTOOL_H
#define TESTDD4HEP_INSPECTHITSCOLLECTIONSTOOL_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "SimG4Interface/ISimG4SaveOutputTool.h"
class IGeoSvc;

/** @class InspectHitsCollectionsTool TestDD4hep/TestDD4hep/InspectHitsCollectionsTool.h InspectHitsCollectionsTool.h
 *
 *  Tool used to inspect the hits collection.
 *  No output in EDM is produced.
 *
 *  @author Anna Zaborowska
 */

class InspectHitsCollectionsTool : public GaudiTool, virtual public ISimG4SaveOutputTool {
public:
  /// Constructor.
  explicit InspectHitsCollectionsTool(const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~InspectHitsCollectionsTool();
  /// Initialize.
  StatusCode initialize() override final;
  /// Finalize.
  StatusCode finalize() override final;
  /// inspect output
  StatusCode saveOutput(const G4Event& aEvent) override final;

private:
  /// Pointer to the geometry service
  SmartIF<IGeoSvc> m_geoSvc;
  /// Name of the readouts (hits collections)
  Gaudi::Property<std::vector<std::string>> m_readoutNames{
      this, "readoutNames", {}, "Names of the readouts (hits collections)"};
};

#endif /* TESTDD4HEP_INSPECTHITSCOLLECTIONSTOOL_H */
