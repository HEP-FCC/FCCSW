#ifndef SIM_PAPASMERGECLUSTERS_H
#define SIM_PAPASMERGECLUSTERS_H

#include "FWCore/DataHandle.h"
#include "GaudiAlg/GaudiTool.h"
#include "SimPapas/IPapasTool.h"
// PAPAS
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
  /// Empty the class structures.
  virtual StatusCode clear();
  /// Execute papas tool
  virtual StatusCode run(papas::Event& pevent, std::shared_ptr<papas::Detector> spDetector) final;
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// collection in which the merged clusters will be stored
  papas::Clusters m_merged_clusters;
  /// Type and subtype of the clusters that are to be merged, eg "em" ecal merged
  Gaudi::Property<std::string> m_typeSubtype{this, "TypeAndSubtype", "", "type and subtype to merge"};
};

#endif  // SIM_PAPASMERGECLUSTERS_H
