
#include "DetCommon/Geant4CaloHit.h"

namespace fcc {

// G4 allocation method
G4ThreadLocal G4Allocator<Geant4CaloHit>* Geant4CaloHitAllocator = 0;
// Destructor
Geant4CaloHit::~Geant4CaloHit() {}
// Default Constructor
Geant4CaloHit::Geant4CaloHit() {}
// Constructor setting some members
Geant4CaloHit::Geant4CaloHit(unsigned int aTrackId, unsigned int aPdgId, double aEnergyDeposit, double aTime)
    : trackId(aTrackId), pdgId(aPdgId), energyDeposit(aEnergyDeposit), time(aTime) {}

// comparison operator
G4int Geant4CaloHit::operator==(const Geant4CaloHit& right) const { return (this == &right) ? 1 : 0; }

}  // namespace fcc
