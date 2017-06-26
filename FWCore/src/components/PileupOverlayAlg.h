#ifndef FWCORE_PILEUPOVERLAYALG_H
#define FWCORE_PILEUPOVERLAYALG_H

#include "FWCore/DataHandle.h"
#include "FWCore/IEDMMergeTool.h"
#include "FWCore/IPileUpTool.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"

// forward declarations
class IEDMMergeTool;

namespace podio {
class EventStore;
class ROOTReader;
}

/*** @class PileupOverlayAlg
 *  Algorithm for Pileup I/O and merging
 *
 * The main purpose of this class is to keep a list of merge tools corresponding to different
 * datatypes, open a file and let the tools read pileup data and merge it with the signal.
 * It is thus flexible with regards to the data collection types, as everything is delegated to
 * tools with a very general interface. One thing this algorithm does do is keep track of the
 * number of pileup events (via the pileup tool) and the current position in the minimum bias pool
 * ( which can be randomized ).
 *
 */

class PileupOverlayAlg : public GaudiAlgorithm {
public:
  explicit PileupOverlayAlg(const std::string&, ISvcLocator*);
  virtual ~PileupOverlayAlg() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;

private:
  /// flag to determine whether to randomly skipy events when reading the pileup store
  Gaudi::Property<bool> m_randomizePileup{
      this, "randomizePileup", false,
      "flag to determine whether to randomly skip events when reading the pileup store"};
  /// random number generator for randomizing pileup reads
  Rndm::Numbers m_flatDist;
  /// Pileup Tool to specify the number of minimum bias events
  ToolHandle<IPileUpTool> m_pileUpTool{"ConstPileUp/PileUpTool", this};

  /// vector of tool interface pointers, to be replaced with ToolHandleArray
  std::vector<IEDMMergeTool*> m_mergeTools;
  /// names of the merge tools
  Gaudi::Property<std::vector<std::string>> m_saveToolNames{this, "mergeTools", {}, "names of the merge tools"};

  // list of minimum bias data readers / stores
  // once the root reader supports multiple files
  // this algo should take advantage of it
  /// list of filenames for the minimum bias pools
  Gaudi::Property<std::vector<std::string>> m_pileupFilenames{
      this, "pileupFilenames", {"min_bias_pool.root"}, "Name of File containing pileup events"};
  /// list of stores for each minimum bias file
  std::vector<podio::EventStore> m_stores;
  /// list of readers for each minimum bias file
  std::vector<podio::ROOTReader> m_readers;

  /// event index within current minimum bias pool
  unsigned int m_minBiasEventIndex;
  /// index of the current minimum bias pool
  unsigned int m_pileupFileIndex;
};

#endif /* FWCORE_PILEUPOVERLAYALG_H */
