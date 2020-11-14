#include "SimG4SaveTrackerHits.h"

// FCCSW
#include "DetInterface/IGeoSvc.h"
#include "SimG4Common/Units.h"
#include "DetCommon/Geant4PreDigiTrackHit.h"

// Geant4
#include "G4Event.hh"

// datamodel
#include "edm4hep/SimTrackerHitCollection.h"

// DD4hep
#include "DD4hep/Detector.h"


DECLARE_COMPONENT(SimG4SaveTrackerHits)

SimG4SaveTrackerHits::SimG4SaveTrackerHits(const std::string& aType, const std::string& aName,
                                           const IInterface* aParent)
    : GaudiTool(aType, aName, aParent),
      m_geoSvc("GeoSvc", aName)
    {
  declareInterface<ISimG4SaveOutputTool>(this);
  declareProperty("SimTrackHits", m_positionedTrackHits, "Handle for tracker hits");
  declareProperty("GeoSvc", m_geoSvc);
}

SimG4SaveTrackerHits::~SimG4SaveTrackerHits() {}

StatusCode SimG4SaveTrackerHits::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service. "
            << "Make sure you have GeoSvc and SimSvc in the right order in the configuration." << endmsg;
    return StatusCode::FAILURE;
  }
  auto lcdd = m_geoSvc->lcdd();
  auto allReadouts = lcdd->readouts();
  for (auto& readoutName : m_readoutNames) {
    if (allReadouts.find(readoutName) == allReadouts.end()) {
      error() << "Readout " << readoutName << " not found! Please check tool configuration." << endmsg;
      return StatusCode::FAILURE;
    } else {
      debug() << "Hits will be saved to EDM from the collection " << readoutName << endmsg;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4SaveTrackerHits::finalize() { return GaudiTool::finalize(); }

StatusCode SimG4SaveTrackerHits::saveOutput(const G4Event& aEvent) {
  G4HCofThisEvent* collections = aEvent.GetHCofThisEvent();
  G4VHitsCollection* collect;
  edm4hep::Geant4PreDigiTrackHit* hit;
  if (collections != nullptr) {
    edm4hep::SimTrackerHitCollection* edmHits = m_positionedTrackHits.createAndPut();
    for (int iter_coll = 0; iter_coll < collections->GetNumberOfCollections(); iter_coll++) {
      collect = collections->GetHC(iter_coll);
      if (std::find(m_readoutNames.begin(), m_readoutNames.end(), collect->GetName()) != m_readoutNames.end()) {
        size_t n_hit = collect->GetSize();
        verbose() << "\t" << n_hit << " hits are stored in a tracker collection #" << iter_coll << ": "
               << collect->GetName() << endmsg;
        for (size_t iter_hit = 0; iter_hit < n_hit; iter_hit++) {
          hit = dynamic_cast<edm4hep::Geant4PreDigiTrackHit*>(collect->GetHit(iter_hit));
          edm4hep::SimTrackerHit edmHit = edmHits->create();
          edmHit.setCellID(hit->cellID);
          edmHit.setEDep(hit->energyDeposit * sim::g42edm::energy);
          /// workaround, store trackid in an unrelated field
          edmHit.setQuality(hit->trackId);
          edmHit.setTime(hit->time);
          edmHit.setPosition({
                              hit->prePos.x() * sim::g42edm::length,
                              hit->prePos.y() * sim::g42edm::length,
                              hit->prePos.z() * sim::g42edm::length,
          });
          CLHEP::Hep3Vector diff = hit->postPos - hit->prePos;
          edmHit.setMomentum({
                               diff.x() * sim::g42edm::length,
                               diff.y() * sim::g42edm::length,
                               diff.z() * sim::g42edm::length,
          });
          edmHit.setPathLength(diff.mag());

          
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}
