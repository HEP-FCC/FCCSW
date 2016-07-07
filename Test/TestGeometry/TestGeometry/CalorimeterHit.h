#ifndef TESTGEOMETRY_CALORIMETERHIT_H
#define TESTGEOMETRY_CALORIMETERHIT_H

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"

class G4AttDef;
class G4AttValue;

/** @class CalorimeterHit.h TestGeometry/TestGeometry/CalorimeterHit.h CalorimeterHit.h
 *
 *  Implementation of the hit for the calorimeter with XYZ Carthesian cell grid.
 *  It records:
 *  - the cell x ID, y ID and z ID
 *  - the energy deposit
 *  - the cell position and rotation.
 *  Based on B5HadCalorimeterHit from Geant4 examples/basic/B5.
 *
 *  @author Anna Zaborowska
 */

namespace test {
class CalorimeterHit : public G4VHit {
public:
  CalorimeterHit();
  CalorimeterHit(int iX, int iY, int iZ);
  CalorimeterHit(const CalorimeterHit &right);
  virtual ~CalorimeterHit();

  const CalorimeterHit& operator=(const CalorimeterHit &right);
  int operator==(const CalorimeterHit &right) const;

  inline void *operator new(size_t);
  inline void operator delete(void *aHit);

  void SetXid(int z) { m_xID = z; }
  int GetXid() const { return m_xID; }

  void SetYid(int z) { m_yID = z; }
  int GetYid() const { return m_yID; }

  void SetZid(int z) { m_zID = z; }
  int GetZid() const { return m_zID; }

  void SetEdep(double de) { m_Edep = de; }
  void AddEdep(double de) { m_Edep += de; }
  double GetEdep() const { return m_Edep; }

  void SetPos(G4ThreeVector xyz) { m_Pos = xyz; }
  G4ThreeVector GetPos() const { return m_Pos; }

  void SetRot(G4RotationMatrix rmat) { m_Rot = rmat; }
  G4RotationMatrix GetRot() const { return m_Rot; }

private:
  int m_xID;
  int m_yID;
  int m_zID;
  double m_Edep;
  G4ThreeVector m_Pos;
  G4RotationMatrix m_Rot;
};

typedef G4THitsCollection<CalorimeterHit> CalorimeterHitsCollection;

extern G4ThreadLocal G4Allocator<CalorimeterHit>* CalorimeterHitAllocator;

inline void* CalorimeterHit::operator new(size_t)
{
  if (!CalorimeterHitAllocator)
    CalorimeterHitAllocator = new G4Allocator<CalorimeterHit>;
  return (void*)CalorimeterHitAllocator->MallocSingle();
}

inline void CalorimeterHit::operator delete(void* aHit)
{
  CalorimeterHitAllocator->FreeSingle((CalorimeterHit*)aHit);
}
}

#endif /* TESTGEOMETRY_CALORIMETERHIT_H */
