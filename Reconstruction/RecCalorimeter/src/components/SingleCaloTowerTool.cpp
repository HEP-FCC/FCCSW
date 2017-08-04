#include "SingleCaloTowerTool.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// datamodel
#include "datamodel/CaloHit.h"
#include "datamodel/CaloHitCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DD4hep/Readout.h"

DECLARE_TOOL_FACTORY(SingleCaloTowerTool)

SingleCaloTowerTool::SingleCaloTowerTool(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareProperty("cells", m_cells, "Cells to create towers from (input)");
  declareInterface<ITowerTool>(this);
}

StatusCode SingleCaloTowerTool::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_geoSvc = service("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the "
               "configuration."
            << endmsg;
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
  // Take readout bitfield decoder from GeoSvc
  m_decoder =
      std::shared_ptr<DD4hep::DDSegmentation::BitField64>(m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder());
  // check if decoder contains "layer"
  std::vector<std::string> fields;
  for (uint itField = 0; itField < m_decoder->size(); itField++) {
    fields.push_back((*m_decoder)[itField].name());
  }
  auto iter = std::find(fields.begin(), fields.end(), "layer");
  if (iter == fields.end()) {
    error() << "Readout does not contain field layer." << endmsg;
    addLayerRestriction = false;
  } else
    addLayerRestriction = true;
  return StatusCode::SUCCESS;
}

StatusCode SingleCaloTowerTool::finalize() { return GaudiTool::finalize(); }

tower SingleCaloTowerTool::towersNumber() {
  // maximum eta of the detector (== eta offset + half of the cell size)
  m_etaMax = fabs(m_segmentation->offsetEta()) + m_segmentation->gridSizeEta() * 0.5;
  m_phiMax = fabs(m_segmentation->offsetPhi()) + M_PI / (double)m_segmentation->phiBins();

  // number of phi bins
  float epsilon = 0.0001;
  m_nPhiTower = ceil(2 * (m_phiMax - epsilon) / m_deltaPhiTower);
  // number of eta bins (if eta maximum is defined)
  m_nEtaTower = ceil(2 * (m_etaMax - epsilon) / m_deltaEtaTower);
  debug() << "etaMax " << m_etaMax << ", deltaEtaTower " << m_deltaEtaTower << ", nEtaTower " << m_nEtaTower << endmsg;
  debug() << "phiMax " << m_phiMax << ", deltaPhiTower " << m_deltaPhiTower << ", nPhiTower " << m_nPhiTower << endmsg;

  tower total;
  total.eta = m_nEtaTower;
  total.phi = m_nPhiTower;
  return total;
}

uint SingleCaloTowerTool::buildTowers(std::vector<std::vector<float>>& aTowers) {
  // Get the input collection with cells from simulation + digitisation (after
  // calibration and with noise)
  const fcc::CaloHitCollection* cells = m_cells.get();
  debug() << "Input cell collection size: " << cells->size() << endmsg;
  // Loop over a collection of calorimeter cells and build calo towers
  // borders of the cell in eta/phi
  float etaCellMin = 0, etaCellMax = 0;
  float phiCellMin = 0, phiCellMax = 0;
  int layerCell = 0;
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
  for (const auto& cell : *cells) {
    // find to which tower(s) the cell belongs
    etaCellMin = m_segmentation->eta(cell.core().cellId) - m_segmentation->gridSizeEta() * 0.5;
    etaCellMax = m_segmentation->eta(cell.core().cellId) + m_segmentation->gridSizeEta() * 0.5;
    phiCellMin = m_segmentation->phi(cell.core().cellId) - M_PI / (double)m_segmentation->phiBins();
    phiCellMax = m_segmentation->phi(cell.core().cellId) + M_PI / (double)m_segmentation->phiBins();
    if (addLayerRestriction == true) {
      m_decoder->setValue(cell.core().cellId);
      layerCell = (*m_decoder)["layer"].value();
    }
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
      fracEtaMin = fabs(eta(iEtaMin) + 0.5 * m_deltaEtaTower - etaCellMin) / m_segmentation->gridSizeEta();
      fracEtaMax = fabs(etaCellMax - eta(iEtaMax) + 0.5 * m_deltaEtaTower) / m_segmentation->gridSizeEta();
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
          fabs(phi(iPhiMin) + 0.5 * m_deltaPhiTower - phiCellMin) / (2 * M_PI / (double)m_segmentation->phiBins());
      fracPhiMax =
          fabs(phiCellMax - phi(iPhiMax) + 0.5 * m_deltaPhiTower) / (2 * M_PI / (double)m_segmentation->phiBins());
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
        if (addLayerRestriction == true) {
          if (layerCell <= m_maximumLayer) {
            aTowers[iEta][phiNeighbour(iPhi)] +=
                cell.core().energy / cosh(m_segmentation->eta(cell.core().cellId)) * ratioEta * ratioPhi;
          }
        } else
          aTowers[iEta][phiNeighbour(iPhi)] +=
              cell.core().energy / cosh(m_segmentation->eta(cell.core().cellId)) * ratioEta * ratioPhi;
      }
    }
  }
  return cells->size();
}

uint SingleCaloTowerTool::idEta(float aEta) const {
  uint id = floor((aEta + m_etaMax) / m_deltaEtaTower);
  return id;
}

uint SingleCaloTowerTool::idPhi(float aPhi) const {
  uint id = floor((aPhi + m_phiMax) / m_deltaPhiTower);
  return id;
}

float SingleCaloTowerTool::eta(int aIdEta) const {
  // middle of the tower
  return ((aIdEta + 0.5) * m_deltaEtaTower - m_etaMax);
}

float SingleCaloTowerTool::phi(int aIdPhi) const {
  // middle of the tower
  return ((aIdPhi + 0.5) * m_deltaPhiTower - m_phiMax);
}

uint SingleCaloTowerTool::phiNeighbour(int aIPhi) const {
  if (aIPhi < 0) {
    return m_nPhiTower + aIPhi;
  } else if (aIPhi >= m_nPhiTower) {
    return aIPhi % m_nPhiTower;
  }
  return aIPhi;
}

float SingleCaloTowerTool::radiusForPosition() const { return m_radius; }
