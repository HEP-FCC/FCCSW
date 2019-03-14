#include "DetSensitive/SimpleCalorimeterSD.h"

// FCCSW
#include "DetCommon/DetUtils.h"
#include "DetCommon/Geant4CaloHit.h"

// DD4hep
#include "DDG4/Defs.h"
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

// Geant4
#include "G4SDManager.hh"

namespace det {
SimpleCalorimeterSD::SimpleCalorimeterSD(const std::string& aDetectorName,
                                         const std::string& aReadoutName,
                                         const dd4hep::Segmentation& aSeg)
    : G4VSensitiveDetector(aDetectorName), m_calorimeterCollection(nullptr), m_seg(aSeg) {
  // name of the collection of hits is determined byt the readout name (from XML)
  collectionName.insert(aReadoutName);
}

SimpleCalorimeterSD::~SimpleCalorimeterSD() {}

void SimpleCalorimeterSD::Initialize(G4HCofThisEvent* aHitsCollections) {
  // create a collection of hits and add it to G4HCofThisEvent
  // deleted in ~G4Event
  m_calorimeterCollection = new G4THitsCollection<fcc::Geant4CaloHit>(SensitiveDetectorName, collectionName[0]);
  aHitsCollections->AddHitsCollection(G4SDManager::GetSDMpointer()->GetCollectionID(m_calorimeterCollection),
                                      m_calorimeterCollection);
}

bool SimpleCalorimeterSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
  // check if energy was deposited
  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep == 0.) return false;

  // as in dd4hep::sim::Geant4GenericSD<Calorimeter>
  const G4Track* track = aStep->GetTrack();
  auto hit = new fcc::Geant4CaloHit(
      track->GetTrackID(), track->GetDefinition()->GetPDGEncoding(), edep, track->GetGlobalTime());
  hit->cellID = utils::cellID(m_seg, *aStep);
  hit->energyDeposit = edep;
  CLHEP::Hep3Vector prePos = aStep->GetPreStepPoint()->GetPosition();
  hit->position = prePos;
  m_calorimeterCollection->insert(hit);
  return true;
}
}
