#ifndef SIM_PAPASMERGECLUSTERS_H
#define SIM_PAPASMERGECLUSTERS_H

#include "FWCore/DataHandle.h"
#include "GaudiAlg/GaudiTool.h"
#include "SimPapas/IPapasTool.h"

#include "papas/datatypes/DefinitionsCollections.h"

#include <string>

/** @class PapasMergeClustersTool Sim/SimPapas/src/PapasMergeClustersTool.h PapasMergeClustersTool.h
 *
 *  Runs the papas cluster merger and adds the new merged clusters into the papasevent
 *  @author A.J. Robson
 */

class PapasMergeClustersTool : public GaudiTool, virtual public IPapasTool {
public:
  /// Constructor.
  PapasMergeClustersTool(const std::string& aType, const std::string& aName, const IInterface* aParent);
  /// Initialize.
  virtual StatusCode initialize();
  /// empty class structures
  /// Structures
  StatusCode createOutputStructures() { return SUCCESS; };
  virtual StatusCode clear();
  /// Execute: Applies the filter
  virtual StatusCode run(papas::Event& pevent) final;
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// Handle for the genparticles to be written
  papas::Clusters m_merged_clusters;
  Gaudi::Property<std::string> m_typeSubtype{this, "TypeAndSubtype", "", "type and subtype to merge"};
};

#endif  // SIM_PAPASMERGECLUSTERS_H
