#ifndef SIM_PAPASBLOCKBUILDER_H
#define SIM_PAPASBLOCKBUILDER_H

// gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "SimPapas/IPapasTool.h"

// papas
#include "papas/datatypes/DefinitionsCollections.h"
#include "papas/detectors/Detector.h"
namespace papas{
  class Detector;
}
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
  virtual StatusCode initialize();
  /// Create any output structures needed.
  StatusCode createOutputStructures() { return SUCCESS; };
  /// Execute papas tool
  virtual StatusCode run(papas::Event& pevent, std::shared_ptr<papas::Detector> det) final;
  /// Empty the class structures.
  virtual StatusCode clear();
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// subtype for ecal clusters that are to be used in blockbuilding eg 'm' for merged
  Gaudi::Property<std::string> m_ecalSubtype{this, "ecalSubtype", "", "ecal subtype"};
  /// subtype for hcal clusters that are to be used in blockbuilding eg 'm' for merged
  Gaudi::Property<std::string> m_hcalSubtype{this, "hcalSubtype", "", "hcal subtype"};
  /// subtype for tracks that are to be used in blockbuilding eg 's' for smeared
  Gaudi::Property<std::string> m_trackSubtype{this, "trackSubtype", "", "track subtype"};
  /// collection to store the blocks that are built by this tool
  papas::Blocks m_blocks;
   /// pointer to papas detector
   //Gaudi::Property<papas::Detector> m_detector{this, "detector", 0, "papas detector"};
   //std::shared_ptr<papas::Detector> m_spDetector;
};

#endif  // SIM_PAPASBLOCKBUILDER_H
