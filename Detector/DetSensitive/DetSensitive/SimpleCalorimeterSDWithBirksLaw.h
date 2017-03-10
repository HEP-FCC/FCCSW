#ifndef DETSENSITIVE_SIMPLECALORIMETERSD_WITHBIRKS_H
#define DETSENSITIVE_SIMPLECALORIMETERSD_WITHBIRKS_H

// DD4hep
#include "DDG4/Geant4Hits.h"
#include "DDSegmentation/Segmentation.h"

// Geant
#include "G4VSensitiveDetector.hh"
#include "G4THitsCollection.hh"

/** SimpleCalorimeterSD DetectorDescription/DetSensitive/src/SimpleCalorimeterSD.h SimpleCalorimeterSD.h
 *
 *  Simple sensitive detector for calorimeter.
 *  It is based on DD4hep::Simulation::Geant4GenericSD<Calorimeter> (but it is not identical).
 *  In particular, the position of the hit is set to G4Step::GetPreStepPoint() position.
 *  New hit is created for each energy deposit.
 *  No timing information is saved.
 *  Birks law is included for polystyrene
 *  @author    Coralie Neubueser
 */

namespace det {
class SimpleCalorimeterSDWithBirksLaw : public G4VSensitiveDetector {
public:
  /** Constructor.
   *  @param aDetectorName Name of the detector
   *  @param aReadoutName Name of the readout (used to name the collection)
   *  @param aSeg Segmentation of the detector (used to retrieve the cell ID)
   */
  SimpleCalorimeterSDWithBirksLaw(const std::string& aDetectorName,
                                  const std::string& aReadoutName,
                                  const DD4hep::Geometry::Segmentation& aSeg);
  /// Destructor
  virtual ~SimpleCalorimeterSDWithBirksLaw();
  /** Initialization.
   *  Creates the hit collection with the name passed in the constructor.
   *  The hit collection is registered in Geant.
   *  @param aHitsCollections Geant hits collection.
   */
  virtual void Initialize(G4HCofThisEvent* aHitsCollections) final;
  /** Process hit once the particle hit the sensitive volume.
   *  Checks if the energy deposit is larger than 0, calculates the position and cellID,
   *  saves that into the hit collection.
   *  New hit is created for each energy deposit.
   *  @param aStep Step in which particle deposited the energy.
   */
  virtual bool ProcessHits(G4Step* aStep, G4TouchableHistory*) final;

  // Variables needed for the calculation of birks law
  const G4String myMaterial;
  const G4double birk1;
  const G4double birk2;

private:
  /// Collection of calorimeter hits
  G4THitsCollection<DD4hep::Simulation::Geant4CalorimeterHit>* m_calorimeterCollection;
  /// Segmentation of the detector used to retrieve the cell Ids
  DD4hep::Geometry::Segmentation m_seg;
};
}

#endif /* DETSENSITIVE_SIMPLECALORIMETERSD_WITHBIRKS_H */
