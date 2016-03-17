#include "SimpleTrackerSD.h"
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"

// DD4hep
#include "DDG4/Geant4Hits.h"
#include "CLHEP/Vector/ThreeVector.h"

namespace det {
SimpleTrackerSD::SimpleTrackerSD(std::string aDetectorName, std::string aReadoutName)
  :G4VSensitiveDetector(aDetectorName) {
  // add a name of the collection of hits
  collectionName.insert(aReadoutName);
  std::cout<<" Adding a collection with the name: "<<aReadoutName<<std::endl;
}

SimpleTrackerSD::~SimpleTrackerSD(){;}

void SimpleTrackerSD::Initialize(G4HCofThisEvent* aHitsCollections)
{
  // create a collection of hits and add it to G4HCofThisEvent
  // get id for collection
  // TODO why we create new collection without checking it exists?
  static int HCID = -1;
  trackerCollection = new G4THitsCollection<DD4hep::Simulation::Geant4Hit>(SensitiveDetectorName,collectionName[0]);
  if(HCID<0)
    HCID = GetCollectionID(0);
  aHitsCollections->AddHitsCollection(HCID,trackerCollection);
}

G4bool SimpleTrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  // check if energy was deposited
  G4double edep = aStep->GetTotalEnergyDeposit();
  if(edep==0.) return false;

  // as in DD4hep::Simulation::Geant4GenericSD<Tracker>
  CLHEP::Hep3Vector prePos = aStep->GetPreStepPoint()->GetPosition();
  CLHEP::Hep3Vector postPos = aStep->GetPostStepPoint()->GetPosition();
  CLHEP::Hep3Vector direction = postPos - prePos;
  double hit_len = direction.perp();
  // create a hit and add it to collection
  const G4Track* track = aStep->GetTrack();
  DD4hep::Simulation::Geant4TrackerHit* hit = new  DD4hep::Simulation::Geant4TrackerHit(
    track->GetTrackID(), track->GetDefinition()->GetPDGEncoding(),edep, track->GetGlobalTime());
  if ( hit )  {
    // TODO implement method to get cellID
    //hit->cellID  = getCellID( step ) ;
    // TODO contrib?
    auto contrib = DD4hep::Simulation::Geant4Hit::extractContribution(aStep);
    hit->energyDeposit =  contrib.deposit ;
    hit->position = DD4hep::Simulation::Position(prePos.x(), prePos.y(), prePos.z());
    hit->momentum = direction;
    hit->length   = hit_len;
    trackerCollection->insert(hit);
    return true;
  }
  return true;
}

void SimpleTrackerSD::EndOfEvent(G4HCofThisEvent*) {}

void SimpleTrackerSD::clear() {}

void SimpleTrackerSD::DrawAll() {}

void SimpleTrackerSD::PrintAll() {}
}
