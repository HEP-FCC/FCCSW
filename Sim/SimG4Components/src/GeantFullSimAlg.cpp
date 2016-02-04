#include <iostream>
#include "GeantFullSimAlg.h"

// FCCSW
#include "SimG4Common/Units.h"
#include "SimG4Interface/IGeantSvc.h"

// albers
#include "datamodel/MCParticleCollection.h"
#include "datamodel/TrackClusterCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackClusterHitsAssociationCollection.h"
#include "datamodel/CaloClusterCollection.h"
#include "datamodel/CaloHitCollection.h"
#include "datamodel/GenVertexCollection.h"

// Geant
#include "G4HCofThisEvent.hh"
#include "G4Event.hh"

// DD4hep
#include "DDG4/Geant4Hits.h"

DECLARE_ALGORITHM_FACTORY(GeantFullSimAlg)

GeantFullSimAlg::GeantFullSimAlg(const std::string& aName, ISvcLocator* aSvcLoc):
GaudiAlgorithm(aName, aSvcLoc) {
  declareInput("genParticles", m_genParticles);
  declareOutput("trackClusters", m_trackClusters);
  declareOutput("trackHits", m_trackHits);
  declareOutput("trackHitsClusters", m_trackHitsClusters);
  declareOutput("caloClusters", m_caloClusters);
  declareOutput("caloHits", m_caloHits);
}
GeantFullSimAlg::~GeantFullSimAlg() {}

StatusCode GeantFullSimAlg::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;
  m_geantSvc = service("GeantSvc");
  if (!m_geantSvc) {
    error() << "Unable to locate Geant Simulation Service" << endmsg;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode GeantFullSimAlg::execute() {
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
  saveTrackerHits(*constevent);
  m_geantSvc->terminateEvent();
  return StatusCode::SUCCESS;
}

StatusCode GeantFullSimAlg::finalize() {
  return GaudiAlgorithm::finalize();
}

G4Event* GeantFullSimAlg::EDM2G4() {
  // Event will be passed to G4RunManager and be deleted in G4RunManager::RunTermination()
  G4Event* g4_event = new G4Event();
  // Creating EDM collections
  const fcc::MCParticleCollection* mcparticles = m_genParticles.get();
  // Adding one particle per one vertex => vertices repeated
  for(const auto& mcparticle : *mcparticles) {
    const fcc::ConstGenVertex& v = mcparticle.StartVertex();
    G4PrimaryVertex* g4_vertex = new G4PrimaryVertex
      (v.Position().X*sim::edm2g4::length, v.Position().Y*sim::edm2g4::length, v.Position().Z*sim::edm2g4::length, v.Ctau()*sim::edm2g4::length);
    const fcc::BareParticle& mccore = mcparticle.Core();
    G4PrimaryParticle* g4_particle = new G4PrimaryParticle
      (mccore.Type, mccore.P4.Px*sim::edm2g4::energy, mccore.P4.Py*sim::edm2g4::energy, mccore.P4.Pz*sim::edm2g4::energy);
    g4_vertex->SetPrimary(g4_particle);
    g4_event->AddPrimaryVertex(g4_vertex);
  }
  return g4_event;
}

void GeantFullSimAlg::saveTrackerHits(const G4Event& aEvent) {
  G4HCofThisEvent* collections = aEvent.GetHCofThisEvent();
  G4VHitsCollection* collect;
  DD4hep::Simulation::Geant4TrackerHit* hit;
  if(collections != nullptr) {
    fcc::TrackClusterCollection* edmClusters = new fcc::TrackClusterCollection();
    fcc::TrackHitCollection* edmHits = new fcc::TrackHitCollection();
    fcc::TrackClusterHitsAssociationCollection* edmAssociations = new fcc::TrackClusterHitsAssociationCollection();
    for (int iter_coll=0; iter_coll<collections->GetNumberOfCollections(); iter_coll++) {
      collect = collections->GetHC(iter_coll);
      if (collect->GetName().operator==("BarHCal_Readout"))
        continue;
      // HERE: check if it's tracker coll
      int n_hit = collect->GetSize();
      debug() << "     " << n_hit<< " hits are stored in collection #"<<iter_coll<<endmsg;
      for(auto iter_hit=0; iter_hit<n_hit; iter_hit++ ) {
        hit = dynamic_cast<DD4hep::Simulation::Geant4TrackerHit*>(collect->GetHit(iter_hit));
        fcc::TrackHit edmHit = edmHits->create();
        fcc::TrackCluster edmCluster = edmClusters->create();
        fcc::BareHit& edmHitCore = edmHit.Core();
        fcc::BareCluster& edmClusterCore = edmCluster.Core();
        edmHitCore.Cellid = hit->cellID;
        edmHitCore.Energy = hit->energyDeposit;
        edmHitCore.Time = hit->truth.time;
        edmClusterCore.position.X = hit->position.x();
        edmClusterCore.position.Y = hit->position.y();
        edmClusterCore.position.Z = hit->position.z();
        edmClusterCore.Energy = hit->energyDeposit;
        edmClusterCore.Time = hit->truth.time;
        fcc::TrackClusterHitsAssociation edmAssociation = edmAssociations->create();
        edmAssociation.Cluster(edmCluster);
        edmAssociation.Hit(edmHit);
      }
    }
    m_trackClusters.put(edmClusters);
    m_trackHits.put(edmHits);
    m_trackHitsClusters.put(edmAssociations);
  }
}

void GeantFullSimAlg::saveHCalDeposits(const G4Event& aEvent) {
  // G4HCofThisEvent* collections = aEvent->GetHCofThisEvent();
  G4HCofThisEvent* collections = aEvent.GetHCofThisEvent();
  G4VHitsCollection* collect;
  DD4hep::Simulation::Geant4CalorimeterHit* hit;
  if(collections != nullptr) {
    fcc::CaloClusterCollection* edmClusters = new fcc::CaloClusterCollection();
    fcc::CaloHitCollection* edmHits = new fcc::CaloHitCollection();
    // CaloClusterHitsAssociationCollection* edmAssociations = new CaloClusterHitsAssociationCollection();
    for (int iter_coll=0; iter_coll<collections->GetNumberOfCollections(); iter_coll++) {
      collect = collections->GetHC(iter_coll);
      if (collect->GetName().operator==("BarHCal_Readout")) {
        unsigned int n_hit = collect->GetSize();
        debug() << "     " << n_hit<< " hits are stored in collection #"<<iter_coll<<endmsg;
        for(auto iter_hit=0; iter_hit<n_hit; iter_hit++ ) {
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
}

