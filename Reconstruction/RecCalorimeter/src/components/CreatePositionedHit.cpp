#include "CreatePositionedHit.h"

//FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"

// our EDM
#include "datamodel/CaloHitCollection.h"
#include "datamodel/PositionedCaloHitCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"

DECLARE_ALGORITHM_FACTORY(CreatePositionedHit)

CreatePositionedHit::CreatePositionedHit(const std::string& name, ISvcLocator* svcLoc)
: GaudiAlgorithm(name, svcLoc) {
  declareInput("caloCells", m_caloCells,"caloCells");
  declareOutput("caloPositionedHits", m_caloPositionedHits,"caloPositionedHits");
  declareProperty("readoutName", m_readoutName="ECalHitsNew");
  declareProperty("activeFieldName", m_activeFieldName="active_layer");
  declareProperty("activeVolumeName", m_activeVolumeName="LAr_sensitive");
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
  m_numLayers = det::utils::countPlacedVolumes(highestVol, m_activeVolumeName);
  debug() << "Number of active layers " << m_numLayers << endmsg;

  return sc;
}

StatusCode CreatePositionedHit::execute() {

  //Get the input caloHits collection
  const fcc::CaloHitCollection* calocells = m_caloCells.get();
  debug() << "Input caloCells collection size: " << calocells->size() << endmsg;

  //Initialize output CaloClusterCollection
  auto edmPositionedHitCollection = m_caloPositionedHits.createAndPut();

  //Intialize value of the half size in r
  double r_cell_size_half = -1.;
  uint64_t id = 0;
  auto decoder = m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder();
  //Loop though CaloHits, calculate position from cellID, create and fill information in a CaloCluster
  for (const auto& cell : *calocells) {
    id = cell.core().cellId;

    //Current active layer r-minimum r
    //Volume dimensions of Tube - det::utils::tubeDimensions(volumeID) - ThreeVector rmin, rmax, dz (half-length)
    double rmin_layer = det::utils::tubeDimensions(id).x();
    //Next active layer [ needed for r_cell = middle of the cell (active + passive) ]
    //If half size of cell in r not known, calculate it from the next layer r-min
    decoder->setValue(id);
    if (r_cell_size_half < 0) {
      if ((*decoder)[m_activeFieldName] < m_numLayers - 1) {
        (*decoder)[m_activeFieldName] = (*decoder)[m_activeFieldName] + 1;
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
    auto position =  m_segmentation->positionFromREtaPhi( r_cell, m_segmentation->eta(id), m_segmentation->phi(id) );
    auto edmPos = fcc::Point();
    edmPos.x = position.x();
    edmPos.y = position.y();
    edmPos.z = position.z();

    auto positionedHit = edmPositionedHitCollection->create(edmPos, cell.core());

    //Debug information about cells
    if ( msgLevel ( MSG::DEBUG ) ) {
      debug() << "cellID " << id <<" energy " << cell.core().energy << " decoder: all fields "
              << decoder->valueString() << " r " << r_cell << " eta " <<  m_segmentation->eta(id)
              << " phi " <<  m_segmentation->phi(id)<< endmsg;
    }
  }
  debug() << "Output CaloCluster collection size: " << edmPositionedHitCollection->size() << endmsg;


  return StatusCode::SUCCESS;
}

StatusCode CreatePositionedHit::finalize() {
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
