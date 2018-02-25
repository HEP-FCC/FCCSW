
#include "DetCommon/Geant4PreDigiTrackHit.h"

namespace fcc {

 G4ThreadLocal G4Allocator<Geant4PreDigiTrackHit>* Geant4PreDigiTrackHitAllocator=0;
Geant4PreDigiTrackHit::~Geant4PreDigiTrackHit() {}
Geant4PreDigiTrackHit::Geant4PreDigiTrackHit() {}
  Geant4PreDigiTrackHit::Geant4PreDigiTrackHit(unsigned int aTrackId, unsigned int aPdgId, double aEnergyDeposit, double aTime) : trackId(aTrackId), pdgId(aPdgId), energyDeposit(aEnergyDeposit), time(aTime) {}

G4int Geant4PreDigiTrackHit::operator==(const Geant4PreDigiTrackHit& right) const { return (this == &right) ? 1 : 0; }
}

