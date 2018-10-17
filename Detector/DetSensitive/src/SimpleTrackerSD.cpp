#include "DetSensitive/SimpleTrackerSD.h"

// FCCSW
#include "DetCommon/DetUtils.h"

// DD4hep
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

// Geant4
#include "G4SDManager.hh"

#include "DetCommon/Geant4PreDigiTrackHit.h"

namespace det {
SimpleTrackerSD::SimpleTrackerSD(const std::string& aDetectorName,
                                 const std::string& aReadoutName,
                                 const dd4hep::Segmentation& aSeg)
    : G4VSensitiveDetector(aDetectorName), m_trackerCollection(nullptr), m_seg(aSeg) {
  // name of the collection of hits is determined byt the readout name (from XML)
  collectionName.insert(aReadoutName);
}

SimpleTrackerSD::~SimpleTrackerSD() {}

void SimpleTrackerSD::Initialize(G4HCofThisEvent* aHitsCollections) {
  // create a collection of hits and add it to G4HCofThisEvent
  // deleted in ~G4Event
  m_trackerCollection = new G4THitsCollection<fcc::Geant4PreDigiTrackHit>(SensitiveDetectorName, collectionName[0]);
  aHitsCollections->AddHitsCollection(G4SDManager::GetSDMpointer()->GetCollectionID(m_trackerCollection),
                                      m_trackerCollection);
}

bool SimpleTrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
  // check if energy was deposited
  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep == 0.) return false;
  // get track
  const G4Track* track = aStep->GetTrack();
  CLHEP::Hep3Vector prePos = aStep->GetPreStepPoint()->GetPosition();
  CLHEP::Hep3Vector postPos = aStep->GetPostStepPoint()->GetPosition();
  // create a hit and add it to collection
  // deleted in ~G4Event
  auto hit = new fcc::Geant4PreDigiTrackHit(
      track->GetTrackID(), track->GetDefinition()->GetPDGEncoding(), edep, track->GetGlobalTime());
  hit->cellID = utils::cellID(m_seg, *aStep);
  hit->prePos = prePos;
  hit->postPos = postPos;
  m_trackerCollection->insert(hit);
  return true;
}
}
