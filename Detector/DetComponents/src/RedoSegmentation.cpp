#include "RedoSegmentation.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// datamodel
#include "datamodel/CaloHitCollection.h"
#include "datamodel/CaloClusterCollection.h"

// DD4hep
#include "DD4hep/LCDD.h"
#include "DDSegmentation/CartesianGridXYZ.h"
#include "DDSegmentation/Segmentation.h"

DECLARE_ALGORITHM_FACTORY(RedoSegmentation)

RedoSegmentation::RedoSegmentation(const std::string& aName, ISvcLocator* aSvcLoc):
GaudiAlgorithm(aName, aSvcLoc)
{
  declareInput("inclusters", m_inClusters,"hits/caloInClusters");
  declareOutput("outhits", m_outHits,"hits/caloOutHits");
  declareProperty("oldReadoutName", m_oldReadoutName = "");
  declareProperty("oldSegmentationIds", m_oldIdentifiers);
  declareProperty("newReadoutName", m_newReadoutName = "");
}

RedoSegmentation::~RedoSegmentation() {}

StatusCode RedoSegmentation::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;
  m_geoSvc = service ("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  // check if readouts exist
  if(m_geoSvc->lcdd()->readouts().find(m_oldReadoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error()<<"Readout <<"<<m_oldReadoutName<<">> does not exist."<<endmsg;
    return StatusCode::FAILURE;
  }
  if(m_geoSvc->lcdd()->readouts().find(m_newReadoutName) == m_geoSvc->lcdd()->readouts().end()) {
    error()<<"Readout <<"<<m_newReadoutName<<">> does not exist."<<endmsg;
    return StatusCode::FAILURE;
  }
  // Take readout, bitfield from GeoSvc
  m_oldDecoder = std::shared_ptr<DD4hep::DDSegmentation::BitField64>(
    m_geoSvc->lcdd()->readout(m_oldReadoutName).idSpec().decoder());
  // segmentation identifiers to be overwritten
  if(m_oldIdentifiers.size()==0) {
    // it is not an error, maybe no segmentation was used previously
    warning()<<"No previous segmentation identifiers. Volume ID may be recomputed incorrectly."<<endmsg;
  }
  // create detector identifiers (= all bitfield ids - segmentation ids)
  for(uint itField = 0; itField < m_oldDecoder->size(); itField++)
  {
    std::string field =  (*m_oldDecoder)[itField].name();
    auto iter = std::find( m_oldIdentifiers.begin(), m_oldIdentifiers.end(), field );
    if( iter == m_oldIdentifiers.end() )
    {
      m_detectorIdentifiers.push_back(field);
    }
  }
  // Take new segmentation from geometry service
  m_segmentation = m_geoSvc->lcdd()->readout(m_newReadoutName).segmentation().segmentation();
  // check if detector identifiers (old and new) agree
  std::vector<std::string> newFields;
  for(uint itField = 0; itField < m_segmentation->decoder()->size(); itField++) {
    newFields.push_back((*m_segmentation->decoder())[itField].name());
  }
  for(const auto& detectorField: m_detectorIdentifiers) {
    auto iter = std::find( newFields.begin(), newFields.end(), detectorField );
    if( iter == newFields.end() ) {
      error()<<"New readout does not contain field <<"<<detectorField<<">> that describes the detector ID."<<endmsg;
      return StatusCode::FAILURE;
       }
  }
  info()<<"Redoing the segmentation."<<endmsg;
  info()<<"Old bitfield:\t"<<m_oldDecoder->fieldDescription()<<endmsg;
  info()<<"New bitfield:\t"<<m_segmentation->decoder()->fieldDescription()<<endmsg;
  info()<<"New segmentation is of type:\t"<<m_segmentation->type()<<endmsg;

  return StatusCode::SUCCESS;
}

StatusCode RedoSegmentation::execute() {
  const fcc::CaloClusterCollection* inClusters = m_inClusters.get();
  fcc::CaloHitCollection* outHits = new fcc::CaloHitCollection();
  // loop over clusters to get the energy deposits: position and cellID
  // cellID contains the volumeID that needs to be rewritten
  int oldid;
  for(const auto& cluster: *inClusters) {
    fcc::CaloHit newHit = outHits->create();
    newHit.Core().Energy = cluster.Core().Energy;
    newHit.Core().Time = cluster.Core().Time;
    m_oldDecoder->setValue(cluster.Core().Bits);
    debug()<<"OLD: "<<m_oldDecoder->valueString()<<endmsg;
    DD4hep::DDSegmentation::Vector3D position(cluster.Core().position.X/10, cluster.Core().position.Y/10, cluster.Core().position.Z/10); // mm to cm
    // first calculate proper segmentation fields
    uint64_t newcellId = m_segmentation->cellID(position,position,volumeID(cluster.Core().Bits));
    m_segmentation->decoder()->setValue(newcellId);
    // now rewrite all other fields (detector ID)
    for(const auto& detectorField: m_detectorIdentifiers) {
      oldid = (*m_oldDecoder)[detectorField];
      (*m_segmentation->decoder())[detectorField] = oldid;
    }
    newHit.Core().Cellid =  m_segmentation->decoder()->getValue();
    debug()<<"NEW: "<<m_segmentation->decoder()->valueString()<<endmsg;
  }
  m_outHits.put(outHits);

  return StatusCode::SUCCESS;
}

StatusCode RedoSegmentation::finalize() {
  return GaudiAlgorithm::finalize();
}

uint64_t RedoSegmentation::volumeID(uint64_t aCellId) const {
  m_oldDecoder->setValue(aCellId);
  for (const auto& identifier: m_oldIdentifiers) {
    (*m_oldDecoder)[identifier] = 0;
  }
  return m_oldDecoder->getValue();
}
