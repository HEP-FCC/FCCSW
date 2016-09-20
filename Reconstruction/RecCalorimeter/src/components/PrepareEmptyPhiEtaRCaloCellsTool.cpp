#include "PrepareEmptyPhiEtaRCaloCellsTool.h"

//FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"

// DD4hep
#include "DD4hep/LCDD.h"

DECLARE_TOOL_FACTORY(PrepareEmptyPhiEtaRCaloCellsTool)

PrepareEmptyPhiEtaRCaloCellsTool::PrepareEmptyPhiEtaRCaloCellsTool(const std::string& type,const std::string& name, const IInterface* parent) 
  : GaudiTool(type, name, parent)
{
  declareProperty("readoutName", m_readoutName, "ECalHitsPhiEta");
  declareProperty("activeVolumeName", m_activeVolumeName,"LAr");
  declareProperty("numVolumesRemove",m_numVolumesRemove,"0");
  declareProperty("activeFieldName", m_activeFieldName,"active_layer");
  declareProperty("fieldNames", m_fieldNames);
  declareProperty("fieldValues", m_fieldValues);
  declareInterface<IPrepareEmptyCaloCellsTool>(this);
}

PrepareEmptyPhiEtaRCaloCellsTool::~PrepareEmptyPhiEtaRCaloCellsTool()
{
}

StatusCode PrepareEmptyPhiEtaRCaloCellsTool::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

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
  m_segmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if(m_segmentation == nullptr) {
    error()<<"There is no phi-eta segmentation."<<endmsg;
    return StatusCode::FAILURE;
  }
  // Get the total number of active volumes in the geometry
  auto highestVol = gGeoManager->GetTopVolume();
  // Substract volumes with same name as the active layers (e.g. ECAL: bath volume)
  unsigned int numLayers = det::utils::countPlacedVolumes(highestVol, m_activeVolumeName)-m_numVolumesRemove;
  info() << "Number of active layers " << numLayers << endmsg;

  // Check if size of names and values of readout fields agree
  if(m_fieldNames.size() != m_fieldValues.size()) {
    error() << "Size of names and values is not the same" << endmsg;
    return StatusCode::FAILURE;
  }

  // Loop over all cells in the calorimeter, create CaloHits objects with cellID(all other properties set to 0)
  // Loop over active layers
  // Take readout, bitfield from GeoSvc
  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();

  for (unsigned int ilayer = 0; ilayer<numLayers; ilayer++) {
    //Get VolumeID
    for(uint it=0; it<m_fieldNames.size(); it++) {
      (*decoder)[m_fieldNames[it]] = m_fieldValues[it];
    }
    (*decoder)[m_activeFieldName] = ilayer+1;
    uint64_t volumeId = decoder->getValue();
    
    debug()<<"Number of segmentation cells in (phi,eta): "<<det::utils::numberOfCells(volumeId, *m_segmentation)<<endmsg;
    //Number of cells in the volume
    auto numCells = det::utils::numberOfCells(volumeId, *m_segmentation);
    debug()<<"numCells: eta " << numCells[0] << " phi " << numCells[1]<<endmsg;
    //Loop over segmenation
    for (int ieta = -floor(numCells[0]*0.5); ieta<numCells[0]*0.5; ieta++) {
      for (int iphi = -floor(numCells[1]*0.5); iphi<numCells[1]*0.5; iphi++) {
	(*decoder)["eta"] = ieta;
	(*decoder)["phi"] = iphi;
	uint64_t cellId = decoder->getValue();
	
	fcc::CaloHit *newCell = new fcc::CaloHit();
	newCell->Core().Cellid = cellId;
	newCell->Core().Energy = 0;     
	newCell->Core().Time = 0;
	newCell->Core().Bits = 0;
	m_caloCellsCollection.push_back(newCell);
	debug() << "ieta " << ieta << " iphi " << iphi << " decoder " << decoder->valueString() << " cellID " << cellId << endmsg; 
      }
    }
     
  }

  return sc;
}

std::vector<fcc::CaloHit*> PrepareEmptyPhiEtaRCaloCellsTool::PrepareEmptyCells() {

  std::vector<fcc::CaloHit*> caloCellsCollection;

  for (auto ecell : m_caloCellsCollection) {
    ecell->Core().Energy = 0;     
    ecell->Core().Time = 0;
    ecell->Core().Bits = 0;
    caloCellsCollection.push_back(ecell);
  }
  debug()<<"CaloCells size: "<<caloCellsCollection.size()<<endmsg;

  return caloCellsCollection;
}

StatusCode PrepareEmptyPhiEtaRCaloCellsTool::finalize() {
  //Deleting the pointers to the cells
  for (auto ecell : m_caloCellsCollection) {
    delete ecell;
  }
  StatusCode sc = GaudiTool::finalize();
  return sc;
}
