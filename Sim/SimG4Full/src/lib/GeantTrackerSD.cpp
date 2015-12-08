#include "SimG4Full/GeantTrackerSD.h"

GeantTrackerSD::GeantTrackerSD(std::string aName): G4VSensitiveDetector(aName) {
  collectionName.insert("TrackerReadout");
}

GeantTrackerSD::~GeantTrackerSD() {}

void GeantTrackerSD::Initialize(G4HCofThisEvent* aHits)
{
  // no idea what is going on here.... TODO !!!!
  //The hits collection(s) created by this sensitive detector must
  // be set to the G4HCofThisEvent object
  static int HCID = -1;
  m_trackHitsCollection = new GeantTrackerHitsCollection
    (SensitiveDetectorName,collectionName[0]);
  if(HCID<0)
  { HCID = GetCollectionID(0); }
  aHits->AddHitsCollection(HCID, m_trackHitsCollection);
}

bool GeantTrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  double enDeposited = aStep->GetTotalEnergyDeposit();
  if(enDeposited==0.) {
    return false;
  }
  GeantTrackerHit* new_hit = new GeantTrackerHit();
  new_hit->SetEnergyDeposit(enDeposited);
  new_hit->SetPosition(aStep->GetPreStepPoint()->GetPosition());
  m_trackHitsCollection->insert(new_hit);
  return true;
}

void GeantTrackerSD::EndOfEvent(G4HCofThisEvent*) {}
