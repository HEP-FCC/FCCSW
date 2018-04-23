#include "CorrectCluster.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"
#include "DetSegmentation/FCCSWGridPhiEta.h"

// DD4hep
#include "DD4hep/Detector.h"

// our EDM
#include "datamodel/CaloCluster.h"
#include "datamodel/CaloClusterCollection.h"
#include "datamodel/CaloHitCollection.h"

DECLARE_ALGORITHM_FACTORY(CorrectCluster)

CorrectCluster::CorrectCluster(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("clusters", m_inClusters, "Input clusters (input)");
  declareProperty("correctedClusters", m_correctedClusters, "Corrected clusters (output)");

  }

StatusCode CorrectCluster::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  return StatusCode::SUCCESS;
}

StatusCode CorrectCluster::execute() {
  // Get the input collection with clusters
  const fcc::CaloClusterCollection* inClusters = m_inClusters.get();
  fcc::CaloClusterCollection* correctedClusters = m_correctedClusters.createAndPut();

  for (const auto& cluster: *inClusters) {
    uint systemId = 5; // TODO fix that assumption, take sys Id from cells, but this requires new weights for other systems
    warning() << "cells-in-cluster SIZE: " << cluster.hits_size() << endmsg;
    double energy = 0;
    for(auto cell = cluster.hits_begin(); cell != cluster.hits_end(); cell++) {
          energy += cell->core().energy;
    }
    warning() << "cluster energy: " << cluster.core().energy << endmsg;
    warning() << "cells-in-cluster energy: " << energy << endmsg;

    // 0. Create new cluster, copy information from input
    fcc::CaloCluster newCluster = correctedClusters->create();
    newCluster.core().energy = cluster.core().energy + 20;
    newCluster.core().position.x = cluster.core().position.x;
    newCluster.core().position.y = cluster.core().position.y;
    newCluster.core().position.z = cluster.core().position.z;
    for(auto cell = cluster.hits_begin(); cell != cluster.hits_end(); cell++) {
      newCluster.addhits(*cell);
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode CorrectCluster::finalize() { return GaudiAlgorithm::finalize(); }
