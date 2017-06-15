#ifndef PILEUPMERGE_H
#define PILEUPMERGE_H

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"
#include "FWCore/DataHandle.h"
#include "FWCore/DataHandle.h"
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


class PileupMerge: public GaudiAlgorithm {
public:
  explicit PileupMerge(const std::string&, ISvcLocator*);
  virtual ~PileupMerge() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;
  virtual StatusCode finalize() override final;
private:
  

  Gaudi::Property<std::string> m_pileupGenParticlesBranchName{this, "genParticleBranch", "genParticles", ""}; 
  Gaudi::Property<std::string> m_pileupGenVerticesBranchName{this, "genVerticesBranch", "genVertices", ""}; 
  Gaudi::Property<std::string> m_pileupTrackHitsBranchName{this, "trackHitsBranch", "trackHits", ""}; 
  Gaudi::Property<std::string> m_pileupPositionedTrackHitsBranchName{this, "posTrackHitsBranch", "positionedTrackHits", ""}; 
  Gaudi::Property<std::string> m_pileupCaloHitsBranchName{this, "caloHitsBranch", "caloHits", ""}; 
  Gaudi::Property<std::string> m_pileupPositionedCaloHitsBranchName{this, "posCaloHitsBranch", "positionedCaloHits", ""}; 

  DataHandle<fcc::MCParticleCollection> m_genParticlesOut{"pileupGenParticles", Gaudi::DataHandle::Writer, this};
  DataHandle<fcc::GenVertexCollection> m_genVerticesOut{"pileupGenVertices", Gaudi::DataHandle::Writer, this};

  DataHandle<fcc::TrackHitCollection> m_trackHitsOut{"pileupTrackHits", Gaudi::DataHandle::Writer, this};
  DataHandle<fcc::PositionedTrackHitCollection> m_posTrackHits{"pileupPosTrackHits", Gaudi::DataHandle::Writer, this};

  DataHandle<fcc::CaloHitCollection> m_caloHitsOut{"pileupCaloHits", Gaudi::DataHandle::Writer, this};
  DataHandle<fcc::PositionedCaloHitCollection> m_posCaloHits{"pileupPosCaloHits", Gaudi::DataHandle::Writer, this};


  Gaudi::Property<std::string> m_pileupFilename{this, "pileupFilename", "min-bias-pool.root", "Name of File containing pileup events"}; 
  Gaudi::Property<bool> m_randomizePileup{this, "randomizePileup", false, "flag to determine wether to randomly skip events when reading the pileup store"}; 
  ToolHandle<IPileUpTool> m_pileUpTool{"ConstPileUp/PileUpTool", this};

  podio::EventStore m_store;
  podio::ROOTReader m_reader;

  Rndm::Numbers m_flatDist;

  unsigned int currentEvent;

};

#endif /* PILEUPMERGE_H */
