#ifndef PILEUPMERGE_H
#define PILEUPMERGE_H

#include "FWCore/IPileUpTool.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "FWCore/DataHandle.h"
#include "FWCore/IEDMMergeTool.h"

#include "datamodel/TrackHitCollection.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/MCParticleCollection.h"
#include "datamodel/GenVertexCollection.h"

#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"

#include "podio/EventStore.h"
#include "podio/ROOTReader.h"


namespace fcc {
  class TrackClusterCollection;
  class TrackClusterHitsAssociationCollection;
}


class PileupOverlayAlg: public GaudiAlgorithm {
public:
  explicit PileupOverlayAlg(const std::string&, ISvcLocator*);
  virtual ~PileupOverlayAlg() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;
private:

  Gaudi::Property<std::string> m_pileupFilename{this, "pileupFilename", "min_bias_pool.root", "Name of File containing pileup events"}; 
  Gaudi::Property<bool> m_randomizePileup{this, "randomizePileup", false, "flag to determine wether to randomly skip events when reading the pileup store"}; 
  ToolHandle<IPileUpTool> m_pileUpTool{"ConstPileUp/PileUpTool", this};

  /// to be replaced with ToolHandleArray
  std::vector<IEDMMergeTool*> m_mergeTools;
  Gaudi::Property<std::vector<std::string>> m_saveToolNames{this, "mergeTools", {}, "Names for the saving tools"};

  podio::EventStore m_store;
  podio::ROOTReader m_reader;

  Rndm::Numbers m_flatDist;

  unsigned int m_minBiasEventIndex;

};

#endif /* PILEUPMERGE_H */
