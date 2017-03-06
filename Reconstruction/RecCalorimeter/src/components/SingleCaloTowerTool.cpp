#include "SingleCaloTowerTool.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// datamodel
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloHit.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DD4hep/Readout.h"

DECLARE_TOOL_FACTORY(SingleCaloTowerTool)

SingleCaloTowerTool::SingleCaloTowerTool(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ITowerTool>(this);
  declareInput("cells", m_cells, "calo/cells");
  declareProperty("readoutName", m_readoutName);
  // the default value to calculate the position of clusters
  declareProperty("radiusForPosition", m_radius = 1.);
  declareProperty("deltaEtaTower", m_deltaEtaTower = 0.01);
  declareProperty("deltaPhiTower", m_deltaPhiTower = 0.01);
  // needed for AlgTool wit output/input until it appears in Gaudi AlgTool constructor
  declareProperty("DataInputs", inputDataObjects());
  declareProperty("DataOutputs", outputDataObjects());
}

StatusCode SingleCaloTowerTool::initialize() {
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
  return StatusCode::SUCCESS;
}

StatusCode SingleCaloTowerTool::finalize() { return GaudiTool::finalize(); }

tower SingleCaloTowerTool::towersNumber() {
  // maximum eta of the detector (== to the eta offset)
  m_etaMax = fabs( m_segmentation->offsetEta() );
  // number of phi bins
  double epsilon = 0.0001;
  m_nPhiTower = ceil(2 * (M_PI-epsilon) / m_deltaPhiTower);
  // number of eta bins (if eta maximum is defined)
  m_nEtaTower = ceil(2 * (m_etaMax-epsilon) / m_deltaEtaTower);
  tower total;
  total.eta = m_nEtaTower;
  total.phi = m_nPhiTower;
  return total;
}

uint SingleCaloTowerTool::buildTowers(std::vector<std::vector<float>>& aTowers) {
  // Get the input collection with cells from simulation + digitisation (after calibration and with noise)
  const fcc::CaloHitCollection* cells = m_cells.get();
  debug() << "Input Hit collection size: " << cells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  int iPhi = 0, iEta = 0;
  float etaCell = 0;
  float deltaEtaHalf = m_deltaEtaTower*0.5;
  float deltaPhiHalf = m_deltaPhiTower*0.5;
  for (const auto& cell : *cells) {
    // find to which tower the cell belongs
    etaCell = m_segmentation->eta(cell.core().cellId);
    iEta = idEta(etaCell + deltaEtaHalf);
    iPhi = idPhi(m_segmentation->phi(cell.core().cellId) + deltaPhiHalf);
    // save transverse energy
    aTowers[iEta][phiNeighbour(iPhi)] += cell.core().energy / cosh(etaCell);
  }
  return cells->size();
}

uint SingleCaloTowerTool::idEta(float aEta) const {
  if ( floor((aEta + m_etaMax) / m_deltaEtaTower) < 0 ) {
    std::cout << "eta " << aEta << " "<< floor((aEta + m_etaMax) / m_deltaEtaTower) << " "<< ceil((aEta + m_etaMax) / m_deltaEtaTower) << std::endl;
  }
  return floor((aEta + m_etaMax) / m_deltaEtaTower);
}

uint SingleCaloTowerTool::idPhi(float aPhi) const {
  if ( floor((aPhi + M_PI) / m_deltaPhiTower) < 0 ) {
   std::cout << aPhi << " "<< floor((aPhi + M_PI) / m_deltaPhiTower) << std::endl;
  }
  return floor((aPhi + M_PI) / m_deltaPhiTower);
}

float SingleCaloTowerTool::eta(int aIdEta) const {
  // middle of the tower
  return ((aIdEta + 0.5) *  m_deltaEtaTower - m_etaMax);
}

float SingleCaloTowerTool::phi(int aIdPhi) const {
  // middle of the tower
  return ( (aIdPhi + 0.5) * m_deltaPhiTower - M_PI);
}

void SingleCaloTowerTool::matchCells(float eta, float phi, uint halfEtaFin, uint halfPhiFin, fcc::CaloCluster& aEdmCluster) {
  const fcc::CaloHitCollection* cells = m_cells.get();
  for (const auto& cell : *cells) {
    float etaCell = m_segmentation->eta(cell.core().cellId);
    float phiCell = m_segmentation->phi(cell.core().cellId);
    if ((abs(idEta(etaCell) - idEta(eta)) <= halfEtaFin) &&
        (abs(idPhi(phiCell) - idPhi(phi)) <= halfPhiFin)) {
       aEdmCluster.addhits(cell);
    }
  }
  return;
}

float SingleCaloTowerTool::radiusForPosition() const {
  return m_radius;
}

unsigned int SingleCaloTowerTool::phiNeighbour(int aIPhi) const {
  if (aIPhi < 0) {
    return m_nPhiTower + aIPhi;
  } else if (aIPhi >= m_nPhiTower) {
    return aIPhi % m_nPhiTower;
  }
  return aIPhi;
}
