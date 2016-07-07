#include "TestGeometry/CalorimeterHit.h"

#include "G4VVisManager.hh"
#include "G4VisAttributes.hh"
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

namespace test {
G4ThreadLocal G4Allocator<CalorimeterHit>* CalorimeterHitAllocator;

CalorimeterHit::CalorimeterHit()
: G4VHit(), m_xID(-1), m_yID(-1), m_zID(-1), m_Edep(0.), m_Pos(0) {}

CalorimeterHit::CalorimeterHit(G4int iX,G4int iY,G4int iZ)
: G4VHit(), m_xID(iX), m_yID(iY), m_zID(iZ), m_Edep(0.), m_Pos(0) {}

CalorimeterHit::~CalorimeterHit() {}

CalorimeterHit::CalorimeterHit(const CalorimeterHit &right) : G4VHit() {
    m_xID = right.m_xID;
    m_yID = right.m_yID;
    m_zID = right.m_zID;
    m_Edep = right.m_Edep;
    m_Pos = right.m_Pos;
    m_Rot = right.m_Rot;
}

const CalorimeterHit& CalorimeterHit::operator=(const CalorimeterHit &right) {
    m_xID = right.m_xID;
    m_yID = right.m_yID;
    m_zID = right.m_zID;
    m_Edep = right.m_Edep;
    m_Pos = right.m_Pos;
    m_Rot = right.m_Rot;
    return *this;
}

int CalorimeterHit::operator==(const CalorimeterHit &right) const {
    return (m_xID==right.m_xID&&m_yID==right.m_yID&&m_zID==right.m_zID);
}

}
