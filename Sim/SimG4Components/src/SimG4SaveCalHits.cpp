#include "SimG4SaveCalHits.h"

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
  declareProperty("caloType", m_calType);
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
  if (m_calType.find("ECal") != std::string::npos && m_calType.find("HCal") != std::string::npos) {
    error()<<"Wrong type of the calorimeter. Set the property 'caloType' to either 'ECal' or 'HCal'"<<endmsg;
    return StatusCode::FAILURE;
  } else {
    info()<<"Initializing a tool saving the outputs for the calorimeter type: "<<m_calType<<endmsg;
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
    // CaloClusterHitsAssociationCollection* edmAssociations = new CaloClusterHitsAssociationCollection();
    for (int iter_coll=0; iter_coll<collections->GetNumberOfCollections(); iter_coll++) {
      collect = collections->GetHC(iter_coll);
      if (collect->GetName().find(m_calType) != std::string::npos) {
        size_t n_hit = collect->GetSize();
        info() << "\t" << n_hit<< " hits are stored in a HCal collection #"<<iter_coll<<": "<<collect->GetName()<<endmsg;
        for(size_t iter_hit=0; iter_hit<n_hit; iter_hit++ ) {
          hit = dynamic_cast<DD4hep::Simulation::Geant4CalorimeterHit*>(collect->GetHit(iter_hit));
          debug() << hit->cellID << " ";
          debug() << hit->energyDeposit << " ";

          debug() << hit->position.x() << " ";
          debug() << hit->position.y() << " ";
          debug() << hit->position.z() << endmsg;

          fcc::CaloHit edmHit = edmHits->create();
          fcc::CaloCluster edmCluster = edmClusters->create();
          fcc::BareHit& edmHitCore = edmHit.Core();
          fcc::BareCluster& edmClusterCore = edmCluster.Core();
          edmHitCore.Cellid = hit->cellID;
          edmHitCore.Energy = hit->energyDeposit;

          edmClusterCore.position.X = hit->position.x();
          edmClusterCore.position.Y = hit->position.y();
          edmClusterCore.position.Z = hit->position.z();
          edmClusterCore.Energy = hit->energyDeposit;

          // CaloClusterHitsAssociationHandle edmAssociation = edmAssociations->create();
          // edmAssociation.mod().Cluster = edmCluster;
          // edmAssociation.mod().Hit = edmHit;
        }
      }
    }
    m_caloClusters.put(edmClusters);
    m_caloHits.put(edmHits);
  }
  return StatusCode::SUCCESS;
}
