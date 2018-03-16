#ifndef FWCORE_PILEUPCLUSTERMERGETOOL_H
#define FWCORE_PILEUPCLUSTERMERGETOOL_H

// Gaudi
#include "GaudiAlg/GaudiTool.h"

// FCCSW
#include "FWCore/DataHandle.h"
#include "FWCore/IEDMMergeTool.h"

namespace fcc {
class ClusterCollection;
}

/** @class PileupClusterMergeTool
 *
 * Implemenation of the MergeTool for *Clusters, templated to give versions for Tracker / Calorimeter
 * While merging, this algorithm tries to keep the trackIDs unique by adding the pileup event number with an offset.
 * This should be transparent, but the trackIDs will be non-consecutive.
 *
 *
 */

template <class Clusters>
class PileupClusterMergeTool : public GaudiTool, virtual public IEDMMergeTool {
public:
  explicit PileupClusterMergeTool(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~PileupClusterMergeTool() = default;

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
  Gaudi::Property<std::string> m_pileupClustersBranchName{this, "pileupClustersBranch", "hits",
                                                      "Name of the branch from which to read pileup collection"};

  /// internal container to keep of collections to be merged
  std::vector<const Clusters*> m_hitCollections;

  /// Output of this tool: merged collection
  DataHandle<Clusters> m_hitsMerged{"overlay/clusters", Gaudi::DataHandle::Writer, this};

  /// input to this tool: signal collection
  DataHandle<Clusters> m_hitsSignal{"overlay/signalClusters", Gaudi::DataHandle::Reader, this};

  /// offset with which the pileup event number is added to the trackID 
  const unsigned int m_trackIDCollectionOffset = 2 << 20;
};

#endif  // FWCORE_PILEUPCLUSTERMERGETOOL_H
