#include "ConeSelection.h"

// FCCSW
#include "DetCommon/DetUtils.h"

// DD4hep
#include "DD4hep/Detector.h"

// root
#include "TVector2.h"
#include "TVector3.h"
#include "TMath.h"

// our EDM
#include "datamodel/CaloHit.h"
#include "datamodel/CaloHitCollection.h"
#include "datamodel/MCParticleCollection.h"

DECLARE_COMPONENT(ConeSelection)

ConeSelection::ConeSelection(const std::string& name, ISvcLocator* svcLoc) : GaudiAlgorithm(name, svcLoc) {
  declareProperty("cells", m_cells, "The cells (input)");
  declareProperty("particles", m_particles, "The geant particles (input)");
  declareProperty("selCells", m_selCells, "The selected cells (output)");
  declareProperty("positionsTool", m_cellPositionsTool, "Handle for tool to retrieve cell positions");
}

StatusCode ConeSelection::initialize() {

  // Check if cell positions tool available
  if (!m_cellPositionsTool.retrieve()) {
    error() << "Unable to retrieve cell positions tool!!!" << endmsg;
    return StatusCode::FAILURE;
  }

  info() << "ConeSelection initialized" << endmsg;
  debug() << "Cone radius: " << m_r << endmsg;
 
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  return StatusCode::SUCCESS;
}

StatusCode ConeSelection::execute() {

  m_cellsMap.clear();

  // Get the input collection with Geant4 hits
  const fcc::CaloHitCollection* cells = m_cells.get();
  debug() << "Input Cell collection size: " << cells->size() << endmsg;
  // Get the input particle collection 
  const fcc::MCParticleCollection* particles = m_particles.get();
  debug() << "Input Particle collection size: " << particles->size() << endmsg;

  fcc::CaloHitCollection* edmCellsCollection = new fcc::CaloHitCollection();
  // Loop over all generated particles
  for (const auto& part : *particles) {
    TVector3 genVec(part.p4().px,part.p4().py,part.p4().pz);
    auto genEta = genVec.Eta();
    auto genPhi = genVec.Phi();
    
    debug() << "Particle direction eta= " << genEta << ", phi= " << genPhi << endmsg;
    // Select cells within cone around particle direction
    for (const auto& cell : *cells) {
      auto posCell = m_cellPositionsTool->xyzPosition(cell.cellId());
      auto eta = posCell.Eta();
      auto phi = posCell.Phi();
      auto circPhi = TVector2::Phi_mpi_pi(phi-genPhi);
      double deltaR = double(sqrt(pow(circPhi,2)+pow((eta-genEta),2)));
      if (deltaR < m_r){
	//debug() << "Found a cell in cone: " << cell.cellId() << endmsg;
	m_cellsMap[cell.cellId()] = cell.energy();
      }
    }
    debug() << "Number of selected cells: " << m_cellsMap.size() << endmsg;
  }

  for (const auto& cell : m_cellsMap) {
    fcc::CaloHit newCell = edmCellsCollection->create();
    newCell.core().energy = cell.second;
    newCell.core().cellId = cell.first;
  }
  
  // push the CaloHitCollection to event store
  m_selCells.put(edmCellsCollection);
  debug() << "Output Cell collection size: " << edmCellsCollection->size() << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode ConeSelection::finalize() { return GaudiAlgorithm::finalize(); }
