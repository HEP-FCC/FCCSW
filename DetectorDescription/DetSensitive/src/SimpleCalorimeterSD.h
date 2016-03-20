#ifndef DETSENSITIVE_SIMPLECALORIMETERSD_H
#define DETSENSITIVE_SIMPLECALORIMETERSD_H

#include "G4VSensitiveDetector.hh"
#include "DDG4/Geant4Hits.h"
#include "G4THitsCollection.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;
#include "DDSegmentation/Segmentation.h"

namespace det {
class SimpleCalorimeterSD : public G4VSensitiveDetector
{

  public:
  SimpleCalorimeterSD(std::string aDetectorName, std::string aReadoutName, DD4hep::Geometry::Segmentation aSeg);
      ~SimpleCalorimeterSD();

      void Initialize(G4HCofThisEvent*HCE);
      G4bool ProcessHits(G4Step*aStep, G4TouchableHistory*ROhist);
      void EndOfEvent(G4HCofThisEvent*HCE);
      void clear();
      void DrawAll();
      void PrintAll();
      uint64_t getCellID(G4Step* s);

  private:
      G4THitsCollection<DD4hep::Simulation::Geant4CalorimeterHit> *calorimeterCollection;
      DD4hep::Geometry::Segmentation m_seg;
};
}

#endif /* DETSENSITIVE_SIMPLECALORIMETERSD_H */
