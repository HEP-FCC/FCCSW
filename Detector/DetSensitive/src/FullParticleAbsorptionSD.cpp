#include "DetSensitive/FullParticleAbsorptionSD.h"

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
FullParticleAbsorptionSD::FullParticleAbsorptionSD(const std::string& aDetectorName,
                                     const std::string& aReadoutName,
                                     const DD4hep::Geometry::Segmentation& aSeg)
    : G4VSensitiveDetector(aDetectorName), m_calorimeterCollection(nullptr), m_seg(aSeg) {
  // name of the collection of hits is determined byt the readout name (from XML)
  collectionName.insert(aReadoutName);
}

FullParticleAbsorptionSD::~FullParticleAbsorptionSD() {}

void FullParticleAbsorptionSD::Initialize(G4HCofThisEvent* aHitsCollections) {
  // create a collection of hits and add it to G4HCofThisEvent
  // deleted in ~G4Event
  m_calorimeterCollection =
      new G4THitsCollection<DD4hep::Simulation::Geant4CalorimeterHit>(SensitiveDetectorName, collectionName[0]);
  aHitsCollections->AddHitsCollection(G4SDManager::GetSDMpointer()->GetCollectionID(m_calorimeterCollection),
                                      m_calorimeterCollection);
}

bool FullParticleAbsorptionSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
  CLHEP::Hep3Vector prePos = aStep->GetPreStepPoint()->GetPosition();
  G4Track* aTrack = aStep->GetTrack();
  G4double kineticEnergy = aTrack->GetKineticEnergy();
  DD4hep::Simulation::Position pos(prePos.x(), prePos.y(), prePos.z());
  auto hit = new DD4hep::Simulation::Geant4CalorimeterHit(pos);
  hit->cellID = utils::cellID(m_seg, *aStep);
  hit->energyDeposit = kineticEnergy;
  m_calorimeterCollection->insert(hit);
  // kill the track to ensure no double counting
  aTrack->SetTrackStatus(fStopAndKill);
  return true;
}
}
