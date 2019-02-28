#ifndef FWCORE_PILEUPTRACKERHITSMERGETOOL_H
#define FWCORE_PILEUPTRACKERHITSMERGETOOL_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "FWCore/IEDMMergeTool.h"

namespace fcc {
class HitCollection;
class PositionedHitCollection;
}

/** @class PileupHitMergeTool
 *
 * Implemenation of the MergeTool for *Hits and *PositionedHits, templated to give versions for Tracker / Calorimeter
 * While merging, this algorithm tries to keep the trackIDs unique by adding the pileup event number with an offset.
 * This should be transparent, but the trackIDs will be non-consecutive.
 *
 *
 */

template <class Hits, class PositionedHits>
class PileupHitMergeTool : public GaudiTool, virtual public IEDMMergeTool {
public:
  explicit PileupHitMergeTool(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~PileupHitMergeTool() = default;

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
  Gaudi::Property<std::string> m_pileupHitsBranchName{this, "pileupHitsBranch", "hits",
                                                      "Name of the branch from which to read pileup collection"};
  /// Name of the branch from which to read pileup collection
  Gaudi::Property<std::string> m_pileupPosHitsBranchName{this, "pileupPositionedHitsBranch", "positionedHits",
                                                         "Name of the branch from which to read pileup collection"};

  /// internal container to keep of collections to be merged
  std::vector<const Hits*> m_hitCollections;
  /// internal container to keep of collections to be merged
  std::vector<const PositionedHits*> m_posHitCollections;

  /// Output of this tool: merged collection
  DataHandle<Hits> m_hitsMerged{"overlay/hits", Gaudi::DataHandle::Writer, this};
  /// Output of this tool: merged collection
  DataHandle<PositionedHits> m_posHitsMerged{"overlay/positionedHits", Gaudi::DataHandle::Writer, this};

  /// input to this tool: signal collection
  DataHandle<Hits> m_hitsSignal{"overlay/signalHits", Gaudi::DataHandle::Reader, this};
  /// input to this tool: signal collection
  DataHandle<PositionedHits> m_posHitsSignal{"overlay/signalPositionedHits", Gaudi::DataHandle::Reader, this};

  /// offset with which the pileup event number is added to the trackID 
  const unsigned int m_trackIDCollectionOffset = 2 << 20;
};

#endif  // FWCORE_PILEUPTRACKERHITSMERGETOOL_H
