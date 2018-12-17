#include "DetSensitive/SimpleDriftChamber.h"

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
SimpleDriftChamber::SimpleDriftChamber(const std::string& aDetectorName,
                                       const std::string& aReadoutName,
                                       const dd4hep::Segmentation& aSeg)
    : G4VSensitiveDetector(aDetectorName), m_driftChamberCollection(nullptr), m_seg(aSeg) {
  // name of the collection of hits is determined byt the readout name (from XML)
  collectionName.insert(aReadoutName);
}

SimpleDriftChamber::~SimpleDriftChamber() {}

void SimpleDriftChamber::Initialize(G4HCofThisEvent* aHitsCollections) {
  // create a collection of hits and add it to G4HCofThisEvent
  // deleted in ~G4Event
  m_driftChamberCollection =
      new G4THitsCollection<fcc::Geant4PreDigiTrackHit>(SensitiveDetectorName, collectionName[0]);
  aHitsCollections->AddHitsCollection(G4SDManager::GetSDMpointer()->GetCollectionID(m_driftChamberCollection),
                                      m_driftChamberCollection);
}

bool SimpleDriftChamber::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
  // check if energy was deposited
  G4double edep = aStep->GetTotalEnergyDeposit();
  G4double stepLength = aStep->GetStepLength();

  // cuts on the Edep and the G4 step length 
  if (edep < m_edepCut || stepLength < m_stepLengthCut) {
    return false;
  }

  // get track
  const G4Track* track = aStep->GetTrack();

  CLHEP::Hep3Vector prePos = aStep->GetPreStepPoint()->GetPosition();
  CLHEP::Hep3Vector postPos = aStep->GetPostStepPoint()->GetPosition();

  auto hit = new fcc::Geant4PreDigiTrackHit(
      track->GetTrackID(), track->GetDefinition()->GetPDGEncoding(), edep, track->GetGlobalTime());

  hit->cellID = utils::cellID(m_seg, *aStep);
  hit->energyDeposit = edep;
  hit->prePos = prePos;
  hit->postPos = postPos;
  m_driftChamberCollection->insert(hit);
  return true;
}
}
