#include "TestGeometry/TestCalorimeterHit.h"

namespace test {
G4ThreadLocal G4Allocator<TestCalorimeterHit>* TestCalorimeterHitAllocator;

TestCalorimeterHit::TestCalorimeterHit() : G4VHit(), m_xID(-1), m_yID(-1), m_zID(-1), m_Edep(0.), m_Pos(0) {}

TestCalorimeterHit::TestCalorimeterHit(G4int iX, G4int iY, G4int iZ)
    : G4VHit(), m_xID(iX), m_yID(iY), m_zID(iZ), m_Edep(0.), m_Pos(0) {}

TestCalorimeterHit::~TestCalorimeterHit() {}

TestCalorimeterHit::TestCalorimeterHit(const TestCalorimeterHit& right) : G4VHit() {
  m_xID = right.m_xID;
  m_yID = right.m_yID;
  m_zID = right.m_zID;
  m_Edep = right.m_Edep;
  m_Pos = right.m_Pos;
  m_Rot = right.m_Rot;
}

const TestCalorimeterHit& TestCalorimeterHit::operator=(const TestCalorimeterHit& right) {
  m_xID = right.m_xID;
  m_yID = right.m_yID;
  m_zID = right.m_zID;
  m_Edep = right.m_Edep;
  m_Pos = right.m_Pos;
  m_Rot = right.m_Rot;
  return *this;
}

int TestCalorimeterHit::operator==(const TestCalorimeterHit& right) const {
  return (m_xID == right.m_xID && m_yID == right.m_yID && m_zID == right.m_zID);
}
}
