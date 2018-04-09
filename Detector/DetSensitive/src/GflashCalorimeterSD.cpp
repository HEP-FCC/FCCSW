#include "DetSensitive/GflashCalorimeterSD.h"

// FCCSW
#include "DetCommon/DetUtils.h"

// DD4hep
#include "DDG4/Geant4Converter.h"
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"
#include "DDG4/Defs.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

// Geant4
#include "G4SDManager.hh"

namespace det {
GflashCalorimeterSD::GflashCalorimeterSD(const std::string& aDetectorName,
                                         const std::string& aReadoutName,
                                         const dd4hep::Segmentation& aSeg)
    : G4VSensitiveDetector(aDetectorName), G4VGFlashSensitiveDetector(), m_seg(aSeg) {
  // name of the collection of hits is determined byt the readout name (from XML)
  collectionName.insert(aReadoutName);
}

GflashCalorimeterSD::~GflashCalorimeterSD() {}

void GflashCalorimeterSD::Initialize(G4HCofThisEvent* aHitsCollections) {
  // create a collection of hits and add it to G4HCofThisEvent
  // deleted in ~G4Event
  m_calorimeterCollection =
      new G4THitsCollection<dd4hep::sim::Geant4CalorimeterHit>(SensitiveDetectorName, collectionName[0]);
  aHitsCollections->AddHitsCollection(G4SDManager::GetSDMpointer()->GetCollectionID(m_calorimeterCollection),
                                      m_calorimeterCollection);
}

bool GflashCalorimeterSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
  // This method is called if full simulation is performed
  // check if energy was deposited
  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep == 0.) return false;

  CLHEP::Hep3Vector prePos = aStep->GetPreStepPoint()->GetPosition();
  dd4hep::Position pos(prePos.x(), prePos.y(), prePos.z());
  // create a new hit
  // deleted in ~G4Event
  dd4hep::sim::Geant4CalorimeterHit* hit = new dd4hep::sim::Geant4CalorimeterHit(pos);
  hit->cellID = utils::cellID(m_seg, *aStep);
  hit->energyDeposit = edep;
  m_calorimeterCollection->insert(hit);
  return true;
}

bool GflashCalorimeterSD::ProcessHits(G4GFlashSpot* aSpot, G4TouchableHistory*) {
  // This method will be called if gflash parametrisation is performed
  G4double edep = aSpot->GetEnergySpot()->GetEnergy();
  // check if energy was deposited
  if (edep == 0.) return false;
  CLHEP::Hep3Vector geantPos = aSpot->GetEnergySpot()->GetPosition();
  dd4hep::Position pos(geantPos.x(), geantPos.y(), geantPos.z());
  // create a new hit
  // deleted in ~G4Event
  dd4hep::sim::Geant4CalorimeterHit* hit = new dd4hep::sim::Geant4CalorimeterHit(pos);
  hit->cellID = cellID(*aSpot);
  hit->energyDeposit = edep;
  m_calorimeterCollection->insert(hit);
  return true;
}

uint64_t GflashCalorimeterSD::cellID(const G4GFlashSpot& aSpot) {
  dd4hep::sim::Geant4VolumeManager volMgr = dd4hep::sim::Geant4Mapping::instance().volumeManager();
  dd4hep::VolumeID volID = volMgr.volumeID(aSpot.GetTouchableHandle()());
  if (m_seg.isValid()) {
    G4ThreeVector global = aSpot.GetEnergySpot()->GetPosition();
    G4ThreeVector local = aSpot.GetTouchableHandle()->GetHistory()->GetTopTransform().TransformPoint(global);
    dd4hep::Position loc(local.x() * MM_2_CM, local.y() * MM_2_CM, local.z() * MM_2_CM);
    dd4hep::Position glob(global.x() * MM_2_CM, global.y() * MM_2_CM, global.z() * MM_2_CM);
    dd4hep::VolumeID cID = m_seg.cellID(loc, glob, volID);
    return cID;
  }
  return volID;
}
}
