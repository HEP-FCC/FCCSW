#ifndef SIM_TRACKERHIT_H
#define SIM_TRACKERHIT_H

// Geant4
#include "G4VHit.hh"
#include "G4THitsCollection.hh"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

class GeantTrackerHit: public G4VHit {
public:
  GeantTrackerHit();
  ~GeantTrackerHit();
  GeantTrackerHit(const GeantTrackerHit &right);
  const GeantTrackerHit& operator=(const GeantTrackerHit &right);
  int operator==(const GeantTrackerHit &right) const;
  void SetEnergyDeposit(G4double);
  double GetEnergyDeposit() const;
  void SetPosition(CLHEP::Hep3Vector);
  CLHEP::Hep3Vector GetPosition() const;

private:
  double m_energyDeposit;
  CLHEP::Hep3Vector m_position;
};

typedef G4THitsCollection<GeantTrackerHit> GeantTrackerHitsCollection;

#endif /* SIM_TRACKERHIT_H */
