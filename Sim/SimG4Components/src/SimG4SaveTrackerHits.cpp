#include "SimG4SaveTrackerHits.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"

// Geant4
#include "G4Event.hh"

// datamodel
#include "datamodel/TrackClusterCollection.h"
#include "datamodel/TrackHitCollection.h"
#include "datamodel/TrackClusterHitsAssociationCollection.h"

// DD4hep
#include "DDG4/Geant4Hits.h"

DECLARE_TOOL_FACTORY(SimG4SaveTrackerHits)

SimG4SaveTrackerHits::SimG4SaveTrackerHits(const std::string& aType, const std::string& aName, const IInterface* aParent) :
  GaudiTool(aType, aName, aParent) {
  declareInterface<ISimG4SaveOutputTool>(this);
  declareProperty("readoutNames", m_readoutNames);
  declareOutput("trackClusters", m_trackClusters,"hits/trackerClusters");
  declareOutput("trackHits", m_trackHits,"hits/trackerHits");
  declareOutput("trackHitsClusters", m_trackHitsClusters,"hits/trackerAssociations");
  // needed for AlgTool wit output/input until it appears in Gaudi AlgTool constructor
  declareProperty("DataInputs", inputDataObjects());
  declareProperty("DataOutputs", outputDataObjects());
}

SimG4SaveTrackerHits::~SimG4SaveTrackerHits() {}

StatusCode SimG4SaveTrackerHits::initialize() {
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

StatusCode SimG4SaveTrackerHits::finalize() {
  return GaudiTool::finalize();
}

StatusCode SimG4SaveTrackerHits::saveOutput(const G4Event& aEvent) {
  G4HCofThisEvent* collections = aEvent.GetHCofThisEvent();
  G4VHitsCollection* collect;
  DD4hep::Simulation::Geant4TrackerHit* hit;
  if(collections != nullptr) {
    fcc::TrackClusterCollection* edmClusters = new fcc::TrackClusterCollection();
    fcc::TrackHitCollection* edmHits = new fcc::TrackHitCollection();
    fcc::TrackClusterHitsAssociationCollection* edmAssociations = new fcc::TrackClusterHitsAssociationCollection();
    for (int iter_coll=0; iter_coll<collections->GetNumberOfCollections(); iter_coll++) {
      collect = collections->GetHC(iter_coll);
      if (std::find(m_readoutNames.begin(), m_readoutNames.end(), collect->GetName()) != m_readoutNames.end()) {
        size_t n_hit = collect->GetSize();
        info() << "\t" << n_hit<< " hits are stored in a tracker collection #"<<iter_coll<<": "<<collect->GetName()<<endmsg;
        for(size_t iter_hit=0; iter_hit<n_hit; iter_hit++ ) {
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
    }
    m_trackClusters.put(edmClusters);
    m_trackHits.put(edmHits);
    m_trackHitsClusters.put(edmAssociations);
  }
  return StatusCode::SUCCESS;
}
