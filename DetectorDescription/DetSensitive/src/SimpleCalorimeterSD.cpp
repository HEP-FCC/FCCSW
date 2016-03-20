#include "SimpleCalorimeterSD.h"
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"

// DD4hep
#include "DDG4/Geant4Hits.h"
#include "DDG4/Geant4Data.h"
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/ThreeVector.h"

namespace det {
SimpleCalorimeterSD::SimpleCalorimeterSD(std::string aDetectorName,
  std::string aReadoutName,
  DD4hep::Geometry::Segmentation aSeg)
  :G4VSensitiveDetector(aDetectorName), m_seg(aSeg) {
  // add a name of the collection of hits
  collectionName.insert(aReadoutName);
  std::cout<<" Adding a collection with the name: "<<aReadoutName<<std::endl;
}

SimpleCalorimeterSD::~SimpleCalorimeterSD(){;}

void SimpleCalorimeterSD::Initialize(G4HCofThisEvent* aHitsCollections)
{
  // create a collection of hits and add it to G4HCofThisEvent
  // get id for collection
  // TODO why we create new collection without checking it exists?
  static int HCID = -1;
  calorimeterCollection = new G4THitsCollection<DD4hep::Simulation::Geant4CalorimeterHit>(SensitiveDetectorName,collectionName[0]);
  if(HCID<0)
    HCID = GetCollectionID(0);
  aHitsCollections->AddHitsCollection(HCID,calorimeterCollection);
}

G4bool SimpleCalorimeterSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  // check if energy was deposited
  G4double edep = aStep->GetTotalEnergyDeposit();
  if(edep==0.) return false;

  // as in DD4hep::Simulation::Geant4GenericSD<Calorimeter>
  CLHEP::Hep3Vector prePos = aStep->GetPreStepPoint()->GetPosition();
  CLHEP::Hep3Vector postPos = aStep->GetPostStepPoint()->GetPosition();
  CLHEP::Hep3Vector midPos = 0.5*(postPos + prePos);
  DD4hep::Simulation::Position pos(midPos.x(),
                                   midPos.y(),
                                   midPos.z());
  CLHEP::Hep3Vector direction = postPos - prePos;
  double hit_len = direction.perp();
  // create a hit and add it to collection
  const G4Track* track = aStep->GetTrack();
  uint64_t id = getCellID(aStep);
  DD4hep::Simulation::Geant4CalorimeterHit* hit;
  for(int i=0; i<calorimeterCollection->entries(); i++) {
    hit = dynamic_cast<DD4hep::Simulation::Geant4CalorimeterHit*>(calorimeterCollection->GetHit(i));
    if(hit->cellID == id) {
      std::cout<<hit->cellID<<"\t=\t"<<id<<std::endl;
      continue;
    }
  }
  if ( !hit )  {
    hit = new  DD4hep::Simulation::Geant4CalorimeterHit(pos);
    hit->cellID  = id;
    calorimeterCollection->insert(hit);
  }
  DD4hep::Simulation::Geant4Hit::Contribution contrib = DD4hep::Simulation::Geant4Hit::extractContribution(aStep);
  hit->truth.push_back(contrib);
  hit->energyDeposit += contrib.deposit;
  return true;
}

void SimpleCalorimeterSD::EndOfEvent(G4HCofThisEvent*) {}

void SimpleCalorimeterSD::clear() {}

void SimpleCalorimeterSD::DrawAll() {}

void SimpleCalorimeterSD::PrintAll() {}

uint64_t SimpleCalorimeterSD::getCellID(G4Step* aStep) {
  DD4hep::Simulation::Geant4VolumeManager volMgr = DD4hep::Simulation::Geant4Mapping::instance().volumeManager();
  DD4hep::Geometry::VolumeManager::VolumeID volID = volMgr.volumeID(aStep->GetPreStepPoint()->GetTouchable());
  if (m_seg.isValid() )  {
    G4ThreeVector global = 0.5 * (  aStep->GetPreStepPoint()->GetPosition()+
      aStep->GetPostStepPoint()->GetPosition());
    G4ThreeVector local  = aStep->GetPreStepPoint()->GetTouchable()->GetHistory()->GetTopTransform().TransformPoint(global);
    DD4hep::Simulation::Position loc(local.x()*MM_2_CM, local.y()*MM_2_CM, local.z()*MM_2_CM);
    DD4hep::Simulation::Position glob(global.x()*MM_2_CM, global.y()*MM_2_CM, global.z()*MM_2_CM);
    DD4hep::Geometry::VolumeManager::VolumeID cID = m_seg.cellID(loc,glob,volID);
    return cID;
  }
  return 0;
}
}
