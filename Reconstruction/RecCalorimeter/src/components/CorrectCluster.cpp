#include "CorrectCluster.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetInterface/IGeoSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "DetSegmentation/FCCSWGridPhiEta.h"

// DD4hep
#include "DD4hep/Detector.h"

// our EDM
#include "datamodel/CaloCluster.h"
#include "datamodel/CaloClusterCollection.h"
#include "datamodel/CaloHitCollection.h"

DECLARE_ALGORITHM_FACTORY(CorrectCluster)

CorrectCluster::CorrectCluster(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc),
  m_histSvc("THistSvc", "CorrectCluster"),
  m_geoSvc("GeoSvc", "CorrectCluster") {
  declareProperty("clusters", m_inClusters, "Input clusters (input)");
  declareProperty("correctedClusters", m_correctedClusters, "Corrected clusters (output)");

}

StatusCode CorrectCluster::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  // create control histograms
  m_hEnergyPreAnyCorrections = new TH1F("energyPreAnyCorrections", "Energy of cluster before any correction", 1000, 0.8 * m_energy, 1.2 * m_energy);
  if (m_histSvc->regHist("/rec/energyPreAnyCorrections", m_hEnergyPreAnyCorrections).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  m_hEnergyPostAllCorrections = new TH1F("energyPostAllCorrections", "Energy of cluster after all corrections", 1000, 0.8 * m_energy, 1.2 * m_energy);
  if (m_histSvc->regHist("/rec/energyPostAllCorrections", m_hEnergyPostAllCorrections).isFailure()) {
    error() << "Couldn't register histogram" << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_etaRecalcLayerWeights.size() < m_numLayers) {
    error() << "m_etaRecalcLayerWeights size and numLayers are different." << endmsg;
    return StatusCode::FAILURE;
  }
  for(uint iSys = 0; iSys < m_systemId.size(); iSys++) {
    // check if readouts exist
    if (m_geoSvc->lcdd()->readouts().find(m_readoutName[iSys]) == m_geoSvc->lcdd()->readouts().end()) {
      error() << "Readout <<" << m_readoutName[iSys] << ">> does not exist." << endmsg;
      return StatusCode::FAILURE;
    }
    // retrieve PhiEta segmentation
    m_segmentation[m_systemId[iSys]] = dynamic_cast<dd4hep::DDSegmentation::FCCSWGridPhiEta*>(
      m_geoSvc->lcdd()->readout(m_readoutName[iSys]).segmentation().segmentation());
    if (m_segmentation[m_systemId[iSys]] == nullptr) {
      error() << "There is no phi-eta segmentation." << endmsg;
      return StatusCode::FAILURE;
    }
    m_decoder[m_systemId[iSys]] = m_geoSvc->lcdd()->readout(m_readoutName[iSys]).idSpec().decoder();
  }
  return StatusCode::SUCCESS;
}

StatusCode CorrectCluster::execute() {
  // Get the input collection with clusters
  const fcc::CaloClusterCollection* inClusters = m_inClusters.get();
  fcc::CaloClusterCollection* correctedClusters = m_correctedClusters.createAndPut();

  for (const auto& cluster: *inClusters) {
    warning() << "cells-in-cluster SIZE: " << cluster.hits_size() << endmsg;
    double energy = 0;
    TVector3 pos (cluster.core().position.x, cluster.core().position.y, cluster.core().position.z);
    double oldEta = pos.Eta();
    for(auto cell = cluster.hits_begin(); cell != cluster.hits_end(); cell++) {
      energy += cell->core().energy;
    }
    warning() << "cluster energy: " << cluster.core().energy << endmsg;
    warning() << "cells-in-cluster energy: " << energy << endmsg;

    // 0. Create new cluster, copy information from input
    fcc::CaloCluster newCluster = correctedClusters->create();
    newCluster.core().energy = cluster.core().energy;
    newCluster.core().position.x = cluster.core().position.x;
    newCluster.core().position.y = cluster.core().position.y;
    newCluster.core().position.z = cluster.core().position.z;
    for(auto cell = cluster.hits_begin(); cell != cluster.hits_end(); cell++) {
      newCluster.addhits(*cell);
    }

    // 1. Correct eta position with log-weighting
    double sumEnFirstLayer = 0;
    uint systemId = 5; // Eta position calculated from ECal barrel
    // get current pseudorapidity
    double newEta = 0;
    std::vector<double> sumEnLayer;
    std::vector<double> sumEtaLayer;
    std::vector<double> sumWeightLayer;
    sumEnLayer.assign(m_numLayers, 0);
    sumEtaLayer.assign(m_numLayers, 0);
    sumWeightLayer.assign(m_numLayers, 0);
    // first check the energy deposited in each layer
    for(auto cell = cluster.hits_begin(); cell != cluster.hits_end(); cell++) {
      m_decoder[systemId]->setValue(cell->core().cellId);
      uint layer = (*m_decoder[systemId])[m_layerFieldName] + m_firstLayerId;
      debug() << layer << endmsg;
      sumEnLayer[layer] += cell->core().energy;
    }
    sumEnFirstLayer = sumEnLayer[0];
    // repeat but calculating eta barycentre in each layer
    for(auto cell = cluster.hits_begin(); cell != cluster.hits_end(); cell++) {
      m_decoder[systemId]->setValue(cell->core().cellId);
      uint layer = (*m_decoder[systemId])[m_layerFieldName] + m_firstLayerId;
      double weightLog = std::max(0., m_etaRecalcLayerWeights[layer] + log(cell->core().energy / sumEnLayer[layer]));
      double eta = m_segmentation[systemId]->eta(cell->core().cellId);
      sumEtaLayer[layer] += (weightLog * eta);
      sumWeightLayer[layer] += weightLog;
    }
    // calculate eta position weighting with energy deposited in layer
    // this energy is a good estimator of 1/sigma^2 of (eta_barycentre-eta_MC) distribution
    for (uint iLayer = 0; iLayer < m_numLayers; iLayer++) {
      if(sumWeightLayer[iLayer] != 0) {
        sumEtaLayer[iLayer] /= sumWeightLayer[iLayer];
      }
      newEta += sumEtaLayer[iLayer] * sumEnLayer[iLayer];
    }
    newEta /= std::accumulate(sumEnLayer.begin(), sumEnLayer.end(), 0);
    warning() << " old eta = " << oldEta << " new eta = " << newEta << endmsg;
    // alter Cartesian position of a cluster using new eta position
    double radius = pos.Perp();
    double phi = pos.Phi();
    newCluster.core().position.x = radius * cos(phi);
    newCluster.core().position.y = radius * sin(phi);
    newCluster.core().position.z = radius * sinh(newEta);

    // Fill histograms
    m_hEnergyPreAnyCorrections->Fill(cluster.core().energy);
    m_hEnergyPostAllCorrections->Fill(newCluster.core().energy);
  }
  return StatusCode::SUCCESS;
}

StatusCode CorrectCluster::finalize() { return GaudiAlgorithm::finalize(); }
