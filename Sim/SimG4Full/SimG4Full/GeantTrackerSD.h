#ifndef SIM_TRACKERSD_H
#define SIM_TRACKERSD_H

#include "G4VSensitiveDetector.hh"

#include "SimG4Data/GeantTrackerHit.h"

class GeantTrackerSD : public G4VSensitiveDetector
{

public:
  GeantTrackerSD(std::string name);
  ~GeantTrackerSD();

  void Initialize(G4HCofThisEvent*HCE);
  bool ProcessHits(G4Step*aStep,G4TouchableHistory*ROhist);
  void EndOfEvent(G4HCofThisEvent*HCE);

private:
  GeantTrackerHitsCollection* m_trackHitsCollection;
};

#endif /* SIM_TRACKERSD_H */
