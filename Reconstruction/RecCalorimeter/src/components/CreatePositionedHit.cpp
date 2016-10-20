#include "CreatePositionedHit.h"

//FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"

// DD4hep
#include "DD4hep/LCDD.h"

DECLARE_ALGORITHM_FACTORY(CreatePositionedHit)

CreatePositionedHit::CreatePositionedHit(const std::string& name, ISvcLocator* svcLoc)
  : GaudiAlgorithm(name, svcLoc)
{
  declareInput("caloCells", m_caloCells,"caloCells");
  declareOutput("caloClusters", m_caloClusters,"caloClusters");
  declareProperty("readoutName", m_readoutName="ECalHitsNew");
  declareProperty("activeFieldName", m_activeFieldName="active_layer");
  declareProperty("activeVolumeName", m_activeVolumeName="LAr");
  //ECAL LAr specific: LAr bath in cryostat same material as active layer volume
  declareProperty("numVolumesRemove",m_numVolumesRemove=1);
}

CreatePositionedHit::~CreatePositionedHit()
{
}

StatusCode CreatePositionedHit::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return sc;

  m_geoSvc = service ("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  // check if readouts exist
  if(m_geoSvc->lcdd()->readouts().find(m_readoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error()<<"Readout <<"<<m_readoutName<<">> does not exist."<<endmsg;
    return StatusCode::FAILURE;
  }

  // Get PhiEta segmentation
  m_segmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());
  if(m_segmentation == nullptr) {
    error()<<"There is no phi-eta segmentation."<<endmsg;
    return StatusCode::FAILURE;
  }

  // Get the total number of active volumes in the geometry
  auto highestVol = gGeoManager->GetTopVolume();
  //Substract volumes with same name as the active layers (e.g. ECAL: bath volume)  
  m_numLayers = det::utils::countPlacedVolumes(highestVol, m_activeVolumeName)-m_numVolumesRemove;
  debug() << "Number of active layers " << m_numLayers << endmsg;

  return sc;
}

StatusCode CreatePositionedHit::execute() {
   
  //Get the input caloHits collection
  const fcc::CaloHitCollection* calocells = m_caloCells.get();
  debug() << "Input caloCells collection size: " << calocells->size() << endmsg;
    
  //Initialize output CaloClusterCollection
  fcc::CaloClusterCollection* edmClusterCollection = new fcc::CaloClusterCollection();
  
  //Intialize value of the half size in r
  double r_cell_size_half = -1.;

  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
  //Loop though CaloHits, calculate position from cellID, create and fill information in a CaloCluster 
  for (const auto& ecells : *calocells) {
    fcc::CaloCluster newCluster = edmClusterCollection->create();
    newCluster.Core().Energy = ecells.Core().Energy;     
    newCluster.Core().Time = ecells.Core().Time;
    newCluster.Core().Bits = ecells.Core().Cellid;
    
    //Current active layer r-minimum r
    //Volume dimensions of Tube - det::utils::tubeDimensions(volumeID) - ThreeVector rmin, rmax, dz (half-length)
    double rmin_layer = det::utils::tubeDimensions(ecells.Core().Cellid).x();
    //Next active layer [ needed for r_cell = middle of the cell (active + passive) ]
    //If half size of cell in r not known, calculate it from the next layer r-min
    decoder->setValue(ecells.Core().Cellid);
    if (r_cell_size_half<0) {
      if ((*decoder)[m_activeFieldName]<m_numLayers) {
	(*decoder)[m_activeFieldName]=(*decoder)[m_activeFieldName]+1;
	uint64_t cellID_next = decoder->getValue();
	double rmin_layer_next = det::utils::tubeDimensions(cellID_next).x();
	r_cell_size_half = (rmin_layer_next - rmin_layer)*0.5;
      }
      else {
	info() << "Cell size in r not defined!!!! " << endmsg;
      }
    }
    //r_cell: middle of the cell (active + passive)
    double r_cell = rmin_layer+r_cell_size_half;
   
    //Global position of the cell
    auto position =  m_segmentation->positionFromREtaPhi( r_cell, m_segmentation->eta(ecells.Core().Cellid), m_segmentation->phi(ecells.Core().Cellid) );
    newCluster.Core().position.X = position.x()*10.;
    newCluster.Core().position.Y = position.y()*10.;
    newCluster.Core().position.Z = position.z()*10.;
      
    //Debug information about cells
    debug() << "cellID " << ecells.Core().Cellid <<" energy " << ecells.Core().Energy << " decoder: all fields " << decoder->valueString() << " r " << r_cell << " eta " <<  m_segmentation->eta(ecells.Core().Cellid) << " phi " <<  m_segmentation->phi(ecells.Core().Cellid)<< endmsg;
  }
  debug() << "Output CaloCluster collection size: " << edmClusterCollection->size() << endmsg;

  //Push the CaloClusterCollection to event store
  m_caloClusters.put(edmClusterCollection);
 
  return StatusCode::SUCCESS;
}

StatusCode CreatePositionedHit::finalize() {  
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
