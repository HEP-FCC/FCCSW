#ifndef RECCALORIMETER_CORRECTCLUSTER_H
#define RECCALORIMETER_CORRECTCLUSTER_H

// FCCSW
#include "FWCore/DataHandle.h"
class IGeoSvc;
class ITHistSvc;

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

namespace fcc {
 class CaloClusterCollection;
 class CaloHitCollection;
}

namespace dd4hep {
namespace DDSegmentation {
class FCCSWGridPhiEta;
class BitField64;
}
}

#include "TH1F.h"

/** @class CorrectCluster
 *
 *  Apply corrections to a reconstructed cluster
 *
 *  Possible corrections:
 *  ...
 *
 *  @author Anna Zaborowska
 *
 */

class CorrectCluster : public GaudiAlgorithm {

public:
  CorrectCluster(const std::string& name, ISvcLocator* svcLoc);

  StatusCode initialize();

  StatusCode execute();

  StatusCode finalize();

private:
  /// Handle for clusters (input collection)
  DataHandle<fcc::CaloClusterCollection> m_inClusters{"clusters", Gaudi::DataHandle::Reader, this};
  /// Handle for corrected clusters (output collection)
  DataHandle<fcc::CaloClusterCollection> m_correctedClusters{"correctedClusters", Gaudi::DataHandle::Writer, this};
};

#endif /* RECCALORIMETER_CORRECTCLUSTER_H */
