#include "CombinedCaloTowerTool.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// datamodel
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloHit.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DD4hep/Readout.h"

DECLARE_TOOL_FACTORY(CombinedCaloTowerTool)

CombinedCaloTowerTool::CombinedCaloTowerTool(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareProperty("ecalCells", m_ecalCells, "");
  declareProperty("hcalCells", m_hcalCells, "");
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
  // check if readouts exist & retrieve PhiEta segmentation
  //if (m_ecalCells.getCollectionName()!="") {
  if (m_geoSvc->lcdd()->readouts().find(m_ecalReadoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout <<" << m_ecalReadoutName << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  m_ecalSegmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(
									 m_geoSvc->lcdd()->readout(m_ecalReadoutName).segmentation().segmentation());
  if (m_ecalSegmentation == nullptr) {
    error() << "There is no phi-eta segmentation in the electromagnetic calorimeter." << endmsg;
    return StatusCode::FAILURE;
  }
  //}
  //if (m_hcalCells!="") {
  if (m_geoSvc->lcdd()->readouts().find(m_hcalReadoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout <<" << m_hcalReadoutName << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  m_hcalSegmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(
									 m_geoSvc->lcdd()->readout(m_hcalReadoutName).segmentation().segmentation());
  if (m_hcalSegmentation == nullptr) {
    error() << "There is no phi-eta segmentation in the hadronic calorimeter." << endmsg;
    return StatusCode::FAILURE;
  }
    //}

  return StatusCode::SUCCESS;
}

StatusCode CombinedCaloTowerTool::finalize() { return GaudiTool::finalize(); }

tower CombinedCaloTowerTool::towersNumber() {
  
  std::vector<double> listPhiMax;
  std::vector<double> listEtaMax;
  listPhiMax.reserve(10);
  listEtaMax.reserve(10);
  //if (m_ecalCells!="") {
  listPhiMax.push_back(fabs(m_ecalSegmentation->offsetPhi()) + M_PI);
  listEtaMax.push_back(fabs(m_ecalSegmentation->offsetEta()) + m_ecalSegmentation->gridSizeEta() * 0.5);
    //}
  //if (m_hcalCells!="") {
  listPhiMax.push_back(fabs(m_hcalSegmentation->offsetPhi()) + M_PI);
  listEtaMax.push_back(fabs(m_hcalSegmentation->offsetEta()) + m_hcalSegmentation->gridSizeEta() * 0.5);
  //}
  
  //Maximum eta & phi of the calorimeter system
  m_phiMax = *std::max_element(listPhiMax.begin(), listPhiMax.end());
  m_etaMax = *std::max_element(listEtaMax.begin(), listEtaMax.end());
  debug() << "Detector limits: phiMax " << m_phiMax << " etaMax " << m_etaMax << endmsg;

  // m_nEtaTower = 2 * idEta(m_etaMax - m_deltaEtaTower / 2.) + 1;
  // number of phi bins
  float epsilon = 0.0001;
  m_nPhiTower = ceil(2 * (m_phiMax - epsilon) / m_deltaPhiTower);
  // number of eta bins (if eta maximum is defined)
  m_nEtaTower = ceil(2 * (m_etaMax - epsilon) / m_deltaEtaTower);
  debug() << "etaMax " << m_etaMax << ", deltaEtaTower " << m_deltaEtaTower << ", nEtaTower " << m_nEtaTower << endmsg;
  debug() << "phiMax " << M_PI << ", deltaPhiTower " << m_deltaPhiTower << ", nPhiTower " << m_nPhiTower << endmsg;

  tower total;
  total.eta = m_nEtaTower;
  total.phi = m_nPhiTower;
  return total;

}

uint CombinedCaloTowerTool::buildTowers(std::vector<std::vector<float>>& aTowers) {
  // 1. ECAL
  // Get the input collection with cells from simulation + digitisation (after calibration and with noise)
  const fcc::CaloHitCollection* ecalCells = m_ecalCells.get();
  debug() << "Input cell collection size (electromagnetic calorimeter): " << ecalCells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  CellsIntoTowers(aTowers, ecalCells, m_ecalSegmentation);

  const fcc::CaloHitCollection* hcalCells = m_hcalCells.get();
  debug() << "Input cell collection size (hadronic calorimeter): " << hcalCells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  CellsIntoTowers(aTowers, hcalCells, m_hcalSegmentation);
 
  return ecalCells->size() + hcalCells->size();
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

void CombinedCaloTowerTool::CellsIntoTowers(std::vector<std::vector<float>>& aTowers, const fcc::CaloHitCollection* aCells, DD4hep::DDSegmentation::GridPhiEta* aSegmentation) {
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
