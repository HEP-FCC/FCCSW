#include "CreateCaloClustersSlidingWindow.h"

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// datamodel
#include "datamodel/CaloClusterCollection.h"

DECLARE_ALGORITHM_FACTORY(CreateCaloClustersSlidingWindow)

CreateCaloClustersSlidingWindow::CreateCaloClustersSlidingWindow(const std::string& name, ISvcLocator* svcLoc)
    : GaudiAlgorithm(name, svcLoc) {
  declareProperty("clusters", m_clusters, "Handle for calo clusters (output collection)");
  declareProperty("towerTool", m_towerTool, "Handle for the tower building tool");
}

StatusCode CreateCaloClustersSlidingWindow::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if (!m_towerTool.retrieve()) {
    error() << "Unable to retrieve the tower building tool." << endmsg;
    return StatusCode::FAILURE;
  }
  // Get number of calorimeter towers
  auto towerMapSize = m_towerTool->towersNumber();
  m_nEtaTower = towerMapSize.eta;
  m_nPhiTower = towerMapSize.phi;
  debug() << "Number of calorimeter towers (eta x phi) : " << m_nEtaTower << " x " << m_nPhiTower << endmsg;
  // make sure that the number of towers in eta is larger than the seeding sliding window
  if (m_nEtaTower < m_nEtaWindow) {
    debug() << "Window size in eta too small!!! Window " << m_nEtaWindow << " # of eta towers " << m_nEtaTower
            << endmsg;
    m_nEtaTower = m_nEtaWindow;
  }
  info() << "CreateCaloClustersSlidingWindow initialized" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode CreateCaloClustersSlidingWindow::execute() {
  // 1. Create calorimeter towers (calorimeter grid in eta phi, all layers merged)
  m_towers.assign(m_nEtaTower, std::vector<float>(m_nPhiTower, 0));
  // Create an output collection
  auto edmClusters = m_clusters.createAndPut();
  // Check if the tower building succeeded
  if (m_towerTool->buildTowers(m_towers) == 0) {
    debug() << "Empty cell collection." << endmsg;
    return StatusCode::SUCCESS;
  }
  // 2. Find local maxima with sliding window, build preclusters, calculate their barycentre position
  // calculate the sum of first m_nEtaWindow bins in eta, for each phi tower
  std::vector<float> sumOverEta(m_nPhiTower, 0);
  for (int iEta = 0; iEta < m_nEtaWindow; iEta++) {
    std::transform(sumOverEta.begin(), sumOverEta.end(), m_towers[iEta].begin(), sumOverEta.begin(),
                   std::plus<float>());
  }

  // preclusters with phi, eta weighted position and transverse energy
  std::vector<cluster> m_preClusters;
  int halfEtaPos = floor(m_nEtaPosition / 2.);
  int halfPhiPos = floor(m_nPhiPosition / 2.);
  float posEta = 0;
  float posPhi = 0;
  float sumEnergyPos = 0;

  // final cluster window
  int halfEtaFin = floor(m_nEtaFinal / 2.);
  int halfPhiFin = floor(m_nPhiFinal / 2.);
  double sumEnergyFin = 0.;
  int idEtaFin = 0;
  int idPhiFin = 0;

  // loop over all Eta slices starting at the half of the first window
  int halfEtaWin = floor(m_nEtaWindow / 2.);
  int halfPhiWin = floor(m_nPhiWindow / 2.);
  float sumWindow = 0;
  float sumPhiSlicePrevEtaWin = 0;
  float sumPhiSliceNextEtaWin = 0;
  float sumFirstPhiSlice = 0;
  float sumLastPhiSlice = 0;
  bool toRemove = false;
  // one slice in eta = window, now only sum over window elements in phi
  for (int iEta = halfEtaWin; iEta < m_nEtaTower - halfEtaWin; iEta++) {
    // sum the first window in phi
    for (int iPhiWindow = -halfPhiWin; iPhiWindow <= halfPhiWin; iPhiWindow++) {
      sumWindow += sumOverEta[phiNeighbour(iPhiWindow)];
    }
    // loop over all the phi slices
    for (int iPhi = 0; iPhi < m_nPhiTower; iPhi++) {
      // if energy is above threshold, it may be a precluster
      if (sumWindow > m_energyThreshold) {
        // test local maximum in phi
        // check closest neighbour on the right
        if (sumOverEta[phiNeighbour(iPhi - halfPhiWin)] < sumOverEta[phiNeighbour(iPhi + halfPhiWin + 1)]) {
          toRemove = true;
        }
        // check closest neighbour on the left
        if (sumOverEta[phiNeighbour(iPhi + halfPhiWin)] < sumOverEta[phiNeighbour(iPhi - halfPhiWin - 1)]) {
          toRemove = true;
        }
        // test local maximum in eta
        // check closest neighbour on the right (if it is not the first window)
        if (iEta > halfEtaWin) {
          for (int iPhiWindowLocalCheck = iPhi - halfPhiWin; iPhiWindowLocalCheck <= iPhi + halfPhiWin;
               iPhiWindowLocalCheck++) {
            sumPhiSlicePrevEtaWin += m_towers[iEta - halfEtaWin - 1][phiNeighbour(iPhiWindowLocalCheck)];
            sumLastPhiSlice += m_towers[iEta + halfEtaWin][phiNeighbour(iPhiWindowLocalCheck)];
          }
          if (sumPhiSlicePrevEtaWin > sumLastPhiSlice) {
            toRemove = true;
          }
        }
        // check closest neighbour on the left (if it is not the last window)
        if (iEta < m_nEtaTower - halfEtaWin - 1) {
          for (int iPhiWindowLocalCheck = iPhi - halfPhiWin; iPhiWindowLocalCheck <= iPhi + halfPhiWin;
               iPhiWindowLocalCheck++) {
            sumPhiSliceNextEtaWin += m_towers[iEta + halfEtaWin + 1][phiNeighbour(iPhiWindowLocalCheck)];
            sumFirstPhiSlice += m_towers[iEta - halfEtaWin][phiNeighbour(iPhiWindowLocalCheck)];
          }
          if (sumPhiSliceNextEtaWin > sumFirstPhiSlice) {
            toRemove = true;
          }
        }
        sumFirstPhiSlice = 0;
        sumLastPhiSlice = 0;
        sumPhiSlicePrevEtaWin = 0;
        sumPhiSliceNextEtaWin = 0;
        // Build precluster
        if (!toRemove) {
          // Calculate barycentre position (usually smaller window used to reduce noise influence)
          posEta = 0;
          posPhi = 0;
          sumEnergyPos = 0;
          // weighted mean for position in eta and phi
          for (int ipEta = iEta - halfEtaPos; ipEta <= iEta + halfEtaPos; ipEta++) {
            for (int ipPhi = iPhi - halfPhiPos; ipPhi <= iPhi + halfPhiPos; ipPhi++) {
              posEta += m_towerTool->eta(ipEta) * m_towers[ipEta][phiNeighbour(ipPhi)];
              posPhi += m_towerTool->phi(ipPhi) * m_towers[ipEta][phiNeighbour(ipPhi)];
              sumEnergyPos += m_towers[ipEta][phiNeighbour(ipPhi)];
            }
          }
          // If too small energy in the position window, calculate the position in the whole sliding window
          // Assigns correct position for cases with maximum energy deposits close to the border in eta
          if (sumEnergyPos > m_energyThresholdFraction * m_energyThreshold) {
            posEta /= sumEnergyPos;
            posPhi /= sumEnergyPos;
          } else {
            posEta = 0;
            posPhi = 0;
            sumEnergyPos = 0;
            for (int ipEta = iEta - halfEtaWin; ipEta <= iEta + halfEtaWin; ipEta++) {
              for (int ipPhi = iPhi - halfPhiWin; ipPhi <= iPhi + halfPhiWin; ipPhi++) {
                posEta += m_towerTool->eta(ipEta) * m_towers[ipEta][phiNeighbour(ipPhi)];
                posPhi += m_towerTool->phi(ipPhi) * m_towers[ipEta][phiNeighbour(ipPhi)];
                sumEnergyPos += m_towers[ipEta][phiNeighbour(ipPhi)];
              }
            }
            posEta /= sumEnergyPos;
            posPhi /= sumEnergyPos;
          }
          if (fabs(posPhi) > M_PI) {
            posPhi += -2 * M_PI * posPhi / fabs(posPhi);
          }
          // Calculate final cluster energy
          sumEnergyFin = 0;
          // Final cluster position
          idEtaFin = m_towerTool->idEta(posEta);
          idPhiFin = m_towerTool->idPhi(posPhi);
          // Recalculating the energy within the final cluster size
          for (int ipEta = idEtaFin - halfEtaFin; ipEta <= idEtaFin + halfEtaFin; ipEta++) {
            for (int ipPhi = idPhiFin - halfPhiFin; ipPhi <= idPhiFin + halfPhiFin; ipPhi++) {
              if (ipEta >= 0 && ipEta < m_nEtaTower) {  // check if we are not outside of map in eta
                sumEnergyFin += m_towers[ipEta][phiNeighbour(ipPhi)];
              }
            }
          }
          // check if changing the barycentre did not decrease energy below threshold
          if (sumEnergyFin > m_energyThreshold) {
            cluster newPreCluster;
            newPreCluster.eta = posEta;
            newPreCluster.phi = posPhi;
            newPreCluster.transEnergy = sumEnergyFin;
            m_preClusters.push_back(newPreCluster);
          }
        }
      }
      toRemove = false;
      // finish processing that window in phi, shift window to the next phi tower
      // substract first phi tower in current window
      sumWindow -= sumOverEta[phiNeighbour(iPhi - halfPhiWin)];
      // add next phi tower to the window
      sumWindow += sumOverEta[phiNeighbour(iPhi + halfPhiWin + 1)];
    }
    sumWindow = 0;
    // finish processing that slice, shift window to next eta tower
    if (iEta < m_nEtaTower - halfEtaWin - 1) {
      // substract first eta slice in current window
      std::transform(sumOverEta.begin(), sumOverEta.end(), m_towers[iEta - halfEtaWin].begin(), sumOverEta.begin(),
                     std::minus<float>());
      // add next eta slice to the window
      std::transform(sumOverEta.begin(), sumOverEta.end(), m_towers[iEta + halfEtaWin + 1].begin(), sumOverEta.begin(),
                     std::plus<float>());
    }
  }

  debug() << "Pre-clusters size before duplicates removal: " << m_preClusters.size() << endmsg;

  // 4. Sort the preclusters according to the transverse energy (descending)
  std::sort(m_preClusters.begin(), m_preClusters.end(),
            [](cluster clu1, cluster clu2) { return clu1.transEnergy > clu2.transEnergy; });

  // 5. Remove duplicates
  for (auto it1 = m_preClusters.begin(); it1 != m_preClusters.end(); it1++) {
    // loop over all clusters with energy lower than it1 (sorting), erase if too close
    for (auto it2 = it1 + 1; it2 != m_preClusters.end();) {
      if ((abs(m_towerTool->idEta((*it1).eta) - m_towerTool->idEta((*it2).eta)) < m_nEtaDuplicates) &&
          ((abs(m_towerTool->idPhi((*it1).phi) - m_towerTool->idPhi((*it2).phi)) < m_nPhiDuplicates) ||
           (abs(m_towerTool->idPhi((*it1).phi) - m_towerTool->idPhi((*it2).phi)) > m_nPhiTower - m_nPhiDuplicates))) {
        m_preClusters.erase(it2);
      } else {
        it2++;
      }
    }
  }
  debug() << "Pre-clusters size after duplicates removal: " << m_preClusters.size() << endmsg;

  // 6. Create final clusters
  // currently only role of r is to calculate x,y,z position
  double radius = m_towerTool->radiusForPosition();
  for (const auto clu : m_preClusters) {
    float clusterEnergy = clu.transEnergy * cosh(clu.eta);
    // apply energy sharing correction (if flag set to true)
    if (m_energySharingCorrection) {
      int idEtaCl = m_towerTool->idEta(clu.eta);
      int idPhiCl = m_towerTool->idPhi(clu.phi);
      // sum of energies in other clusters in each eta-phi tower of our current cluster (idEtaCl, idPhiCl)
      std::vector<std::vector<float>> sumEnergySharing;
      sumEnergySharing.assign(m_nEtaFinal, std::vector<float>(m_nPhiFinal, 0));
      // loop over all clusters and check if they have any tower in common with our current cluster
      for (const auto cluSharing : m_preClusters) {
        int idEtaClShare = m_towerTool->idEta(cluSharing.eta);
        int idPhiClShare = m_towerTool->idPhi(cluSharing.phi);
        if (idEtaCl != idEtaClShare && idPhiCl != idPhiClShare) {
          // check for overlap between clusters
          if (abs(idEtaClShare - idEtaCl) < m_nEtaFinal &&
              ((abs(idPhiClShare - idPhiCl) < m_nPhiFinal) ||
               (abs(idPhiClShare - idPhiCl) > m_nPhiTower - m_nPhiFinal))) {
            // add energy in shared towers to sumEnergySharing[][]
            for (int iEta = std::max(idEtaCl, idEtaClShare) - halfEtaFin;
                 iEta <= std::min(idEtaCl, idEtaClShare) + halfEtaFin; iEta++) {
              for (int iPhi = std::max(idPhiCl, idPhiClShare) - halfPhiFin;
                   iEta <= std::min(idPhiCl, idPhiClShare) + halfPhiFin; iPhi++) {
                if (iEta >= 0 && iEta < m_nEtaTower) {  // check if we are not outside of map in eta
                  sumEnergySharing[iEta - idEtaCl + halfEtaFin][iPhi - idPhiCl + halfPhiFin] +=
                      m_towers[iEta][phiNeighbour(iPhi)] * cosh(m_towerTool->eta(iEta));
                }
              }
            }
          }
        }
      }
      // apply the actual correction: substract the weighted energy contributions in other clusters
      for (int iEta = idEtaCl - halfEtaFin; iEta <= idEtaCl + halfEtaFin; iEta++) {
        for (int iPhi = idPhiCl - halfPhiFin; iPhi <= idPhiCl + halfPhiFin; iPhi++) {
          if (sumEnergySharing[iEta - idEtaCl + halfEtaFin][iPhi - idPhiCl + halfPhiFin] != 0) {
            float sumButOne = sumEnergySharing[iEta - idEtaCl + halfEtaFin][iPhi - idPhiCl + halfPhiFin];
            float towerEnergy = m_towers[iEta][phiNeighbour(iPhi)] * cosh(m_towerTool->eta(iEta));
            clusterEnergy -= towerEnergy * sumButOne / (sumButOne + towerEnergy);
          }
        }
      }
    }
    // save the clusters in our EDM
    // check ET thereshold once more (ET could change with the energy sharing correction)
    if (clusterEnergy / cosh(clu.eta) > m_energyThreshold) {
      auto edmCluster = edmClusters->create();
      auto& edmClusterCore = edmCluster.core();
      edmClusterCore.position.x = radius * cos(clu.phi);
      edmClusterCore.position.y = radius * sin(clu.phi);
      edmClusterCore.position.z = radius * sinh(clu.eta);
      edmClusterCore.energy = clusterEnergy;
      debug() << "Cluster eta: " << clu.eta << " phi: " << clu.phi << " x: " << edmClusterCore.position.x
              << " y: " << edmClusterCore.position.y << " z: " << edmClusterCore.position.z
              << " energy: " << edmClusterCore.energy << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode CreateCaloClustersSlidingWindow::finalize() { return GaudiAlgorithm::finalize(); }

unsigned int CreateCaloClustersSlidingWindow::phiNeighbour(int aIPhi) const {
  if (aIPhi < 0) {
    return m_nPhiTower + aIPhi;
  } else if (aIPhi >= m_nPhiTower) {
    return aIPhi % m_nPhiTower;
  }
  return aIPhi;
}
