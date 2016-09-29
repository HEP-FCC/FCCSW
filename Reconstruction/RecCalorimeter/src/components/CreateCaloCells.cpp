#include "CreateCaloCells.h"

//FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"

// DD4hep
#include "DD4hep/LCDD.h"

// our EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloHit.h"

#include <vector>

DECLARE_ALGORITHM_FACTORY(CreateCaloCells)

CreateCaloCells::CreateCaloCells(const std::string& name, ISvcLocator* svcLoc)
  : GaudiAlgorithm(name, svcLoc)
{
  declareInput("hits", m_hits,"hits");
  declareOutput("cells", m_cells,"cells");

  declareProperty("mergeTool",m_mergeTool);
  declarePrivateTool(m_mergeTool,"MergeCaloHitsTool");
  declareProperty("calibTool",m_calibTool);
  declarePrivateTool(m_calibTool,"CalibrateCaloHitsTool");
  declareProperty("noiseTool",m_noiseTool);
  declarePrivateTool(m_noiseTool,"NoiseCaloCellsTool");

  declareProperty("doCellCalibration",m_doCellCalibration=true);
  declareProperty("addCellNoise",m_addCellNoise=true);
  declareProperty("filterCellNoise",m_filterCellNoise=false);

  //PhiEta segmentation required
  declareProperty("readoutName", m_readoutName="ECalHitsPhiEta");
  declareProperty("activeVolumeName", m_activeVolumeName="LAr");
  //number of volumes with active material which are not readout
  declareProperty("numVolumesRemove",m_numVolumesRemove=0);
  declareProperty("activeFieldName", m_activeFieldName="active_layer");
  declareProperty("fieldNames", m_fieldNames);
  declareProperty("fieldValues", m_fieldValues);

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
  info() << "filterCellNoise   : " << m_filterCellNoise << endmsg;

  //Initialization of various tools
  //Merge hits
  if (!m_mergeTool.retrieve()) {
    error()<<"Unable to retrieve the merge hits to cells tool!!!"<<endmsg;
    return StatusCode::FAILURE;
  }

  //Calibrate Geant4 energy to EM scale tool
  if (m_doCellCalibration) {
    if (!m_calibTool.retrieve()) {
      error()<<"Unable to retrieve the calo cells calibration tool!!!"<<endmsg;
      return StatusCode::FAILURE;
    }
  }

  //Cell noise tool
  if (m_addCellNoise) {
    if (!m_noiseTool.retrieve()) {
      error()<<"Unable to retrieve the calo cells noise tool!!!"<<endmsg;
      return StatusCode::FAILURE;
    }
    //Prepare a vector of all cells in calorimeter with their cellID
    StatusCode sc_prepareCells = prepareEmptyCells(m_edmHitsNoiseVector);
    if (sc_prepareCells.isFailure()) {
      error()<<"Unable to create empty cells!!! Check the input for readout"<<endmsg;
      return StatusCode::FAILURE;
    }
  }
    
  return sc;
}

StatusCode CreateCaloCells::execute() {
   
  //Get the input collection with Geant4 hits
  const fcc::CaloHitCollection* hits = m_hits.get();
  debug() << "Input Hit collection size: " << hits->size() << endmsg;

  //Final vector of cells
  std::vector<fcc::CaloHit*> edmFinalCellsVector;

  //Merge hits with the same cellID
  std::vector<fcc::CaloHit*> edmMergedHitsVector = m_mergeTool->mergeOneCollection(*hits);

  //Calibrate Geant4 energy to EM scale tool
  if (m_doCellCalibration) {
    m_calibTool->calibrate(edmMergedHitsVector);
  }
  
 
  if (!m_addCellNoise) {
    edmFinalCellsVector = edmMergedHitsVector;
  }
  //Create random noise hits for each cell, merge noise hits with signal hits, apply filter on cell energy if required
  else {
    m_noiseTool->createRandomCellNoise(m_edmHitsNoiseVector);
    edmFinalCellsVector = m_mergeTool->mergeTwoVectors(edmMergedHitsVector, m_edmHitsNoiseVector);
    if (m_filterCellNoise) {
      m_noiseTool->filterCellNoise(edmFinalCellsVector);
    }
  }

  //Copy information from vector to CaloHitCollection
  fcc::CaloHitCollection* edmCellsCollection = new fcc::CaloHitCollection();
  for (auto ecells : edmFinalCellsVector) {
    fcc::CaloHit newCell = edmCellsCollection->create();
      newCell.Core().Energy = ecells->Core().Energy;     
      newCell.Core().Time = ecells->Core().Time;
      newCell.Core().Bits = ecells->Core().Bits;
      newCell.Core().Cellid = ecells->Core().Cellid;
  }
  debug() << "Output Cell collection size: " << edmCellsCollection->size() << endmsg;

  //Cleaning the vectors of CaloHits*
  for (auto ecells : edmFinalCellsVector) {
    delete ecells;
  }
  if (m_addCellNoise) {
    for (auto ecells : edmMergedHitsVector) {
      delete ecells;
    }
  }


  //Push the CaloHitCollection to event store
  m_cells.put(edmCellsCollection);
 
  return StatusCode::SUCCESS;
}

StatusCode CreateCaloCells::finalize() {  
  //Cleaning of the vector of CaloHits*
  for (auto ehit: m_edmHitsNoiseVector) {
    delete ehit;
  }
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}

StatusCode CreateCaloCells::prepareEmptyCells(std::vector<fcc::CaloHit*>& caloCellsVector) {

  // Get GeoSvc
  m_geoSvc = service ("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
 
  // Check if readouts exist
  info()<<"Readout: "<<m_readoutName<< endmsg;
  if(m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error()<<"Readout <<"<<m_readoutName<<">> does not exist."<<endmsg;
    return StatusCode::FAILURE;
  }

  //Get PhiEta segmentation
  auto segmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if(segmentation == nullptr) {
    error()<<"There is no phi-eta segmentation!!!!"<<endmsg;
    return StatusCode::FAILURE;
  }
  // Get the total number of active volumes in the geometry
  auto highestVol = gGeoManager->GetTopVolume();
  // Substract volumes with active material which are not readout (e.g. ECAL: bath volume)
  unsigned int numLayers = det::utils::countPlacedVolumes(highestVol, m_activeVolumeName)-m_numVolumesRemove;
  info() << "Number of active layers " << numLayers << endmsg;

  // Check if size of names and values of readout fields agree
  if(m_fieldNames.size() != m_fieldValues.size()) {
    error() << "Size of names and values is not the same" << endmsg;
    return StatusCode::FAILURE;
  }

  // Take readout, bitfield from GeoSvc
  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();

  // Loop over all cells in the calorimeter, create CaloHits objects with cellID(all other properties set to 0)
  // Loop over active layers
  for (unsigned int ilayer = 0; ilayer<numLayers; ilayer++) {
    //Get VolumeID
    for(uint it=0; it<m_fieldNames.size(); it++) {
      (*decoder)[m_fieldNames[it]] = m_fieldValues[it];
    }
    (*decoder)[m_activeFieldName] = ilayer+1;
    uint64_t volumeId = decoder->getValue();
    
    debug()<<"Number of segmentation cells in (phi,eta): "<<det::utils::numberOfCells(volumeId, *segmentation)<<endmsg;
    //Number of cells in the volume
    auto numCells = det::utils::numberOfCells(volumeId, *segmentation);
    //Loop over segmenation
    for (int iphi = -floor(numCells[0]*0.5); iphi<numCells[0]*0.5; iphi++) {
      for (int ieta = -floor(numCells[1]*0.5); ieta<numCells[1]*0.5; ieta++) {
	(*decoder)["phi"] = iphi;
	(*decoder)["eta"] = ieta;
	uint64_t cellId = decoder->getValue();
	
	fcc::CaloHit *newCell = new fcc::CaloHit();
	newCell->Core().Cellid = cellId;
	newCell->Core().Energy = 0;     
	newCell->Core().Time = 0;
	newCell->Core().Bits = 0;
	caloCellsVector.push_back(newCell);
	debug() << "ieta " << ieta << " iphi " << iphi << " decoder " << decoder->valueString() << " cellID " << cellId << endmsg; 
      }
    }     
  }

  return StatusCode::SUCCESS;
}

