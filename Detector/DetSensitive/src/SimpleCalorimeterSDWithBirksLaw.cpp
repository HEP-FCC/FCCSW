#include "DetSensitive/SimpleCalorimeterSDWithBirksLaw.h"

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
SimpleCalorimeterSDWithBirksLaw::SimpleCalorimeterSDWithBirksLaw(const std::string& aDetectorName,
                                                                 const std::string& aReadoutName,
                                                                 const DD4hep::Geometry::Segmentation& aSeg)
    : G4VSensitiveDetector(aDetectorName), m_calorimeterCollection(nullptr), m_seg(aSeg) {
  // name of the collection of hits is determined byt the readout name (from XML)
  collectionName.insert(aReadoutName);
}

SimpleCalorimeterSDWithBirksLaw::~SimpleCalorimeterSDWithBirksLaw() {}

void SimpleCalorimeterSDWithBirksLaw::Initialize(G4HCofThisEvent* aHitsCollections) {
  // create a collection of hits and add it to G4HCofThisEvent
  // deleted in ~G4Event
  m_calorimeterCollection =
      new G4THitsCollection<DD4hep::Simulation::Geant4CalorimeterHit>(SensitiveDetectorName, collectionName[0]);
  aHitsCollections->AddHitsCollection(G4SDManager::GetSDMpointer()->GetCollectionID(m_calorimeterCollection),
                                      m_calorimeterCollection);
}

bool SimpleCalorimeterSDWithBirksLaw::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
  // check if energy was deposited
  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep == 0.) return false;

  const G4String myMaterial = "Polystyrene";
  const G4double birk1 = 0.0130 * CLHEP::g / (CLHEP::MeV * CLHEP::cm2);
  const G4double birk2 = 9.6e-6 * CLHEP::g / (CLHEP::MeV * CLHEP::cm2) * CLHEP::g / (CLHEP::MeV * CLHEP::cm2);
  G4double response = 0.;

  G4double destep = aStep->GetTotalEnergyDeposit();
  G4Material* material = aStep->GetPreStepPoint()->GetMaterial();
  G4double charge = aStep->GetPreStepPoint()->GetCharge();

  if ((charge != 0.) && (material->GetName() == myMaterial)) {
    G4double rkb = birk1;
    // --- correction for particles with more than 1 charge unit ---
    // --- based on alpha particle data (only apply for MODEL=1) ---
    if (std::fabs(charge) > 1.0) rkb *= 7.2 / 12.6;

    if (aStep->GetStepLength() != 0) {
      G4double dedx = destep / (aStep->GetStepLength()) / (material->GetDensity());
      response = destep / (1. + rkb * dedx + birk2 * dedx * dedx);
    } else {
      response = destep;
    }
  } else {
    response = destep;
  }
  edep = response;

  // as in DD4hep::Simulation::Geant4GenericSD<Calorimeter>
  CLHEP::Hep3Vector prePos = aStep->GetPreStepPoint()->GetPosition();
  DD4hep::Simulation::Position pos(prePos.x(), prePos.y(), prePos.z());
  auto hit = new DD4hep::Simulation::Geant4CalorimeterHit(pos);
  hit->cellID = utils::cellID(m_seg, *aStep);
  hit->energyDeposit = edep;
  m_calorimeterCollection->insert(hit);
  return true;
}
}
