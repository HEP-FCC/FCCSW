#include "CreateCaloCells.h"

// our EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloHit.h"

#include <vector>

DECLARE_ALGORITHM_FACTORY(CreateCaloCells)

CreateCaloCells::CreateCaloCells(const std::string& name, ISvcLocator* svcLoc)
  : GaudiAlgorithm(name, svcLoc),
  m_doCellCalibration(true)
{
  declareProperty("doCellCalibration",m_doCellCalibration=true);
  declareProperty("addCellNoise",m_addCellNoise=true);
  declareProperty("prepareTool",m_prepareTool);
  declarePrivateTool(m_prepareTool,"PrepareEmptyCaloCellsTool", true);
  declareProperty("mergeTool",m_mergeTool);
  declarePrivateTool(m_mergeTool,"MergeHitsToCaloCellsTool", true);
  declareProperty("calibTool",m_calibTool);
  declarePrivateTool(m_calibTool,"CalibrateCaloCellsTool", true);
  declareProperty("noiseTool",m_noiseTool);
  declarePrivateTool(m_noiseTool,"NoiseCaloCellsTool", true);
  declareInput("hits", m_caloHits, "hits");
  declareOutput("caloCells", m_caloCells,"caloCells");
}

CreateCaloCells::~CreateCaloCells()
{
}

StatusCode CreateCaloCells::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  info() << "CreateCaloCells initialized" << endmsg;
  info() << "doCellCalibration : " << m_doCellCalibration << endmsg;
  info() << "addCellNoise      : " << m_addCellNoise << endmsg;

  //Initialization of various tools
  //Prepare CaloHits for each cell
  if (!m_prepareTool.retrieve()) {
    error()<<"Unable to retrieve the prepare empty cells tool!!!"<<endmsg;
    return StatusCode::FAILURE;
  }

  //Merge Geant4 hits to cells tool
  if (!m_mergeTool.retrieve()) {
    error()<<"Unable to retrieve the merge hits to cells tool!!!"<<endmsg;
    return StatusCode::FAILURE;
  }

  //Calibrate Geant4 energy to EM scale tool
  if (!m_calibTool.retrieve()) {
    error()<<"Unable to retrieve the calo cells calibration tool!!!"<<endmsg;
    return StatusCode::FAILURE;
  }

  //Cell noise tool
  if (!m_noiseTool.retrieve()) {
    error()<<"Unable to retrieve the calo cells noise tool!!!"<<endmsg;
    return StatusCode::FAILURE;
  }
    
  return sc;
}

StatusCode CreateCaloCells::execute() {
   
  //Get the input collection with Geant4 hits
  const fcc::CaloHitCollection* calohits = m_caloHits.get();
  debug() << "Input Hit collection size: " << calohits->size() << endmsg;
    
  //CaloHitCollection for the final cells information (stored in event store)
  //Create a new CaloHitCollection (vector of CaloHit at the moment - problem with changing information in CaloHitCollection - read only)
  //TODO: Create empty cells with correct cellID
  std::vector<fcc::CaloHit*> edmCellsVector = m_prepareTool->PrepareEmptyCells();
  //Merge the hits into the cells
  m_mergeTool->DoMerge(*calohits, edmCellsVector);

  //Calibrate Geant4 energy to EM scale tool
  if (m_doCellCalibration) {
    m_calibTool->Calibrate(edmCellsVector);
  }
    
  //Add cell noise to cells
  if (m_addCellNoise) {
    m_noiseTool->AddNoise(edmCellsVector);
  }

  //Copy information from vector of CaloHit to CaloHitCollection
  fcc::CaloHitCollection* edmCellsCollection = new fcc::CaloHitCollection();
  for (auto ecells : edmCellsVector) {
    fcc::CaloHit newCell = edmCellsCollection->create();
      newCell.Core().Energy = ecells->Core().Energy;     
      newCell.Core().Time = ecells->Core().Time;
      newCell.Core().Bits = ecells->Core().Bits;
      newCell.Core().Cellid = ecells->Core().Cellid;
  }
  debug() << "Output Cell collection size: " << edmCellsCollection->size() << endmsg;
  //Push the CaloHitCollection to event store
  m_caloCells.put(edmCellsCollection);
 
  return StatusCode::SUCCESS;
}

StatusCode CreateCaloCells::finalize() {  
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
