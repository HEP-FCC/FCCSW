#include "SimG4SaveTrackerHits.h"

// FCCSW
#include "DetCommon/Geant4PreDigiTrackHit.h"
#include "DetInterface/IGeoSvc.h"
#include "SimG4Common/Units.h"

// Geant4
#include "G4Event.hh"

// datamodel
#include "datamodel/DigiTrackHitAssociationCollection.h"
#include "datamodel/PositionedTrackHitCollection.h"
#include "datamodel/TrackHitCollection.h"

// DD4hep
#include "DD4hep/Detector.h"

DECLARE_TOOL_FACTORY(SimG4SaveTrackerHits)

SimG4SaveTrackerHits::SimG4SaveTrackerHits(const std::string& aType, const std::string& aName,
                                           const IInterface* aParent)
    : GaudiTool(aType, aName, aParent) {
  declareInterface<ISimG4SaveOutputTool>(this);
  declareProperty("positionedTrackHits", m_positionedTrackHits, "Handle for tracker hits");
  declareProperty("digiTrackHits", m_digiTrackHits, "Handle for digi tracker hits");
  declareProperty("trackHits", m_trackHits, "Handle for tracker hits including position information");
}

SimG4SaveTrackerHits::~SimG4SaveTrackerHits() {}

StatusCode SimG4SaveTrackerHits::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_geoSvc = service("GeoSvc");
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
  fcc::Geant4PreDigiTrackHit* hit;
  if (collections != nullptr) {
    fcc::PositionedTrackHitCollection* edmPositions = m_positionedTrackHits.createAndPut();
    fcc::TrackHitCollection* edmHits = m_trackHits.createAndPut();
    fcc::DigiTrackHitAssociationCollection* edmDigiHits = m_digiTrackHits.createAndPut();
    for (int iter_coll = 0; iter_coll < collections->GetNumberOfCollections(); iter_coll++) {
      collect = collections->GetHC(iter_coll);
      if (std::find(m_readoutNames.begin(), m_readoutNames.end(), collect->GetName()) != m_readoutNames.end()) {
        size_t n_hit = collect->GetSize();
        debug() << "\t" << n_hit << " hits are stored in a tracker collection #" << iter_coll << ": "
                << collect->GetName() << endmsg;
        for (size_t iter_hit = 0; iter_hit < n_hit; iter_hit++) {
          hit = dynamic_cast<fcc::Geant4PreDigiTrackHit*>(collect->GetHit(iter_hit));
          fcc::TrackHit edmHit = edmHits->create();
          fcc::BareHit& edmHitCore = edmHit.core();
          fcc::DigiTrackHitAssociation edmDigiHit = edmDigiHits->create();
          edmHitCore.cellId = hit->cellID;
          edmHitCore.energy = hit->energyDeposit * sim::g42edm::energy;
          edmHitCore.bits = hit->trackId;
          edmHitCore.time = hit->time;
          fcc::Point preStepPosition = fcc::Point();
          preStepPosition.x = hit->prePos.x() * sim::g42edm::length;
          preStepPosition.y = hit->prePos.y() * sim::g42edm::length;
          preStepPosition.z = hit->prePos.z() * sim::g42edm::length;
          fcc::Point postStepPosition = fcc::Point();
          postStepPosition.x = hit->postPos.x() * sim::g42edm::length;
          postStepPosition.y = hit->postPos.y() * sim::g42edm::length;
          postStepPosition.z = hit->postPos.z() * sim::g42edm::length;

          fcc::PositionedTrackHit edmPositionedHit = edmPositions->create(preStepPosition, edmHitCore);
          edmDigiHit.postStepPosition(postStepPosition);
          edmDigiHit.hit(edmPositionedHit);
        }
      }
    }
  }
  return StatusCode::SUCCESS;
}
