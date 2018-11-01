#include "DetSensitive/MergeTrackerSD.h"

// FCCSW
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"
#include "DetCommon/DetUtils.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

#include "DetCommon/SelectTrackInformation.h"
// Geant4
#include "G4SDManager.hh"

#include "DetCommon/Geant4PreDigiTrackHit.h"

namespace det {
MergeTrackerSD::MergeTrackerSD(const std::string& aDetectorName,
                               const std::string& aReadoutName,
                               const dd4hep::Segmentation& aSeg)
    : G4VSensitiveDetector(aDetectorName),
      m_trackerCollection(nullptr),
      m_seg(aSeg),
      m_previousLeft(false),
      m_preCopyNo(-1),
      m_postCopyNo(-1),
      m_preVolume(nullptr),
      m_postVolume(nullptr),
      m_preStepPos(0., 0., 0.),
      m_postStepPos(0., 0., 0.),
      m_depEnergy(0.),
      m_globTime(0.),
      m_nHits(0),
      m_trackID(-1),
      m_partPDG(0) {
  // name of the collection of hits is determined byt the readout name (from XML)
  collectionName.insert(aReadoutName);
}

MergeTrackerSD::~MergeTrackerSD() {}

void MergeTrackerSD::Initialize(G4HCofThisEvent* aHitsCollections) {
  // create a collection of hits and add it to G4HCofThisEvent
  // deleted in ~G4Event
  m_trackerCollection = new G4THitsCollection<fcc::Geant4PreDigiTrackHit>(SensitiveDetectorName, collectionName[0]);
  aHitsCollections->AddHitsCollection(G4SDManager::GetSDMpointer()->GetCollectionID(m_trackerCollection),
                                      m_trackerCollection);
  // (re)set parameters for merging
  m_trackID = -1;
  m_previousLeft = false;
}

bool MergeTrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*) {
  G4double edep = aStep->GetTotalEnergyDeposit();
  if (edep < m_energyThreshold) return false;

  // set flag for particles which should be written out
  SelectForParticleHistory(aStep);
  // uniquely identify volume
  // get the volumes of pre and post step
  G4VPhysicalVolume* preVolume = aStep->GetPreStepPoint()->GetPhysicalVolume();
  G4VPhysicalVolume* postVolume = aStep->GetPostStepPoint()->GetPhysicalVolume();
  // access copy numbers of volumes of pre and post step
  G4int preCopyNo = preVolume->GetCopyNo();
  G4int postCopyNo = postVolume->GetCopyNo();

  // get the track identification
  G4int trackID = aStep->GetTrack()->GetTrackID();
  // first step in that sensitive module
  if (m_trackID == -1) {
    // create new hit
    CreateHit(aStep, preVolume, postVolume, edep, trackID);
    // check if leaving material
    if ((preCopyNo != postCopyNo) && (preVolume != postVolume)) {
      m_previousLeft = true;
    }
  }
  // another hit in same sensitive module
  else if ((preCopyNo == m_preCopyNo) && (preVolume == m_preVolume)) {
    // check if created by same particle and if the previous did not leave the module
    if ((trackID == m_trackID) && (!m_previousLeft)) {
      // update hit
      UpdateHit(aStep, edep);
      // check if leaving material
      if ((preCopyNo != postCopyNo) && (preVolume != postVolume)) {
        m_previousLeft = true;
      }
    }
    // created by another particle
    else {
      // previous hit created by particle that stopped in material or left sensitive module
      // save and clear
      if (m_nHits != 0) {
        SaveHit();
        ClearHit();
      }
      // create new hit
      CreateHit(aStep, preVolume, postVolume, edep, trackID);
      // check if leaving material
      if ((preCopyNo != postCopyNo) && (preVolume != postVolume)) {
        m_previousLeft = true;
      }
    }
  }
  // another hit in different sensitive module
  else {
    // previous hit created by particle that stopped in material or left sensitive module
    // save and clear
    if (m_nHits != 0) {
      SaveHit();
      ClearHit();
    }
    // create new hit
    CreateHit(aStep, preVolume, postVolume, edep, trackID);
    // check if leaving material
    if ((preCopyNo != postCopyNo) && (preVolume != postVolume)) {
      m_previousLeft = true;
    }
  }
  return true;
}

void MergeTrackerSD::CreateHit(
    G4Step* aStep, G4VPhysicalVolume* preVolume, G4VPhysicalVolume* postVolume, G4double depEnergy, G4int trackID) {

  // get track
  const G4Track* track = aStep->GetTrack();
  // set current pre and post position
  m_preStepPos = aStep->GetPreStepPoint()->GetPosition();
  m_postStepPos = aStep->GetPostStepPoint()->GetPosition();
  // set the current volume names
  m_preVolume = preVolume;
  m_postVolume = postVolume;
  m_preCopyNo = preVolume->GetCopyNo();
  m_postCopyNo = postVolume->GetCopyNo();
  // set all other parameters
  m_depEnergy = depEnergy;
  m_globTime = track->GetGlobalTime();
  m_nHits = 1;
  m_trackID = trackID;
  m_partPDG = track->GetDefinition()->GetPDGEncoding();
  m_cellID = utils::cellID(m_seg, *aStep, false);

  // (re)set
  m_previousLeft = false;
}

void MergeTrackerSD::UpdateHit(G4Step* aStep, G4double depEnergy) {

  // get track
  const G4Track* track = aStep->GetTrack();
  // Update post position
  m_postStepPos = aStep->GetPostStepPoint()->GetPosition();
  // set all other parameters
  m_depEnergy += depEnergy;
  m_globTime += track->GetGlobalTime();
  m_nHits++;
  //
}

void MergeTrackerSD::SaveHit() {
  // average global time
  m_globTime = (m_globTime != 0.) ? m_globTime / m_nHits : 0.;
  // create a hit and add it to collection
  // deleted in ~G4Event
  auto hit = new fcc::Geant4PreDigiTrackHit(m_trackID, m_partPDG, m_depEnergy, m_globTime);
  hit->cellID = m_cellID;
  hit->prePos = m_preStepPos;
  hit->postPos = m_postStepPos;
  m_trackerCollection->insert(hit);
}

void MergeTrackerSD::ClearHit() {

  // reset parameters
  m_preStepPos.setX(0.);
  m_preStepPos.setY(0.);
  m_preStepPos.setZ(0.);

  m_postStepPos.setX(0.);
  m_postStepPos.setY(0.);
  m_postStepPos.setZ(0.);

  m_depEnergy = 0.;
  m_globTime = 0.;
  m_nHits = 0;
}

void MergeTrackerSD::EndOfEvent(G4HCofThisEvent*) {

  // Save last collection hit, if some
  if (m_nHits != 0) {
    SaveHit();
    ClearHit();
  }
}

void MergeTrackerSD::SelectForParticleHistory(G4Step* aStep) const {
    aStep->GetTrack()->SetUserInformation(new det::SelectTrackInformation("SelectParticle"));
}
}
