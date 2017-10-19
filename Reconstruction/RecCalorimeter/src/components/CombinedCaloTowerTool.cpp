#include "CombinedCaloTowerTool.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// datamodel
#include "datamodel/CaloHit.h"
#include "datamodel/CaloHitCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DD4hep/Readout.h"

DECLARE_TOOL_FACTORY(CombinedCaloTowerTool)

CombinedCaloTowerTool::CombinedCaloTowerTool(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareProperty("ecalBarrelCells", m_ecalBarrelCells, "");
  declareProperty("calEndcapCells", m_calEndcapCells, "");
  declareProperty("calFwdCells", m_calFwdCells, "");
  declareProperty("hcalBarrelCells", m_hcalBarrelCells, "");
  declareProperty("hcalExtBarrelCells", m_hcalExtBarrelCells, "");
  declareInterface<ITowerTool>(this);
}

StatusCode CombinedCaloTowerTool::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }

  // check if readouts exist & retrieve PhiEta segmentations
  // if readout does not exist, reconstruction without this calorimeter part will be performed
  info() << "Retrieving Ecal barrel segmentation" << endmsg;
  m_ecalBarrelSegmentation = retrieveSegmentation(m_ecalBarrelReadoutName);
  info() << "Retrieving endcap calorimeter segmentation" << endmsg;
  m_calEndcapSegmentation = retrieveSegmentation(m_calEndcapReadoutName);
  info() << "Retrieving forward calorimeter segmentation" << endmsg;
  m_calFwdSegmentation = retrieveSegmentation(m_calFwdReadoutName);
  info() << "Retrieving Hcal barrel segmentation" << endmsg;
  m_hcalBarrelSegmentation = retrieveSegmentation(m_hcalBarrelReadoutName);
  info() << "Retrieving Hcal extended barrel segmentation" << endmsg;
  m_hcalExtBarrelSegmentation = retrieveSegmentation(m_hcalExtBarrelReadoutName);

  return StatusCode::SUCCESS;
}

StatusCode CombinedCaloTowerTool::finalize() { return GaudiTool::finalize(); }

tower CombinedCaloTowerTool::towersNumber() {

  std::vector<double> listPhiMax;
  std::vector<double> listEtaMax;
  listPhiMax.reserve(10);
  listEtaMax.reserve(10);
  if (m_ecalBarrelSegmentation != nullptr) {
    listPhiMax.push_back(fabs(m_ecalBarrelSegmentation->offsetPhi()) + M_PI / (double)m_ecalBarrelSegmentation->phiBins());
    listEtaMax.push_back(fabs(m_ecalBarrelSegmentation->offsetEta()) + m_ecalBarrelSegmentation->gridSizeEta() * 0.5);
  }
  if (m_calEndcapSegmentation != nullptr) {
    listPhiMax.push_back(fabs(m_calEndcapSegmentation->offsetPhi()) + M_PI / (double)m_calEndcapSegmentation->phiBins());
    listEtaMax.push_back(fabs(m_calEndcapSegmentation->offsetEta()) + m_calEndcapSegmentation->gridSizeEta() * 0.5);
  }
  if (m_calFwdSegmentation != nullptr) {
    listPhiMax.push_back(fabs(m_calFwdSegmentation->offsetPhi()) + M_PI / (double)m_calFwdSegmentation->phiBins());
    listEtaMax.push_back(fabs(m_calFwdSegmentation->offsetEta()) + m_calFwdSegmentation->gridSizeEta() * 0.5);
  }
  if (m_hcalBarrelSegmentation != nullptr) {
    listPhiMax.push_back(fabs(m_hcalBarrelSegmentation->offsetPhi()) + M_PI / (double)m_hcalBarrelSegmentation->phiBins());
    listEtaMax.push_back(fabs(m_hcalBarrelSegmentation->offsetEta()) + m_hcalBarrelSegmentation->gridSizeEta() * 0.5);
  }
  if (m_hcalExtBarrelSegmentation != nullptr) {
    listPhiMax.push_back(fabs(m_hcalExtBarrelSegmentation->offsetPhi()) + M_PI / (double)m_hcalExtBarrelSegmentation->phiBins());
    listEtaMax.push_back(fabs(m_hcalExtBarrelSegmentation->offsetEta()) +
                         m_hcalExtBarrelSegmentation->gridSizeEta() * 0.5);
  }

  // Maximum eta & phi of the calorimeter system
  m_phiMax = *std::max_element(listPhiMax.begin(), listPhiMax.end());
  m_etaMax = *std::max_element(listEtaMax.begin(), listEtaMax.end());
  debug() << "Detector limits: phiMax " << m_phiMax << " etaMax " << m_etaMax << endmsg;

  // m_nEtaTower = 2 * idEta(m_etaMax - m_deltaEtaTower / 2.) + 1;
  // number of phi bins
  float epsilon = 0.0001;
  m_nPhiTower = ceil(2 * (m_phiMax - epsilon) / m_deltaPhiTower);
  // number of eta bins (if eta maximum is defined)
  m_nEtaTower = ceil(2 * (m_etaMax - epsilon) / m_deltaEtaTower);
  debug() << "Towers: etaMax " << m_etaMax << ", deltaEtaTower " << m_deltaEtaTower << ", nEtaTower " << m_nEtaTower << endmsg;
  debug() << "Towers: phiMax " << m_phiMax << ", deltaPhiTower " << m_deltaPhiTower << ", nPhiTower " << m_nPhiTower << endmsg;

  tower total;
  total.eta = m_nEtaTower;
  total.phi = m_nPhiTower;
  return total;
}

uint CombinedCaloTowerTool::buildTowers(std::vector<std::vector<float>>& aTowers) {
  uint totalNumberOfCells = 0;

  // 1. ECAL barrel
  // Get the input collection with calorimeter cells
  const fcc::CaloHitCollection* ecalBarrelCells = m_ecalBarrelCells.get();
  debug() << "Input Ecal barrel cell collection size: " << ecalBarrelCells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  if (m_ecalBarrelSegmentation != nullptr) {
    CellsIntoTowers(aTowers, ecalBarrelCells, m_ecalBarrelSegmentation);
    totalNumberOfCells += ecalBarrelCells->size();
  }

  // 2. Endcap calorimeter
  const fcc::CaloHitCollection* calEndcapCells = m_calEndcapCells.get();
  debug() << "Input calorimeter endcap cell collection size: " << calEndcapCells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  if (m_calEndcapSegmentation != nullptr) {
    CellsIntoTowers(aTowers, calEndcapCells, m_calEndcapSegmentation);
    totalNumberOfCells += calEndcapCells->size();
  }

  // 3. Forward calorimeter
  const fcc::CaloHitCollection* calFwdCells = m_calFwdCells.get();
  debug() << "Input forward calorimeter cell collection size: " << calFwdCells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  if (m_calFwdSegmentation != nullptr) {
    CellsIntoTowers(aTowers, calFwdCells, m_calFwdSegmentation);
    totalNumberOfCells += calFwdCells->size();
  }

  // 4. HCAL barrel
  const fcc::CaloHitCollection* hcalBarrelCells = m_hcalBarrelCells.get();
  debug() << "Input hadronic barrel cell collection size: " << hcalBarrelCells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  if (m_hcalBarrelSegmentation != nullptr) {
    CellsIntoTowers(aTowers, hcalBarrelCells, m_hcalBarrelSegmentation);
    totalNumberOfCells += hcalBarrelCells->size();
  }

  // 4. HCAL extended barrel
  const fcc::CaloHitCollection* hcalExtBarrelCells = m_hcalExtBarrelCells.get();
  debug() << "Input hadronic extended barrel cell collection size: " << hcalExtBarrelCells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  if (m_hcalExtBarrelSegmentation != nullptr) {
    CellsIntoTowers(aTowers, hcalExtBarrelCells, m_hcalExtBarrelSegmentation);
    totalNumberOfCells += hcalExtBarrelCells->size();
  }

  return totalNumberOfCells;
}

uint CombinedCaloTowerTool::idEta(float aEta) const {
  uint id = floor((aEta + m_etaMax) / m_deltaEtaTower);
  return id;
}

uint CombinedCaloTowerTool::idPhi(float aPhi) const {
  uint id = floor((aPhi + m_phiMax) / m_deltaPhiTower);
  return id;
}

float CombinedCaloTowerTool::eta(int aIdEta) const {
  // middle of the tower
  return ((aIdEta + 0.5) * m_deltaEtaTower - m_etaMax);
}

float CombinedCaloTowerTool::phi(int aIdPhi) const {
  // middle of the tower
  return ((aIdPhi + 0.5) * m_deltaPhiTower - m_phiMax);
}

uint CombinedCaloTowerTool::phiNeighbour(int aIPhi) const {
  if (aIPhi < 0) {
    return m_nPhiTower + aIPhi;
  } else if (aIPhi >= m_nPhiTower) {
    return aIPhi % m_nPhiTower;
  }
  return aIPhi;
}

float CombinedCaloTowerTool::radiusForPosition() const { return m_radius; }

void CombinedCaloTowerTool::CellsIntoTowers(std::vector<std::vector<float>>& aTowers,
                                            const fcc::CaloHitCollection* aCells,
                                            DD4hep::DDSegmentation::GridPhiEta* aSegmentation) {
  // Loop over a collection of calorimeter cells and build calo towers
  // borders of the cell in eta/phi
  float etaCellMin = 0, etaCellMax = 0;
  float phiCellMin = 0, phiCellMax = 0;
  // tower index of the borders of the cell
  int iPhiMin = 0, iPhiMax = 0;
  int iEtaMin = 0, iEtaMax = 0;
  // fraction of cell area in eta/phi belonging to towers
  // Min - first tower, Max - last tower, Middle - middle tower(s)
  // If cell size <= tower size => first == last == middle tower, all fractions = 1
  // cell size > tower size => Sum of fractions = 1
  float ratioEta = 1.0, ratioPhi = 1.0;
  float fracEtaMin = 1.0, fracEtaMax = 1.0, fracEtaMiddle = 1.0;
  float fracPhiMin = 1.0, fracPhiMax = 1.0, fracPhiMiddle = 1.0;
  float epsilon = 0.0001;
  for (const auto& cell : *aCells) {
    // find to which tower(s) the cell belongs
    etaCellMin = aSegmentation->eta(cell.core().cellId) - aSegmentation->gridSizeEta() * 0.5;
    etaCellMax = aSegmentation->eta(cell.core().cellId) + aSegmentation->gridSizeEta() * 0.5;
    phiCellMin = aSegmentation->phi(cell.core().cellId) - M_PI / (double)aSegmentation->phiBins();
    phiCellMax = aSegmentation->phi(cell.core().cellId) + M_PI / (double)aSegmentation->phiBins();
    iEtaMin = idEta(etaCellMin + epsilon);
    iPhiMin = idPhi(phiCellMin + epsilon);
    iEtaMax = idEta(etaCellMax - epsilon);
    iPhiMax = idPhi(phiCellMax - epsilon);
    // if a cell is larger than a tower in eta/phi, calculate the fraction of
    // the cell area belonging to the first/last/middle towers
    fracEtaMin = 1.0;
    fracEtaMax = 1.0;
    fracEtaMiddle = 1.0;
    if (iEtaMin != iEtaMax) {
      fracEtaMin = fabs(eta(iEtaMin) + 0.5 * m_deltaEtaTower - etaCellMin) / aSegmentation->gridSizeEta();
      fracEtaMax = fabs(etaCellMax - eta(iEtaMax) + 0.5 * m_deltaEtaTower) / aSegmentation->gridSizeEta();
      if ((iEtaMax - iEtaMin - 1) != 0) {
        fracEtaMiddle = (1 - fracEtaMin - fracEtaMax) / float(iEtaMax - iEtaMin - 1);
      } else {
        fracEtaMiddle = 0.0;
      }
    }
    fracPhiMin = 1.0;
    fracPhiMax = 1.0;
    fracPhiMiddle = 1.0;
    if (iPhiMin != iPhiMax) {
      fracPhiMin =
          fabs(phi(iPhiMin) + 0.5 * m_deltaPhiTower - phiCellMin) / (2 * M_PI / (double)aSegmentation->phiBins());
      fracPhiMax =
          fabs(phiCellMax - phi(iPhiMax) + 0.5 * m_deltaPhiTower) / (2 * M_PI / (double)aSegmentation->phiBins());
      if ((iPhiMax - iPhiMin - 1) != 0) {
        fracPhiMiddle = (1 - fracPhiMin - fracPhiMax) / float(iPhiMax - iPhiMin - 1);
      } else {
        fracPhiMiddle = 0.0;
      }
    }

    // Loop through the appropriate towers and add transverse energy
    for (auto iEta = iEtaMin; iEta <= iEtaMax; iEta++) {
      if (iEta == iEtaMin) {
        ratioEta = fracEtaMin;
      } else if (iEta == iEtaMax) {
        ratioEta = fracEtaMax;
      } else {
        ratioEta = fracEtaMiddle;
      }
      for (auto iPhi = iPhiMin; iPhi <= iPhiMax; iPhi++) {
        if (iPhi == iPhiMin) {
          ratioPhi = fracPhiMin;
        } else if (iPhi == iPhiMax) {
          ratioPhi = fracPhiMax;
        } else {
          ratioPhi = fracPhiMiddle;
        }
        aTowers[iEta][phiNeighbour(iPhi)] +=
            cell.core().energy / cosh(aSegmentation->eta(cell.core().cellId)) * ratioEta * ratioPhi;
      }
    }
  }
}

DD4hep::DDSegmentation::GridPhiEta* CombinedCaloTowerTool::retrieveSegmentation(std::string aReadoutName) {
  DD4hep::DDSegmentation::GridPhiEta* segmentation = nullptr;
  if (m_geoSvc->lcdd()->readouts().find(aReadoutName) == m_geoSvc->lcdd()->readouts().end()) {
    info() << "Readout does not exist! Please check if it is correct. Processing without it." << endmsg;
  } else {
    info() << "Readout " << aReadoutName << " found." << endmsg;
    segmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(
        m_geoSvc->lcdd()->readout(aReadoutName).segmentation().segmentation());
    if (segmentation == nullptr) {
      error() << "There is no phi-eta segmentation for the readout " << aReadoutName << " defined." << endmsg;
    }
  }
  return segmentation;
}
