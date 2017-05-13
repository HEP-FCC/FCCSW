#include "TestGeometry/TestCalorimeterSD.h"
#include "TestGeometry/TestCalorimeterHit.h"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4VTouchable.hh"

namespace test {
TestCalorimeterSD::TestCalorimeterSD(std::string name) : G4VSensitiveDetector(name), m_HitsCollection(0), m_CellNo(1) {
  collectionName.insert("ECalorimeterColl");
}

TestCalorimeterSD::TestCalorimeterSD(std::string name, G4int aCellNoInAxis)
    : G4VSensitiveDetector(name), m_HitsCollection(0), m_CellNo(aCellNoInAxis) {
  collectionName.insert("ECalorimeterColl");
}

TestCalorimeterSD::~TestCalorimeterSD() {}

void TestCalorimeterSD::Initialize(G4HCofThisEvent* hce) {
  m_HitsCollection = new TestCalorimeterHitsCollection(SensitiveDetectorName, collectionName[0]);
  hce->AddHitsCollection(G4SDManager::GetSDMpointer()->GetCollectionID(m_HitsCollection), m_HitsCollection);

  // fill calorimeter hits with zero energy deposition
  for (G4int ix = 0; ix < m_CellNo; ix++)
    for (G4int iy = 0; iy < m_CellNo; iy++)
      for (G4int iz = 0; iz < m_CellNo; iz++) {
        TestCalorimeterHit* hit = new TestCalorimeterHit();
        m_HitsCollection->insert(hit);
      }
}

G4bool TestCalorimeterSD::ProcessHits(G4Step* step, G4TouchableHistory*) {
  G4double edep = step->GetTotalEnergyDeposit();
  if (edep == 0.) return true;

  G4TouchableHistory* touchable = (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());

  G4int yNo = touchable->GetCopyNumber(1);
  G4int xNo = touchable->GetCopyNumber(2);
  G4int zNo = touchable->GetCopyNumber(0);

  G4int hitID = m_CellNo * m_CellNo * xNo + m_CellNo * yNo + zNo;
  TestCalorimeterHit* hit = (*m_HitsCollection)[hitID];

  if (hit->GetXid() < 0) {
    hit->SetXid(xNo);
    hit->SetYid(yNo);
    hit->SetZid(zNo);
    G4int depth = touchable->GetHistory()->GetDepth();
    G4AffineTransform transform = touchable->GetHistory()->GetTransform(depth);
    transform.Invert();
    hit->SetRot(transform.NetRotation());
    hit->SetPos(transform.NetTranslation());
  }
  hit->AddEdep(edep);
  return true;
}
}
