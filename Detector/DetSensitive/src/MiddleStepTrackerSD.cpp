#include "DetSensitive/MiddleStepTrackerSD.h"

// FCCSW
#include "DetCommon/DetUtils.h"

// DD4hep
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

// Geant4
#include "G4SDManager.hh"

namespace det {
MiddleStepTrackerSD::MiddleStepTrackerSD(const std::string& aDetectorName,
                                         const std::string& aReadoutName,
                                         const DD4hep::Geometry::Segmentation& aSeg)
    : G4VSensitiveDetector(aDetectorName), m_trackerCollection(nullptr), m_seg(aSeg) {
  // name of the collection of hits is determined byt the readout name (from XML)
  collectionName.insert(aReadoutName);
}

MiddleStepTrackerSD::~MiddleStepTrackerSD() {}

void MiddleStepTrackerSD::Initialize(G4HCofThisEvent* aHitsCollections) {
  // create a collection of hits and add it to G4HCofThisEvent
  // deleted in ~G4Event
  m_trackerCollection = new G4THitsCollection<DD4hep::Simulation::Geant4Hit>(SensitiveDetectorName, collectionName[0]);
  aHitsCollections->AddHitsCollection(G4SDManager::GetSDMpointer()->GetCollectionID(m_trackerCollection),
                                      m_trackerCollection);
}

bool MiddleStepTrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
  // check if energy was deposited
  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep == 0.) return false;

  // as in DD4hep::Simulation::Geant4GenericSD<Tracker>
  CLHEP::Hep3Vector prePos = aStep->GetPreStepPoint()->GetPosition();
  CLHEP::Hep3Vector postPos = aStep->GetPostStepPoint()->GetPosition();
  CLHEP::Hep3Vector medPos = 0.5 * (prePos + postPos);
  // middle position between prestep and poststep is saved to cluster
  DD4hep::Simulation::Position position(medPos.x(), medPos.y(), medPos.z());
  CLHEP::Hep3Vector direction = postPos - prePos;
  // create a hit and add it to collection
  const G4Track* track = aStep->GetTrack();
  // deleted in ~G4Event
  auto hit = new DD4hep::Simulation::Geant4TrackerHit(
      track->GetTrackID(), track->GetDefinition()->GetPDGEncoding(), edep, track->GetGlobalTime());
  // take mid position between prestep and poststep to calculate the position in segmentation
  hit->cellID = utils::cellID(m_seg, *aStep, false);
  hit->energyDeposit = edep;
  hit->position = position;
  hit->momentum = direction;
  m_trackerCollection->insert(hit);
  return true;
}
}
