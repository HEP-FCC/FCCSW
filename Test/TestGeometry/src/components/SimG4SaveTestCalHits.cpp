#include "SimG4SaveTestCalHits.h"

// FCCSW
#include "SimG4Common/Units.h"
#include "TestGeometry/TestCalorimeterHit.h"

// Geant4
#include "G4Event.hh"

// datamodel
#include "datamodel/CaloClusterCollection.h"
#include "datamodel/CaloHitCollection.h"

DECLARE_TOOL_FACTORY(SimG4SaveTestCalHits)

SimG4SaveTestCalHits::SimG4SaveTestCalHits(const std::string& aType, const std::string& aName, const IInterface* aParent) :
  GaudiTool(aType, aName, aParent) {
  declareInterface<ISimG4SaveOutputTool>(this);
  declareProperty("caloType", m_calType);
  declareOutput("caloClusters", m_caloClusters,"hits/caloClusters");
  declareOutput("caloHits", m_caloHits,"hits/caloHits");
  // needed for AlgTool wit output/input until it appears in Gaudi AlgTool constructor
  declareProperty("DataInputs", inputDataObjects());
  declareProperty("DataOutputs", outputDataObjects());
}

SimG4SaveTestCalHits::~SimG4SaveTestCalHits() {}

StatusCode SimG4SaveTestCalHits::initialize() {
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

StatusCode SimG4SaveTestCalHits::finalize() {
  return GaudiTool::finalize();
}

StatusCode SimG4SaveTestCalHits::saveOutput(const G4Event& aEvent) {
  G4HCofThisEvent* collections = aEvent.GetHCofThisEvent();
  G4VHitsCollection* collect;
  test::TestCalorimeterHit* hit;
  double fCellNo = 11.;
  double energyTotal;
  int hitNo;
  if(collections != nullptr) {
    fcc::CaloClusterCollection* edmClusters = new fcc::CaloClusterCollection();
    fcc::CaloHitCollection* edmHits = new fcc::CaloHitCollection();
    for (int iter_coll=0; iter_coll<collections->GetNumberOfCollections(); iter_coll++) {
      collect = collections->GetHC(iter_coll);
      if (collect->GetName().find(m_calType) != std::string::npos) {
        size_t n_hit = collect->GetSize();
        energyTotal = 0;
        hitNo = 0;
        info() << "\t" << n_hit<< " hits are stored in a calorimeter collection #"<<iter_coll<<": "<<collect->GetName()<<endmsg;
        for(size_t iter_hit=0; iter_hit<n_hit; iter_hit++ ) {
          hit = dynamic_cast<test::TestCalorimeterHit*>(collect->GetHit(iter_hit));
          if(hit->GetXid() != -1 && hit->GetYid() != -1 && hit->GetZid() != -1) {
            fcc::CaloHit edmHit = edmHits->create();
            fcc::CaloCluster edmCluster = edmClusters->create();
            fcc::BareHit& edmHitCore = edmHit.Core();
            fcc::BareCluster& edmClusterCore = edmCluster.Core();
            edmHitCore.Cellid = fCellNo*fCellNo*hit->GetXid()+fCellNo*hit->GetYid()+hit->GetZid();
            edmHitCore.Energy = hit->GetEdep()*sim::g42edm::energy;
            if( edmHitCore.Energy!=0)
              hitNo++;

            edmClusterCore.position.X = hit->GetPos().x()*sim::g42edm::length;
            edmClusterCore.position.Y = hit->GetPos().y()*sim::g42edm::length;
            edmClusterCore.position.Z = hit->GetPos().z()*sim::g42edm::length;
            debug() <<"position of hit: " << hit->GetPos()<< "mm "<<endmsg;
            edmClusterCore.Energy = hit->GetEdep()*sim::g42edm::energy;
            energyTotal += edmClusterCore.Energy;
          }
        }
        debug() << "\t" << hitNo<< " hits are non-zero in collection #"<<iter_coll<<": "<<collect->GetName()<<endmsg;
        debug() << "\t" << edmClusters->size()<< " hits are stored in EDM"<<endmsg;
        debug() << "\t" << energyTotal<< " GeV = total energy stored"<<endmsg;
      }
    }
    m_caloClusters.put(edmClusters);
    m_caloHits.put(edmHits);
  }
  return StatusCode::SUCCESS;
}
