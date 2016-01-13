#include "G4FullSimAlg.h"

// FCCSW
#include "SimG4Common/Units.h"
#include "SimG4Interface/IG4SimSvc.h"

// albers
#include "datamodel/MCParticleCollection.h"
#include "datamodel/CaloClusterCollection.h"
#include "datamodel/CaloHitCollection.h"

// Geant
#include "G4HCofThisEvent.hh"
#include "G4Event.hh"

// DD4hep
#include "DDG4/Geant4Hits.h"

DECLARE_ALGORITHM_FACTORY(G4FullSimAlg)

G4FullSimAlg::G4FullSimAlg(const std::string& aName, ISvcLocator* aSvcLoc):
GaudiAlgorithm(aName, aSvcLoc) {
  declareInput("genParticles", m_genParticles);
  declareOutput("caloClusters", m_caloClusters);
  declareOutput("caloHits", m_caloHits);
  declareProperty("outputs",m_saveTool);
  declarePrivateTool(m_saveTool);
}
G4FullSimAlg::~G4FullSimAlg() {}

StatusCode G4FullSimAlg::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;
  m_geantSvc = service("G4SimSvc");
  if (!m_geantSvc) {
    error() << "Unable to locate Geant Simulation Service" << endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_saveTool.retrieve()) {
    error() << "Unable to retrieve the output saving tool." << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode G4FullSimAlg::execute() {
  // first translate the event
  G4Event* event = EDM2G4();
  if ( !event ) {
    error() << "Unable to translate EDM MC data to G4Event" << endmsg;
    return StatusCode::FAILURE;
  }
  m_geantSvc->processEvent(*event);
  G4Event* constevent;
  m_geantSvc->retrieveEvent(constevent);
  // here specify what is the output of interest
  saveHCalDeposits(*constevent);
  m_saveTool->saveOutput(*constevent);
  m_geantSvc->terminateEvent();
  return StatusCode::SUCCESS;
}

StatusCode G4FullSimAlg::finalize() {
  return GaudiAlgorithm::finalize();
}

G4Event* G4FullSimAlg::EDM2G4() {
  // Event will be passed to G4RunManager and be deleted in G4RunManager::RunTermination()
  G4Event* g4_event = new G4Event();
  // Creating EDM collections
  const MCParticleCollection* mcparticles = m_genParticles.get();
  // Adding one particle per one vertex => vertices repeated
  for(const auto& mcparticle : *mcparticles) {
    const GenVertex& v = mcparticle.read().StartVertex.read();
    G4PrimaryVertex* g4_vertex = new G4PrimaryVertex
      (v.Position.X*sim::edm2g4::length, v.Position.Y*sim::edm2g4::length, v.Position.Z*sim::edm2g4::length, v.Ctau*sim::edm2g4::length);
    const BareParticle& mccore = mcparticle.read().Core;
    G4PrimaryParticle* g4_particle = new G4PrimaryParticle
      (mccore.Type, mccore.P4.Px*sim::edm2g4::energy, mccore.P4.Py*sim::edm2g4::energy, mccore.P4.Pz*sim::edm2g4::energy);
    g4_vertex->SetPrimary(g4_particle);
    g4_event->AddPrimaryVertex(g4_vertex);
  }
  return g4_event;
}

void G4FullSimAlg::saveHCalDeposits(const G4Event& aEvent) {
  G4HCofThisEvent* collections = aEvent.GetHCofThisEvent();
  G4VHitsCollection* collect;
  DD4hep::Simulation::Geant4CalorimeterHit* hit;
  if(collections != nullptr) {
    CaloClusterCollection* edmClusters = new CaloClusterCollection();
    CaloHitCollection* edmHits = new CaloHitCollection();
    // CaloClusterHitsAssociationCollection* edmAssociations = new CaloClusterHitsAssociationCollection();
    for (int iter_coll=0; iter_coll<collections->GetNumberOfCollections(); iter_coll++) {
      collect = collections->GetHC(iter_coll);
      if (collect->GetName().find("HCal") != std::string::npos) {
        unsigned int n_hit = collect->GetSize();
        info() << "\t" << n_hit<< " hits are stored in a HCal collection #"<<iter_coll<<": "<<collect->GetName()<<endmsg;
        for(auto iter_hit=0; iter_hit<n_hit; iter_hit++ ) {
          hit = dynamic_cast<DD4hep::Simulation::Geant4CalorimeterHit*>(collect->GetHit(iter_hit));
          debug() << hit->cellID << " ";
          debug() << hit->energyDeposit << " ";

          debug() << hit->position.x() << " ";
          debug() << hit->position.y() << " ";
          debug() << hit->position.z() << endmsg;

          CaloHitHandle edmHit = edmHits->create();
          CaloClusterHandle edmCluster = edmClusters->create();
          BareHit& edmHitCore = edmHit.mod().Core;
          BareCluster& edmClusterCore = edmCluster.mod().Core;
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
}

