#ifndef FWCORE_PILEUPOVERLAYALG_H
#define FWCORE_PILEUPOVERLAYALG_H

#include "FWCore/DataHandle.h"
#include "FWCore/IEDMMergeTool.h"
#include "FWCore/IPileUpTool.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"

#include "podio/EventStore.h"
#include "podio/ROOTReader.h"

// forward declarations
class IEDMMergeTool;

/*** @class PileupOverlayAlg
 *  Algorithm for Pileup I/O and merging
 *
 * The main purpose of this class is to keep a list of merge tools corresponding to different
 * datatypes, open a file and let the tools read pileup data and merge it with the signal.
 * It is thus flexible with regards to the data collection types, as everything is delegated to
 * tools with a very general interface. One thing this algorithm does do is keep track of the
 * number of pileup events (via the pileup tool) and the current position in the minimum bias pool
 * ( which can be randomized by skipping events with a probability that can be set in the options file ).
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
  /// probability of skipping  next event in randomized read
  Gaudi::Property<float> m_skipProbability{this, "skipProbability", 0.5, "Probability with which to skip next event"};
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
  Gaudi::Property<bool> m_doShuffleInputFiles{this, "doShuffleInputFiles", false, "Shuffle list of input files for additional randomization"};
  /// store for the minimum bias file
  podio::EventStore m_store;
  /// reader for the minimum bias file
  podio::ROOTReader m_reader;

  /// event index within current minimum bias pool
  unsigned int m_minBiasEventIndex;
  /// index of the current minimum bias pool
  unsigned int m_pileupFileIndex;
};

#endif /* FWCORE_PILEUPOVERLAYALG_H */
