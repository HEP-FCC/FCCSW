#ifndef SIM_PAPASBLOCKBUILDER_H
#define SIM_PAPASBLOCKBUILDER_H

// gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimPapas/IPapasTool.h"

// papas
#include "papas/datatypes/DefinitionsCollections.h"
#include <string>

/** @class PapasBuildBlocksTool Sim/SimPapas/src/PapasBuildBlocksTool.h PapasBuildBlocksTool.h
 *
 *  Runs the papas block building algorithm which creates particle flow blocks out of tracks and clusters
 *  that are linked (and from which reconstructed particles will be built).
 *  @author A.J. Robson
 */

class PapasBuildBlocksTool : public GaudiTool, virtual public IPapasTool {
public:
  /// Constructor.
  PapasBuildBlocksTool(const std::string& aType, const std::string& aName, const IInterface* aParent);
  /// Initialize.
  virtual StatusCode initialize();
  /// Structures
  StatusCode createOutputStructures() { return SUCCESS; };
  /// Execute:
  virtual StatusCode run(papas::Event& pevent) final;
  /// empty class structures
  virtual StatusCode clear();
  /// Finalize.
  virtual StatusCode finalize();

private:
  Gaudi::Property<std::string> m_ecalSubtype{this, "ecalSubtype", "", "ecal subtype"};
  Gaudi::Property<std::string> m_hcalSubtype{this, "hcalSubtype", "", "hcal subtype"};
  Gaudi::Property<std::string> m_trackSubtype{this, "trackSubtype", "", "track subtype"};
  papas::Blocks m_blocks;
};

#endif  // SIM_PAPASBLOCKBUILDER_H
