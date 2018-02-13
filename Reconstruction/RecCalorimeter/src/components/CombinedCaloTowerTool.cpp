#include "CombinedCaloTowerTool.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// datamodel
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloHit.h"

// DD4hep
//#include "DD4hep/LCDD.h"
#include "DD4hep/Readout.h"
#include "DD4hep/Detector.h"
#include "DDSegmentation/Segmentation.h"

DECLARE_TOOL_FACTORY(CombinedCaloTowerTool)

CombinedCaloTowerTool::CombinedCaloTowerTool(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ITowerTool>(this);
  declareProperty("ecalCells", m_ecalCells, "calo/ecalCells (input)");
  declareProperty("hcalCells", m_hcalCells, "calo/hcalCells (input)");
  declareProperty("ecalReadoutName", m_ecalReadoutName);
  declareProperty("hcalReadoutName", m_hcalReadoutName);
  // the default value to calculate the position of clusters
  declareProperty("radiusForPosition", m_radius = 1.);
  declareProperty("etaMax", m_etaMax = 0.);
  declareProperty("deltaEtaTower", m_deltaEtaTower = 0.01);
  declareProperty("deltaPhiTower", m_deltaPhiTower = 0.01);
  // needed for AlgTool wit output/input until it appears in Gaudi AlgTool constructor
  //  declareProperty("DataInputs", inputDataObjects());
  //declareProperty("DataOutputs", outputDataObjects());
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
  // check if readouts exist
  if (m_geoSvc->lcdd()->readouts().find(m_ecalReadoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout <<" << m_ecalReadoutName << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  if (m_geoSvc->lcdd()->readouts().find(m_hcalReadoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error() << "Readout <<" << m_hcalReadoutName << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }
  // retrieve PhiEta segmentation
  m_ecalSegmentation = dynamic_cast<dd4hep::DDSegmentation::FCCSWGridPhiEta*>(
      m_geoSvc->lcdd()->readout(m_ecalReadoutName).segmentation().segmentation());
  if (m_ecalSegmentation == nullptr) {
    error() << "There is no phi-eta segmentation in the electromagnetic calorimeter." << endmsg;
    return StatusCode::FAILURE;
  }
  m_hcalSegmentation = dynamic_cast<dd4hep::DDSegmentation::FCCSWGridPhiEta*>(
      m_geoSvc->lcdd()->readout(m_hcalReadoutName).segmentation().segmentation());
  if (m_hcalSegmentation == nullptr) {
    error() << "There is no phi-eta segmentation in the hadronic calorimeter." << endmsg;
    return StatusCode::FAILURE;
  }
  if( ! m_etaMax) {
    warning() << "Undefined detector size in eta. In each event the cell collection will be searched for maximum eta." << endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode CombinedCaloTowerTool::finalize() { return GaudiTool::finalize(); }

tower CombinedCaloTowerTool::towersNumber() {
  // number of phi bins
  m_nPhiTower = idPhi(2 * M_PI);
  // number of eta bins (if eta maximum is defined)
  if( m_etaMax) {
    m_nEtaTower = 2 * idEta(m_etaMax - m_deltaEtaTower / 2.) + 1;
  } else {
    m_nEtaTower = 0;
  }
  return {m_nEtaTower, m_nPhiTower};
}

int CombinedCaloTowerTool::etaTowersNumber() {
  // recalculate number of eta bins for each event
  const fcc::CaloHitCollection* ecalCells = m_ecalCells.get();
  float etaCell = 0;
  for (const auto& cell : *ecalCells) {
    etaCell = fabs(m_ecalSegmentation->eta(cell.core().cellId));
    if( etaCell > m_etaMax ) {
      m_etaMax = etaCell;
    }
  }
  const fcc::CaloHitCollection* hcalCells = m_hcalCells.get();
  for (const auto& cell : *hcalCells) {
    etaCell = fabs(m_hcalSegmentation->eta(cell.core().cellId));
    if( etaCell > m_etaMax ) {
      m_etaMax = etaCell;
    }
  }
  // eta from cell collection is middle of cell
  m_nEtaTower = 2 * m_etaMax / m_deltaEtaTower+ 1;
  return m_nEtaTower;
}


uint CombinedCaloTowerTool::buildTowers(std::vector<std::vector<float>>& aTowers) {
  // 1. ECAL
  // Get the input collection with cells from simulation + digitisation (after calibration and with noise)
  const fcc::CaloHitCollection* ecalCells = m_ecalCells.get();
  debug() << "Input cell collection size (electromagnetic calorimeter): " << ecalCells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  int iPhi = 0, iEta = 0;
  float etaCell = 0;
  for (const auto& cell : *ecalCells) {
    // find to which tower the cell belongs
    etaCell = m_ecalSegmentation->eta(cell.core().cellId);
    iEta = idEta(etaCell);
    iPhi = idPhi(m_ecalSegmentation->phi(cell.core().cellId));
    // save transverse energy
    aTowers[iEta][iPhi] += cell.core().energy / cosh(etaCell);
  }
  // 2. HCAL
  // Get the input collection with cells from simulation + digitisation (after calibration and with noise)
  const fcc::CaloHitCollection* hcalCells = m_hcalCells.get();
  debug() << "Input cell collection size (hadronic calorimeter): " << hcalCells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  for (const auto& cell : *hcalCells) {
    // find to which tower the cell belongs
    etaCell = m_hcalSegmentation->eta(cell.core().cellId);
    iEta = idEta(etaCell);
    iPhi = idPhi(m_hcalSegmentation->phi(cell.core().cellId));
    // save transverse energy
    aTowers[iEta][iPhi] += cell.core().energy / cosh(etaCell);
  }
  return ecalCells->size() + hcalCells->size();
}

uint CombinedCaloTowerTool::idEta(float aEta) const {
  // shift Ids so they start at 0 (segmentation returns IDs that may be from -N to N)
  // for segmentation in eta the middle cell has its centre at eta=0 (segmentation offset = 0)
  return floor((aEta + m_deltaEtaTower / 2.) / m_deltaEtaTower) + floor(m_nEtaTower / 2);
}

uint CombinedCaloTowerTool::idPhi(float aPhi) const {
  // shift Ids so they start at 0 (segmentation returns IDs that may be from -N to N)
  // for segmentation in phi the middle cell has its centre at phi=0 (segmentation offset = 0)
  return floor((aPhi + m_deltaPhiTower / 2.) / m_deltaPhiTower) + floor(m_nPhiTower / 2);
}

float CombinedCaloTowerTool::eta(int aIdEta) const {
  // middle of the tower
  return (aIdEta - (m_nEtaTower - 1) / 2) * m_deltaEtaTower;
}

float CombinedCaloTowerTool::phi(int aIdPhi) const {
  // middle of the tower
  return (aIdPhi - (m_nPhiTower - 1) / 2) * m_deltaPhiTower;
}

void CombinedCaloTowerTool::matchCells(float eta, float phi, uint halfEtaFin, uint halfPhiFin, fcc::CaloCluster& aEdmCluster) {
  // 1. ECAL
  const fcc::CaloHitCollection* ecalCells = m_ecalCells.get();
  for (const auto& cell : *ecalCells) {
    float etaCell = m_ecalSegmentation->eta(cell.core().cellId);
    float phiCell = m_ecalSegmentation->phi(cell.core().cellId);
    if ((abs(idEta(etaCell) - idEta(eta)) <= halfEtaFin) &&
        (abs(idPhi(phiCell) - idPhi(phi)) <= halfPhiFin)) {
       aEdmCluster.addhits(cell);
    }
  }
  // 1. HCAL
  const fcc::CaloHitCollection* hcalCells = m_hcalCells.get();
  for (const auto& cell : *hcalCells) {
    float etaCell = m_hcalSegmentation->eta(cell.core().cellId);
    float phiCell = m_hcalSegmentation->phi(cell.core().cellId);
    if ((abs(idEta(etaCell) - idEta(eta)) <= halfEtaFin) &&
        (abs(idPhi(phiCell) - idPhi(phi)) <= halfPhiFin)) {
       aEdmCluster.addhits(cell);
    }
  }
  return;
}

float CombinedCaloTowerTool::radiusForPosition() const {
  return m_radius;
}
