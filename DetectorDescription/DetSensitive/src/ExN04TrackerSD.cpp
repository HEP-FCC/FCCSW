#include "DetSensitive/ExN04TrackerSD.h"
// #include "ExN04TrackerHit.h"
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"

ExN04TrackerSD::ExN04TrackerSD(std::string aDetectorName, std::string aReadoutName)
  :G4VSensitiveDetector(aDetectorName) {
  // add a collection of hits
  // (where? is it needed? dd4hep doesnt do it?)
  // collectionName.insert(aReadoutName);
  // std::cout<<" Adding collection named: "<<HCname<<std::endl;
}

ExN04TrackerSD::~ExN04TrackerSD(){;}

void ExN04TrackerSD::Initialize(G4HCofThisEvent* aHitsCollections)
{
  // create a collection of hits and add it to G4HCofThisEvent
  // (where? is it needed? dd4hep doesnt do it?)
  // get id for collection
  static int HCID = -1;
  // trackerCollection = new ExN04TrackerHitsCollection
  //                     (SensitiveDetectorName,collectionName[0]);
  if(HCID<0)
  { HCID = GetCollectionID(0); }
  // HCE->AddHitsCollection(HCID,trackerCollection);
}

G4bool ExN04TrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  // check if energy was deposited
  G4double edep = aStep->GetTotalEnergyDeposit();
  if(edep==0.) return false;

  // create a hit and add it to collection

  // ExN04TrackerHit* newHit = new ExN04TrackerHit();
  // newHit->SetEdep( edep );
  // newHit->SetPos( aStep->GetPreStepPoint()->GetPosition() );
  // trackerCollection->insert( newHit );
  //std::cout<<"hit:  E = "<<edep<<"  pos: "<<aStep->GetPreStepPoint()->GetPosition()<<std::endl;
  //
  return true;
}

void ExN04TrackerSD::EndOfEvent(G4HCofThisEvent*) {}

void ExN04TrackerSD::clear() {}

void ExN04TrackerSD::DrawAll() {}

void ExN04TrackerSD::PrintAll() {}
