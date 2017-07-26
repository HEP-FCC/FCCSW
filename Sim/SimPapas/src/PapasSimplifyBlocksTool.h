#ifndef SIM_PAPASSIMPLIFYBLOCKS_H
#define SIM_PAPASSIMPLIFYBLOCKS_H

#include "FWCore/DataHandle.h"
#include "GaudiAlg/GaudiTool.h"
#include "SimPapas/IPapasTool.h"
// PAPAS
#include "papas/datatypes/DefinitionsCollections.h"
#include "papas/reconstruction/PFBlock.h"
#include <string>

/** @class PapasSimplifyBlocksTool Sim/SimPapas/src/PapasSimplifyBlocksTool.h PapasSimplifyBlocksTool.h
 *
 *  Runs the papas block simplifying algorithm which takes existing particle flow blocks of tracks and clusters
 *  that are linked and tries to simplify them, eg by ensuring that each track is associated with at most
 *  one hcal.
 *  @author A.J. Robson
 */
class PapasSimplifyBlocksTool : public GaudiTool, virtual public IPapasTool {
public:
  /// Constructor.
  PapasSimplifyBlocksTool(const std::string& aType, const std::string& aName, const IInterface* aParent);
  /// Initialize.
  virtual StatusCode initialize();
  /// Execute papas tool.
  virtual StatusCode run(papas::Event& pevent, std::shared_ptr<papas::Detector> spDetector) final;
  /// empty class structures
  virtual StatusCode clear();
  /// Finalize.
  virtual StatusCode finalize();

private:
  /// collection of papas blocks that will be filled by this tool
  papas::Blocks m_blocks;
  /// Which subtype of block to use as the input, eg "r" for reconstructed
  Gaudi::Property<std::string> m_blockSubtype{this, "blockSubtype", "", "block subtype"};
};

#endif  // SIM_PAPASSIMPLIFYBLOCKS_H
