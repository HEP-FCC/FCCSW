#ifndef TESTGEOMETRY_CALORIMETERSD_H
#define TESTGEOMETRY_CALORIMETERSD_H

#include "TestGeometry/CalorimeterHit.h"

#include "G4VSensitiveDetector.hh"
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

/** @class CalorimeterHit.h TestGeometry/TestGeometry/CalorimeterHit.h CalorimeterHit.h
 *
 *  Implementation of the sensitive detector for the calorimeter with XYZ Carthesian cell grid.
 *  The cell id is computed as N*N*id_x+N*id_y+id_z, where N is the number of cells (identical in each direction).
 *  Based on B5HadCalorimeterSD from examples/basic/B5.
 *
 *  @author Anna Zaborowska
 */
namespace test {
class CalorimeterSD : public G4VSensitiveDetector
{
public:
  CalorimeterSD(std::string name);
  CalorimeterSD(std::string name, int aCellNoInAxis);
  virtual ~CalorimeterSD();
  virtual void Initialize(G4HCofThisEvent*HCE);
  virtual G4bool ProcessHits(G4Step*aStep, G4TouchableHistory*ROhist);
private:
  CalorimeterHitsCollection* m_HitsCollection;
  int m_CellNo;
};
}

#endif /* TESTGEOMETRY_CALORIMETERSD_H */
