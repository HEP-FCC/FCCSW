#ifndef FWCORE_PILEUPDIGITRACKERHITSMERGETOOL_H
#define FWCORE_PILEUPDIGITRACKERHITSMERGETOOL_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "FWCore/IEDMMergeTool.h"

namespace fcc {
class PositionedTrackHit;
class PositionedTrackHitCollection;
class DigiTrackHitAssociation;
class DigiTrackHitAssociationCollection;
class MCParticle;
class MCParticleCollection;
}

/** @class PileupDigiTrackHitMergeTool
 *
 * Implemenation of the MergeTool for Digitization hits and, if requested, simulated hits.
 * While merging, this algorithm tries to keep the trackIDs unique by adding the pileup event number with an offset.
 * This should be transparent, but the trackIDs will be non-consecutive.
 *
 */

class PileupDigiTrackHitMergeTool : public GaudiTool, virtual public IEDMMergeTool {
public:
  explicit PileupDigiTrackHitMergeTool(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~PileupDigiTrackHitMergeTool() = default;

  virtual StatusCode initialize() override final;

  virtual StatusCode finalize() override final;

  /// fill the member containers of collection pointers in this class with a collection from the event store
  virtual StatusCode readPileupCollection(podio::EventStore& store) override final;

  /// merge the collections in the member container and put them on  the event store
  virtual StatusCode mergeCollections() override final;

  /// fill the member container of collection pointer with a collection from the event store
  virtual StatusCode readSignal() override final;

private:
  /// Name of the branch from which to read pileup collection
  Gaudi::Property<std::string> m_pileupHitsBranchName{this, "pileupHitsBranch", "positionedHits",
                                                      "Name of the branch from which to read pileup collection"};
  /// Name of the branch from which to read pileup collection
  Gaudi::Property<std::string> m_pileupPosHitsBranchName{this, "pileupDigiHitsBranch", "digiHits",
                                                         "Name of the branch from which to read pileup collection"};

  /// Name of the branch from which to read pileup collection
  Gaudi::Property<std::string> m_pileupParticleBranchName{this, "pileupParticleBranch", "simParticles",
                                                          "Name of the branch from which to read pileup collection"};

  /// Flag indicating if particles are present and should be merged
  Gaudi::Property<bool> m_mergeParticles{this, "mergeParticles", false,
                                         "Flag indicating if particles are present and should be merged"};

  /// internal container to keep of collections to be merged
  std::vector<const fcc::PositionedTrackHitCollection*> m_hitCollections;
  /// internal container to keep of collections to be merged
  std::vector<const fcc::DigiTrackHitAssociationCollection*> m_digiTrackHitsCollections;
  /// internal container to keep of collections to be merge
  std::vector<const fcc::MCParticleCollection*> m_particleCollections;

  /// Output of this tool: merged collection
  DataHandle<fcc::PositionedTrackHitCollection> m_hitsMerged{"overlay/hits", Gaudi::DataHandle::Writer, this};
  /// Output of this tool: merged collection
  DataHandle<fcc::DigiTrackHitAssociationCollection> m_digiHitsMerged{"overlay/digiHits", Gaudi::DataHandle::Writer,
                                                                      this};
  /// Output of this tool: merged collection
  DataHandle<fcc::MCParticleCollection> m_particlesMerged{"overlay/particles", Gaudi::DataHandle::Writer, this};

  /// input to this tool: signal collection
  DataHandle<fcc::PositionedTrackHitCollection> m_hitsSignal{"overlay/signalHits", Gaudi::DataHandle::Reader, this};
  /// input to this tool: signal collection
  DataHandle<fcc::DigiTrackHitAssociationCollection> m_posHitsSignal{"overlay/signalDigiHits",
                                                                     Gaudi::DataHandle::Reader, this};
  /// input to this tool: signal collection
  DataHandle<fcc::MCParticleCollection> m_particlesSignal{"overlay/signalParticles", Gaudi::DataHandle::Reader, this};

  /// offset with which the pileup event number is added to the trackID
  const unsigned int m_trackIDCollectionOffset = 2.5e6;

  /// Mask to obtain system ID
  const unsigned long long m_systemMask = 0xf;

  /// Private function internally used to check if a hit is a tracker hit
  bool isTrackerHit(unsigned long long cellId) const;
};

inline bool PileupDigiTrackHitMergeTool::isTrackerHit(unsigned long long cellId) const {
  return ((cellId & m_systemMask) < 5);
}

#endif  // FWCORE_PILEUPDIGITRACKERHITSMERGETOOL_H
