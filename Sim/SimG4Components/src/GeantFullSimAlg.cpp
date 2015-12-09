#include "GeantFullSimAlg.h"

// tmp
#include <fstream>

// FCCSW
#include "SimG4Common/ParticleInformation.h"
#include "SimG4Common/Units.h"
#include "SimG4Interface/IGeantSvc.h"
#include "SimG4Data/GeantTrackerHit.h"

// albers
#include "datamodel/MCParticleCollection.h"
#include "datamodel/TrackClusterCollection.h"

// Geant
#include "G4HCofThisEvent.hh"
#include "G4Event.hh"

DECLARE_ALGORITHM_FACTORY(sim::GeantFullSimAlg)

namespace sim {
GeantFullSimAlg::GeantFullSimAlg(const std::string& aName, ISvcLocator* aSvcLoc):
GaudiAlgorithm(aName, aSvcLoc) {
  declareInput("genParticles", m_genParticles);
  declareOutput("trackClusters", m_trackClusters);
}
GeantFullSimAlg::~GeantFullSimAlg() {}

StatusCode GeantFullSimAlg::initialize() {
  if (GaudiAlgorithm::initialize().isFailure())
    return StatusCode::FAILURE;
  if (service("GeantSvc", m_geantSvc, true).isFailure()) {
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
  m_geantSvc->processEvent(event);
  const G4Event* constevent;
  m_geantSvc->retrieveEvent(constevent);
  // here specify what is the output of interest
  SaveTrackerHits(constevent);
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

void GeantFullSimAlg::SaveTrackerHits(const G4Event* aEvent) {
  G4HCofThisEvent* collections = aEvent->GetHCofThisEvent();
  debug() << "     " << collections->GetNumberOfCollections()<< " Collections" << endmsg;
    info()<<collections->GetHC(0)->GetName()<<endmsg;
    info()<<collections->GetHC(0)->GetSDname()<<endmsg;
  GeantTrackerHitsCollection* track_collection;
  if(collections) {
    track_collection = dynamic_cast<GeantTrackerHitsCollection*>(collections->GetHC(0));
    info()<<collections->GetHC(0)->GetName()<<endmsg;
    info()<<collections->GetHC(0)->GetSDname()<<endmsg;
    if(track_collection) {
      // HERE: Save hits to the EDM?
      int n_hit = track_collection->GetSize();
      debug() << "     " << n_hit
              << " hits are stored in SaveHitsTrackerHitsCollection" << endmsg;
      // tmp solution
      std::ofstream file;
      CLHEP::Hep3Vector pos;
      file.open("tracker_hits.txt");
      for(auto iter_hit=0; iter_hit<n_hit; iter_hit++ ) {
        pos = dynamic_cast<GeantTrackerHit*>(track_collection->GetHit(iter_hit))->GetPosition();
        file<<pos.x()<<" "<<pos.y()<<" "<<pos.z()<<std::endl;
      }
      file.close();
    }
  }
}
}
