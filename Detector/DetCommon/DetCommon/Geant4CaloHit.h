#ifndef DETCOMMON_GEANT4CALOHIT
#define DETCOMMON_GEANT4CALOHIT

#include "G4Allocator.hh"
#include "G4THitsCollection.hh"
#include "G4VHit.hh"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

namespace fcc {

/** @class  Geant4CaloHit
 *
 * Data structure to hold the geant4 output in the Calorimeter.
 *
 */
class Geant4CaloHit : public G4VHit {
public:
  /// Default constructor
  Geant4CaloHit();
  /// Constructor setting some members
  Geant4CaloHit(unsigned int aTrackId, unsigned int aPdgId, double aEnergyDeposit, double aTime);
  // Destructor
  virtual ~Geant4CaloHit();

  /// comparison operator
  G4int operator==(const Geant4CaloHit&) const;
  /// new operator needed for g4 memory allocation
  inline void* operator new(size_t);
  /// delete operator needed for g4 memory allocation
  inline void operator delete(void*);

  /// method from base class, unused
  virtual void Draw(){};
  /// method from base class, unused
  virtual void Print(){};

  // these members are public, following the example of G4VHit:

  /// the pre-step position of the step in which energy was deposited
  CLHEP::Hep3Vector position;
  /// the DD4hep cellID of the volume in which the energy was deposited
  unsigned long cellID;
  /// the g4 trackId of the particle that deposited the energy
  unsigned int trackId;
  /// the particle data group identification code for the particle
  int pdgId;
  /// the energy deposited in the material during the step
  double energyDeposit;
  /// the time coordinate of the energy deposit
  double time;
};

// types and functions for G4 memory allocation, inspired by the G4VHit classes in Geant4 examples

extern G4ThreadLocal G4Allocator<Geant4CaloHit>* Geant4CaloHitAllocator;

inline void* Geant4CaloHit::operator new(size_t) {
  if (!Geant4CaloHitAllocator) Geant4CaloHitAllocator = new G4Allocator<Geant4CaloHit>;
  return (void*)Geant4CaloHitAllocator->MallocSingle();
}

inline void Geant4CaloHit::operator delete(void* hit) { Geant4CaloHitAllocator->FreeSingle((Geant4CaloHit*)hit); }

}  // namespace fcc

#endif
