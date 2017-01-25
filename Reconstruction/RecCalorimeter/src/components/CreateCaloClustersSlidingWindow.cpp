#include "CreateCaloClustersSlidingWindow.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"

// Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// datamodel
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloClusterCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"

DECLARE_ALGORITHM_FACTORY(CreateCaloClustersSlidingWindow)

CreateCaloClustersSlidingWindow::CreateCaloClustersSlidingWindow(const std::string& name, ISvcLocator* svcLoc)
    : GaudiAlgorithm(name, svcLoc), m_volumeId(0) {
  declareInput("cells", m_cells, "calo/cells");
  declareOutput("clusters", m_clusters, "calo/clusters");
  declareProperty("readoutName", m_readoutName = "ECalHitsPhiEta");
  declareProperty("fieldNames", m_fieldNames);
  declareProperty("fieldValues", m_fieldValues);
  declareProperty("deltaEtaTower", m_deltaEtaTower = 0.01);
  declareProperty("deltaPhiTower", m_deltaPhiTower = 0.01);
  declareProperty("nEtaWindow", m_nEtaWindow = 5);
  declareProperty("nPhiWindow", m_nPhiWindow = 15);
  declareProperty("nEtaPosition", m_nEtaPosition = 3);
  declareProperty("nPhiPosition", m_nPhiPosition = 3);
  declareProperty("nEtaDuplicates", m_nEtaDuplicates = 2);
  declareProperty("nPhiDuplicates", m_nPhiDuplicates = 2);
  declareProperty("nEtaFinal", m_nEtaFinal = 5);
  declareProperty("nPhiFinal", m_nPhiFinal = 15);
  declareProperty("energyThreshold", m_energyThreshold = 3);
  declareProperty("checkPhiLocalMax", m_checkPhiLocalMax = true);
  declareProperty("checkEtaLocalMax", m_checkEtaLocalMax = true);
  declareProperty("saveCells", m_saveCells = false);
}

StatusCode CreateCaloClustersSlidingWindow::initialize() {
  if (GaudiAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  // check if readouts exist
  if (m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout <<" << m_readoutName << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  // retrieve PhiEta segmentation
  m_segmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(
      m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if (m_segmentation == nullptr) {
    error() << "There is no phi-eta segmentation." << endmsg;
    return StatusCode::FAILURE;
  }

  // get the ID of the volume for which the cells are counted
  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
  if (m_fieldNames.size() != m_fieldValues.size()) {
    error() << "Volume readout field descriptors (names and values) have different size. " << endmsg;
    return StatusCode::FAILURE;
  }
  decoder->setValue(0);
  for (unsigned int it = 0; it < m_fieldNames.size(); it++) {
    (*decoder)[m_fieldNames[it]] = m_fieldValues[it];
  }
  m_volumeId = decoder->getValue();
  info() << "Reconstruction in volume with ID " << m_volumeId << endmsg;

  info() << "CreateCaloClustersSlidingWindow initialized" << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode CreateCaloClustersSlidingWindow::execute() {
  // 1. Get calorimeter towers (calorimeter grid in eta phi, all layers merged)
  prepareTowers();
  buildTowers();
  debug() << "Number of calorimeter towers (eta x phi) : " << m_towers.size() << " x " << m_towers[0].size() << endmsg;

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
        if (m_checkPhiLocalMax) {
          // check closest neighbour on the right
          if (sumOverEta[phiNeighbour(iPhi - halfPhiWin)] < sumOverEta[phiNeighbour(iPhi + halfPhiWin + 1)]) {
            toRemove = true;
          }
          // check closest neighbour on the left
          if (sumOverEta[phiNeighbour(iPhi + halfPhiWin)] < sumOverEta[phiNeighbour(iPhi - halfPhiWin - 1)]) {
            toRemove = true;
          }
        }
        // test local maximum in eta (if it wasn't already marked as to be removed)
        if (m_checkEtaLocalMax && (!toRemove)) {
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
        }
        // Build precluster
        if (!toRemove) {
          // Calculate barycentre position (usually smaller window used to reduce noise influence)
          for (int ipEta = iEta - halfEtaPos; ipEta <= iEta + halfEtaPos; ipEta++) {
            for (int ipPhi = iPhi - halfPhiPos; ipPhi <= iPhi + halfPhiPos; ipPhi++) {
              posEta += eta(ipEta) * m_towers[ipEta][phiNeighbour(ipPhi)];
              posPhi += phi(phiNeighbour(ipPhi)) * m_towers[ipEta][phiNeighbour(ipPhi)];
              sumEnergyPos += m_towers[ipEta][phiNeighbour(ipPhi)];
            }
          }
          // If non-zero energy in the cluster, add to pre-clusters (reduced size for pos. calculation -> energy in the
          // core can be zero)
          if (sumEnergyPos != 0) {
            posEta /= sumEnergyPos;
            posPhi /= sumEnergyPos;
            // Calculate final cluster energy
            sumEnergyFin = 0;
            idEtaFin = idEta(posEta);
            idPhiFin = idPhi(posPhi);
            // Recalculating the energy within the final cluster size
            for (int ipEta = idEtaFin - halfEtaFin; ipEta <= idEtaFin + halfEtaFin; ipEta++) {
              for (int ipPhi = idPhiFin - halfPhiFin; ipPhi <= idPhiFin + halfPhiFin; ipPhi++) {
                if (idEtaFin > 0 && idEtaFin < m_nEtaTower) {
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
          posEta = 0;
          posPhi = 0;
          sumEnergyPos = 0;
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
      if ((abs(idEta((*it1).eta) - idEta((*it2).eta)) < m_nEtaDuplicates) &&
          ((abs(idPhi((*it1).phi) - idPhi((*it2).phi)) < m_nPhiDuplicates) ||
           (abs(idPhi((*it1).phi) - idPhi((*it2).phi)) > m_nPhiTower - m_nPhiDuplicates))) {
        m_preClusters.erase(it2);
      } else {
        it2++;
      }
    }
  }
  debug() << "Pre-clusters size after duplicates removal: " << m_preClusters.size() << endmsg;

  // 6. Create final clusters
  // currently r plays no role, take inner radius for position calculation
  auto tubeSizes = det::utils::tubeDimensions(m_volumeId);
  double rDetector = tubeSizes.x() * 10;  // cm to mm
  auto edmClusters = m_clusters.createAndPut();
  const fcc::CaloHitCollection* cells = m_cells.get();
  for (const auto clu : m_preClusters) {
    auto edmCluster = edmClusters->create();
    auto& edmClusterCore = edmCluster.core();
    edmClusterCore.position.x = rDetector * cos(clu.phi);
    edmClusterCore.position.y = rDetector * sin(clu.phi);
    edmClusterCore.position.z = rDetector * sinh(clu.eta);
    edmClusterCore.energy = clu.transEnergy * cosh(clu.eta);
    debug() << "Cluster eta: " << clu.eta << " phi: " << clu.phi << "x: " << edmClusterCore.position.x << " y "
            << edmClusterCore.position.y << " z " << edmClusterCore.position.z << " energy " << edmClusterCore.energy
            << endmsg;
    if (m_saveCells) {
      // loop over cells and see if they belong here
      for (const auto& cell : *cells) {
        float etaCell = m_segmentation->eta(cell.core().cellId);
        float phiCell = m_segmentation->phi(cell.core().cellId);
        if ((abs(idEta(etaCell) - idEta(clu.eta)) <= halfEtaFin) &&
            (abs(idPhi(phiCell) - idPhi(clu.phi)) <= halfPhiFin)) {
          edmCluster.addhits(cell);
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode CreateCaloClustersSlidingWindow::finalize() { return GaudiAlgorithm::finalize(); }

void CreateCaloClustersSlidingWindow::prepareTowers() {
  auto numOfCells = det::utils::numberOfCells(m_volumeId, *m_segmentation);
  m_nEtaTower = numOfCells[1];
  m_nPhiTower = numOfCells[0];
  m_towers.assign(m_nEtaTower, std::vector<float>(m_nPhiTower, 0));
  if (m_nPhiTower % 2 == 0 || m_nEtaTower % 2 == 0) {
    error() << "Number of segmentation bins must be an odd number. See detector documentation for more details."
            << endmsg;
  }
}

void CreateCaloClustersSlidingWindow::buildTowers() {
  // Get the input collection with cells from simulation + digitisation (after calibration and with noise)
  const fcc::CaloHitCollection* cells = m_cells.get();
  debug() << "Input Hit collection size: " << cells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  int iPhi = 0, iEta = 0;
  float etaCell = 0;
  for (const auto& ecell : *cells) {
    // find to which tower the cell belongs
    etaCell = m_segmentation->eta(ecell.core().cellId);
    iEta = idEta(etaCell);
    iPhi = idPhi(m_segmentation->phi(ecell.core().cellId));
    // save transverse energy
    m_towers[iEta][iPhi] += ecell.core().energy / cosh(etaCell);
  }
  return;
}

int CreateCaloClustersSlidingWindow::idEta(float aEta) const {
  // shift Ids so they start at 0 (segmentation returns IDs that may be from -N to N)
  // for segmentation in eta the middle cell has its centre at eta=0 (segmentation offset = 0)
  return floor((aEta + m_deltaEtaTower / 2.) / m_deltaEtaTower) + floor(m_nEtaTower / 2);
}

int CreateCaloClustersSlidingWindow::idPhi(float aPhi) const {
  // shift Ids so they start at 0 (segmentation returns IDs that may be from -N to N)
  // for segmentation in phi the middle cell has its centre at phi=0 (segmentation offset = 0)
  return floor((aPhi + m_deltaPhiTower / 2.) / m_deltaPhiTower) + floor(m_nPhiTower / 2);
}

float CreateCaloClustersSlidingWindow::eta(int aIdEta) const {
  // middle of the tower
  return (aIdEta - (m_nEtaTower - 1) / 2) * m_deltaEtaTower;
}

float CreateCaloClustersSlidingWindow::phi(int aIdPhi) const {
  // middle of the tower
  return (aIdPhi - (m_nPhiTower - 1) / 2) * m_deltaPhiTower;
}

unsigned int CreateCaloClustersSlidingWindow::phiNeighbour(int aIPhi) const {
  if (aIPhi < 0) {
    return m_nPhiTower + aIPhi;
  } else if (aIPhi >= m_nPhiTower) {
    return aIPhi % m_nPhiTower;
  }
  return aIPhi;
}
