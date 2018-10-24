#ifndef DETSENSITIVE_SIMPLEDRIFTCHAMBER_H
#define DETSENSITIVE_SIMPLEDRIFTCHAMBER_H

// DD4hep
#include "DDG4/Geant4Hits.h"
#include "DDSegmentation/Segmentation.h"

// Geant
#include "G4THitsCollection.hh"
#include "G4VSensitiveDetector.hh"

namespace fcc {

class Geant4PreDigiTrackHit;
}

/*
 *
 *  @author    nalipour
 */

namespace det {
class SimpleDriftChamber : public G4VSensitiveDetector {
public:
  /** Constructor.
   *  @param aDetectorName Name of the detector
   *  @param aReadoutName Name of the readout (used to name the collection)
   *  @param aSeg Segmentation of the detector (used to retrieve the cell ID)
   */
  SimpleDriftChamber(const std::string& aDetectorName,
                     const std::string& aReadoutName,
                     const dd4hep::Segmentation& aSeg);
  /// Destructor
  virtual ~SimpleDriftChamber();
  /** Initialization.
   *  Creates the hit collection with the name passed in the constructor.
   *  The hit collection is registered in Geant.
   *  @param aHitsCollections Geant hits collection.
   */
  virtual void Initialize(G4HCofThisEvent* aHitsCollections) final;
  virtual bool ProcessHits(G4Step* aStep, G4TouchableHistory*) final;

private:
  /// Collection of tracker hits
  G4THitsCollection<fcc::Geant4PreDigiTrackHit>* m_driftChamberCollection;
  /// Segmentation of the detector used to retrieve the cell Ids
  dd4hep::Segmentation m_seg;

  // cuts on the Edep and the G4 step length 
  double m_edepCut = 10 * CLHEP::eV;
  double m_stepLengthCut = 5 * CLHEP::micrometer;
};
}

#endif /* DETSENSITIVE_SIMPLEDRIFTCHAMBER_H */
