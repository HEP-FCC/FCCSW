#ifndef DETSENSITIVE_GEANT4PREDIGITRACKHIT_H
#define DETSENSITIVE_GEANT4PREDIGITRACKHIT_H

#include "G4Allocator.hh"
#include "G4THitsCollection.hh"
#include "G4VHit.hh"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

namespace fcc {

/** @class  Geant4PreDigiTrackHit 
 *
 * Data structure to hold the geant4 output in the Tracker.
 * It provides members to hold both the post- and pre-step
 * position of the step in the sensitive volume. Note that 
 * this often, but not necessarily corresponds to the entry -
 * and exit point in the sensitive volume. It is possible that
 * Geant will take several steps even in a thin volume such as 
 * the tracker module silicon.
 *
 *
 */
class Geant4PreDigiTrackHit : public G4VHit {
public:
  /// Default constructor
  Geant4PreDigiTrackHit();
  /// Constructor setting some members
  Geant4PreDigiTrackHit(unsigned int aTrackId, unsigned int aPdgId, double aEnergyDeposit, double aTime);
  // Destructor
  virtual ~Geant4PreDigiTrackHit();

  /// comparison operator
  G4int operator==(const Geant4PreDigiTrackHit&) const;
  /// new operator needed for g4 memory allocation
  inline void* operator new(size_t);
  /// delete operator needed for g4 memory allocation
  inline void operator delete(void*);

  /// method from base class, unused
  virtual void Draw() {};
  /// method from base class, unused
  virtual void Print() {};

  // these members are public, following the example of G4VHit:

  /// the pre-step position of the step in which energy was deposited
  CLHEP::Hep3Vector prePos;
  /// the post-step position of the step in which energy was deposited
  CLHEP::Hep3Vector postPos;
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

typedef G4THitsCollection<Geant4PreDigiTrackHit> Geant4PreDigiTrackHitsCollection;

extern G4ThreadLocal G4Allocator<Geant4PreDigiTrackHit>* Geant4PreDigiTrackHitAllocator;

inline void* Geant4PreDigiTrackHit::operator new(size_t) {
  if (!Geant4PreDigiTrackHitAllocator) Geant4PreDigiTrackHitAllocator = new G4Allocator<Geant4PreDigiTrackHit>;
  return (void*)Geant4PreDigiTrackHitAllocator->MallocSingle();
}

inline void Geant4PreDigiTrackHit::operator delete(void* hit) {
  Geant4PreDigiTrackHitAllocator->FreeSingle((Geant4PreDigiTrackHit*)hit);
}

} // namespace fcc

#endif
