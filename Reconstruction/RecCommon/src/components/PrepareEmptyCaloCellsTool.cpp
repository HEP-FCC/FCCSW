#include "PrepareEmptyCaloCellsTool.h"

//FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"
#include "DetSegmentation/GridPhiEta.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DDSegmentation/Segmentation.h"

// our EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloHit.h"


DECLARE_TOOL_FACTORY(PrepareEmptyCaloCellsTool)

PrepareEmptyCaloCellsTool::PrepareEmptyCaloCellsTool(const std::string& type,const std::string& name, const IInterface* parent) 
  : GaudiTool(type, name, parent)
{
  declareProperty("readoutName", m_readoutName, "ECalHitsPhiEta");
  declareProperty("fieldName", m_fieldName,"active_layer");
  declareInterface<IPrepareEmptyCaloCellsTool>(this);
}

PrepareEmptyCaloCellsTool::~PrepareEmptyCaloCellsTool()
{
}

StatusCode PrepareEmptyCaloCellsTool::initialize() {

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  info() << "PrepareEmptyCaloCellsTool initialized" << endmsg;

  m_geoSvc = service ("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
 
  info()<<"Readout: "<<m_readoutName<< " fieldName " << m_fieldName << endmsg;
  // check if readouts exist
  if(m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error()<<"Readout <<"<<m_readoutName<<">> does not exist."<<endmsg;
    return StatusCode::FAILURE;
  }

  m_segmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if(m_segmentation == nullptr) {
    info()<<"There is no phi-eta segmentation."<<endmsg;
  }
  // Take readout, bitfield from GeoSvc
  m_decoder = std::unique_ptr<DD4hep::DDSegmentation::BitField64>(new DD4hep::DDSegmentation::BitField64(m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder()->fieldDescription()));

  std::string m_volumeMatchName="LAr";

  // Get the total number of active volumes in the geometry
  auto highestVol = gGeoManager->GetTopVolume();
  info() << "Number of modules whose name matches " << m_volumeMatchName << ": \t" <<det::utils::countPlacedVolumes(highestVol, m_volumeMatchName) << endmsg;
  //ECAL specific: substract bath volume
  unsigned int numLayers = det::utils::countPlacedVolumes(highestVol, m_volumeMatchName)-1;

  //VolumeID
  for (unsigned int ilayer = 0; ilayer<numLayers; ilayer++) {
    (*m_decoder)["system"] = 5;
    (*m_decoder)["ECAL_Cryo"] = 1;
    (*m_decoder)["bath"] = 1;
    (*m_decoder)["EM_barrel"] = 1;
    (*m_decoder)["active_layer"] = ilayer+1;
    m_volumeId = m_decoder->getValue();
    info()<<"volumeID <<"<<m_volumeId<<endmsg;
 
    if(m_segmentation != nullptr) {
      info()<<"Number of segmentation cells in (phi,eta): "<<det::utils::numberOfCells(m_volumeId, *m_segmentation)<<endmsg;
    
      auto numCells = det::utils::numberOfCells(m_volumeId, *m_segmentation);
      //info()<<"numCells: eta " << numCells[0] << " phi " << numCells[1]<<endmsg;
      
      for (int ieta = -floor(numCells[0]*0.5); ieta<numCells[0]*0.5; ieta++) {
	for (int iphi = -floor(numCells[1]*0.5); iphi<numCells[1]*0.5; iphi++) {
	  (*m_decoder)["eta"] = ieta;
	  (*m_decoder)["phi"] = iphi;
	  m_cellId = m_decoder->getValue();
	  //info()<<"cellID <<"<<m_cellId<<endmsg;

	  fcc::CaloHit *newCell = new fcc::CaloHit();
	  newCell->Core().Cellid = m_cellId;
	  newCell->Core().Energy = 0;     
	  newCell->Core().Time = 0;
	  newCell->Core().Bits = 0;
	  m_caloCellsCollection.push_back(newCell);
	  //info() << "ieta " << ieta << " iphi " << iphi << " decoder " << m_decoder->valueString() << " cellID " << m_cellId << " celID " << newCell->Core().Cellid << endmsg; 
	}
      }
      
    }
    else {
    }

  }

  return sc;
}

std::vector<fcc::CaloHit*> PrepareEmptyCaloCellsTool::PrepareEmptyCells() {

  std::vector<fcc::CaloHit*> caloCellsCollection;

  for (auto ecell : m_caloCellsCollection) {
    ecell->Core().Energy = 0;     
    ecell->Core().Time = 0;
    ecell->Core().Bits = 0;
    caloCellsCollection.push_back(ecell);
    //info() << " cellID " <<ecell->Core().Cellid << " energy " << ecell->Core().Energy << endmsg; 
  }
  // info()<<"CaloCells size: "<<caloCellsCollection.size()<<endmsg;

  return caloCellsCollection;
}

StatusCode PrepareEmptyCaloCellsTool::finalize() {
  StatusCode sc = GaudiTool::finalize();
  info() << "PrepareEmptyCaloCellsTool finalized" << endmsg;
  return sc;
}
