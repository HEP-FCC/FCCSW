#ifndef DETSENSITIVE_GEANT4PREDIGITRACKHIT_H
#define DETSENSITIVE_GEANT4PREDIGITRACKHIT_H

#include "G4Allocator.hh"
#include "G4THitsCollection.hh"
#include "G4VHit.hh"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

namespace fcc {

class Geant4PreDigiTrackHit : public G4VHit {
public:
  Geant4PreDigiTrackHit();
  Geant4PreDigiTrackHit(unsigned int aTrackId, unsigned int aPdgId, double aEnergyDeposit, double aTime);
  virtual ~Geant4PreDigiTrackHit();

  // operators
  G4int operator==(const Geant4PreDigiTrackHit&) const;

  inline void* operator new(size_t);
  inline void operator delete(void*);

  // methods from base class
  virtual void Draw(){};
  virtual void Print(){};

  CLHEP::Hep3Vector prePos;
  CLHEP::Hep3Vector postPos;
  unsigned long cellID;
  unsigned int trackId;
  int pdgId;
  double energyDeposit;
  double time;

private:
};

typedef G4THitsCollection<Geant4PreDigiTrackHit> Geant4PreDigiTrackHitsCollection;

extern G4ThreadLocal G4Allocator<Geant4PreDigiTrackHit>* Geant4PreDigiTrackHitAllocator;

inline void* Geant4PreDigiTrackHit::operator new(size_t) {
  if (!Geant4PreDigiTrackHitAllocator) Geant4PreDigiTrackHitAllocator = new G4Allocator<Geant4PreDigiTrackHit>;
  return (void*)Geant4PreDigiTrackHitAllocator->MallocSingle();
}


inline void Geant4PreDigiTrackHit::operator delete(void* hit) {
  Geant4PreDigiTrackHitAllocator->FreeSingle((Geant4PreDigiTrackHit*)hit);
}

}

#endif
