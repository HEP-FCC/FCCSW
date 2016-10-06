#include "SimG4SaveCalHits.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// Geant4
#include "G4Event.hh"

// datamodel
#include "datamodel/CaloClusterCollection.h"
#include "datamodel/CaloHitCollection.h"

// DD4hep
#include "DDG4/Geant4Hits.h"

DECLARE_TOOL_FACTORY(SimG4SaveCalHits)

SimG4SaveCalHits::SimG4SaveCalHits(const std::string& aType, const std::string& aName, const IInterface* aParent) :
  GaudiTool(aType, aName, aParent) {
  declareInterface<ISimG4SaveOutputTool>(this);
  declareProperty("readoutNames", m_readoutNames);
  declareOutput("caloClusters", m_caloClusters,"hits/caloClusters");
  declareOutput("caloHits", m_caloHits,"hits/caloHits");
  // needed for AlgTool wit output/input until it appears in Gaudi AlgTool constructor
  declareProperty("DataInputs", inputDataObjects());
  declareProperty("DataOutputs", outputDataObjects());
}

SimG4SaveCalHits::~SimG4SaveCalHits() {}

StatusCode SimG4SaveCalHits::initialize() {
  if(GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_geoSvc = service ("GeoSvc");
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  auto lcdd = m_geoSvc->lcdd();
  auto allReadouts = lcdd->readouts();
  for(auto& readoutName: m_readoutNames) {
    if(allReadouts.find(readoutName) == allReadouts.end()) {
      error()<<"Readout "<<readoutName<<" not found! Please check tool configuration."<<endmsg;
      return StatusCode::FAILURE;
    } else {
      debug()<<"Hits will be saved to EDM from the collection "<<readoutName<<endmsg;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4SaveCalHits::finalize() {
  return GaudiTool::finalize();
}

StatusCode SimG4SaveCalHits::saveOutput(const G4Event& aEvent) {
  G4HCofThisEvent* collections = aEvent.GetHCofThisEvent();
  G4VHitsCollection* collect;
  DD4hep::Simulation::Geant4CalorimeterHit* hit;
  if(collections != nullptr) {
    fcc::CaloClusterCollection* edmClusters = new fcc::CaloClusterCollection();
    fcc::CaloHitCollection* edmHits = new fcc::CaloHitCollection();
    for (int iter_coll=0; iter_coll<collections->GetNumberOfCollections(); iter_coll++) {
      collect = collections->GetHC(iter_coll);
      if (std::find(m_readoutNames.begin(), m_readoutNames.end(), collect->GetName()) != m_readoutNames.end()) {
        size_t n_hit = collect->GetSize();
        debug() << "\t" << n_hit<< " hits are stored in a collection #"<<iter_coll<<": "<<collect->GetName()<<endmsg;
        for(size_t iter_hit=0; iter_hit<n_hit; iter_hit++ ) {
          hit = dynamic_cast<DD4hep::Simulation::Geant4CalorimeterHit*>(collect->GetHit(iter_hit));
          fcc::CaloHit edmHit = edmHits->create();
          fcc::CaloCluster edmCluster = edmClusters->create();
          fcc::BareHit& edmHitCore = edmHit.Core();
          fcc::BareCluster& edmClusterCore = edmCluster.Core();
          edmHitCore.Cellid = hit->cellID;
          edmHitCore.Energy = hit->energyDeposit;
          edmClusterCore.position.X = hit->position.x();
          edmClusterCore.position.Y = hit->position.y();
          edmClusterCore.position.Z = hit->position.z();
          edmClusterCore.Bits = hit->cellID;
          edmClusterCore.Energy = hit->energyDeposit;
        }
      }
    }
    m_caloClusters.put(edmClusters);
    m_caloHits.put(edmHits);
  }
  return StatusCode::SUCCESS;
}
