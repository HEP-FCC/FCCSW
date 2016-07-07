#include "DetSensitive/AggregateCalorimeterSD.h"

// FCCSW
#include "DetSensitive/SegmentationHelper.h"

// DD4hep
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

// Geant4
#include "G4SDManager.hh"

namespace det {
AggregateCalorimeterSD::AggregateCalorimeterSD(const std::string& aDetectorName,
  const std::string& aReadoutName,
  const DD4hep::Geometry::Segmentation& aSeg)
  : G4VSensitiveDetector(aDetectorName), m_seg(aSeg) {
  // name of the collection of hits is determined byt the readout name (from XML)
  collectionName.insert(aReadoutName);
}

AggregateCalorimeterSD::~AggregateCalorimeterSD(){}

void AggregateCalorimeterSD::Initialize(G4HCofThisEvent* aHitsCollections)
{
  // create a collection of hits and add it to G4HCofThisEvent
  // deleted in ~G4Event
  calorimeterCollection = new G4THitsCollection
    <DD4hep::Simulation::Geant4CalorimeterHit>(SensitiveDetectorName,collectionName[0]);
  aHitsCollections->AddHitsCollection(G4SDManager::GetSDMpointer()->GetCollectionID(calorimeterCollection),calorimeterCollection);
}

bool AggregateCalorimeterSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  // check if energy was deposited
  G4double edep = aStep->GetTotalEnergyDeposit();
  if(edep==0.)
    return false;

  // as in DD4hep::Simulation::Geant4GenericSD<Calorimeter>
  CLHEP::Hep3Vector prePos = aStep->GetPreStepPoint()->GetPosition();
  CLHEP::Hep3Vector postPos = aStep->GetPostStepPoint()->GetPosition();
  CLHEP::Hep3Vector midPos = 0.5*(postPos + prePos);
  DD4hep::Simulation::Position pos(midPos.x(), midPos.y(), midPos.z());
  // check the cell ID
  uint64_t id = segmentation::cellID(m_seg, *aStep);
  DD4hep::Simulation::Geant4CalorimeterHit* hit, *hitMatch = nullptr;
  // Check if there is already some energy deposit in that cell
  for(int i=0; i<calorimeterCollection->entries(); i++) {
    hit = dynamic_cast<DD4hep::Simulation::Geant4CalorimeterHit*>
      (calorimeterCollection->GetHit(i));
    if(hit->cellID == id) {
      hitMatch = hit;
      hitMatch->energyDeposit += edep;
      return true;
    }
  }
  // if not, create a new hit
  // deleted in ~G4Event
  hitMatch = new DD4hep::Simulation::Geant4CalorimeterHit(pos);
  // hit is expected to be created, otherwise abort job
  assert(hitMatch != nullptr);
  hitMatch->cellID  = id;
  hitMatch->energyDeposit = edep;
  calorimeterCollection->insert(hitMatch);
  return true;
}
}
