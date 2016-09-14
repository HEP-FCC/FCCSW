#include "SaveCaloClusters.h"

//FCCSW
#include "DetInterface/IGeoSvc.h"
#include "DetCommon/DetUtils.h"
#include "DetSegmentation/GridPhiEta.h"

// our EDM
#include "datamodel/CaloCluster.h"
#include "datamodel/CaloClusterCollection.h"
#include "datamodel/CaloHitCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DDSegmentation/CartesianGridXYZ.h"
#include "DDSegmentation/Segmentation.h"

//CLHEP
//#include "CLHEP/Vector/ThreeVector.h"

DECLARE_ALGORITHM_FACTORY(SaveCaloClusters)

SaveCaloClusters::SaveCaloClusters(const std::string& name, ISvcLocator* svcLoc)
  : GaudiAlgorithm(name, svcLoc)
{
  declareInput("caloCells", m_caloCells, "caloCells");
  declareOutput("caloClusters", m_caloClusters, "caloClusters");
  declareProperty("readoutName", m_readoutName, "ECalHitsNew");
  declareProperty("fieldName", m_fieldName,"system");
}

SaveCaloClusters::~SaveCaloClusters()
{
}

StatusCode SaveCaloClusters::initialize() {

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

  m_segmentation = dynamic_cast<DD4hep::DDSegmentation::GridPhiEta*>(m_geoSvc->lcdd()->readout(m_readoutName).segmentation().segmentation());

  // Take readout, bitfield from GeoSvc
  m_decoder = std::unique_ptr<DD4hep::DDSegmentation::BitField64>(new DD4hep::DDSegmentation::BitField64(m_geoSvc->lcdd()->readout(m_readoutName).idSpec().decoder()->fieldDescription()));
  
  return sc;
}

StatusCode SaveCaloClusters::execute() {
   
  //Get the input collection with Geant4 hits
  const fcc::CaloHitCollection* calocells = m_caloCells.get();
  debug() << "Input caloCells collection size: " << calocells->size() << endmsg;
    
  //Copy information from vector of CaloHit to CaloHitCollection
  fcc::CaloClusterCollection* edmClusterCollection = new fcc::CaloClusterCollection();
  

  for (const auto& ecells : *calocells) {
    fcc::CaloCluster newCluster = edmClusterCollection->create();
    newCluster.Core().Energy = ecells.Core().Energy;     
    newCluster.Core().Time = ecells.Core().Time;
    newCluster.Core().Bits = ecells.Core().Cellid;
    
    //Volume dimension (Tube) - det::utils::tubeDimensions(volumeID) - ThreeVector rmin, rmax, dz (half-length)
    double rmin_layer = det::utils::tubeDimensions(ecells.Core().Cellid).x();
    double rmax_layer = det::utils::tubeDimensions(ecells.Core().Cellid).y();
    double r_cell = (rmin_layer+rmax_layer)/2.0;
   
    auto position =  m_segmentation->positionFromREtaPhi( r_cell, m_segmentation->eta(ecells.Core().Cellid), m_segmentation->phi(ecells.Core().Cellid) );
    newCluster.Core().position.X = position.x()*10.;
    newCluster.Core().position.Y = position.y()*10.;
    newCluster.Core().position.Z = position.z()*10.;
      
    m_decoder->setValue(ecells.Core().Cellid);
    
    if (ecells.Core().Energy>100.) {
      debug() << "energy " << ecells.Core().Energy << " decoder: all fields " << m_decoder->valueString() << endmsg;
    
      debug() << "energy " << ecells.Core().Energy << " eta " <<  m_segmentation->eta(ecells.Core().Cellid) << " phi " <<  m_segmentation->phi(ecells.Core().Cellid)<< " cellID " << ecells.Core().Cellid << endmsg;
    }
    
  }
   
  debug() << "Output Cluster collection size: " << edmClusterCollection->size() << endmsg;
  //Push the CaloHitCollection to event store
  m_caloClusters.put(edmClusterCollection);
 
  return StatusCode::SUCCESS;
}

StatusCode SaveCaloClusters::finalize() {  
  StatusCode sc = GaudiAlgorithm::finalize();
  return sc;
}
