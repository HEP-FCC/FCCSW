#ifndef DETSENSITIVE_BIRKSLAWCALORIMETERSD_H
#define DETSENSITIVE_BIRKSLAWCALORIMETERSD_H

// DD4hep
#include "DDG4/Geant4Hits.h"
#include "DDSegmentation/Segmentation.h"

// Geant
#include "G4THitsCollection.hh"
#include "G4VSensitiveDetector.hh"

/** BirksLawCalorimeterSD DetectorDescription/DetSensitive/src/BirksLawCalorimeterSD.h BirksLawCalorimeterSD.h
 *
 *  Sensitive detector for calorimeters, using Polystyrene.
 *  It is based on DD4hep::Simulation::Geant4GenericSD<Calorimeter> (but it is not identical).
 *  In particular, the position of the hit is set to G4Step::GetPreStepPoint() position.
 *  New hit is created for each energy deposit.
 *  No timing information is saved.
 *  Birks law reduces the energy deposited in the scintillator.
 *  @author    Coralie Neubueser
 */

namespace det {
class BirksLawCalorimeterSD : public G4VSensitiveDetector {
public:
  /** Constructor.
   *  @param aDetectorName Name of the detector
   *  @param aReadoutName Name of the readout (used to name the collection)
   *  @param aSeg Segmentation of the detector (used to retrieve the cell ID)
   */
  BirksLawCalorimeterSD(const std::string& aDetectorName,
                        const std::string& aReadoutName,
                        const DD4hep::Geometry::Segmentation& aSeg);
  /// Destructor
  virtual ~BirksLawCalorimeterSD();
  /** Initialization.
   *  Creates the hit collection with the name passed in the constructor.
   *  The hit collection is registered in Geant.
   *  @param aHitsCollections Geant hits collection.
   */
  virtual void Initialize(G4HCofThisEvent* aHitsCollections) final;
  /** Process hit once the particle hit the sensitive volume.
   *  Checks if the energy deposit is larger than 0, calculates the position and cellID,
   *  saves that into the hit collection.
   *  The energy deposit is reduced following the Birks law, that is an empirical formula
   *  for the light yield per path length as a function of the energy loss per path length
   *  of a particle traversing the scintillator. With th assumption that kB is 0.126 mm/MeV
   *  for polystyrene-based scintillators.
   *  New hit is created for each energy deposit.
   *  @param aStep Step in which particle deposited the energy.
   */
  virtual bool ProcessHits(G4Step* aStep, G4TouchableHistory*) final;

private:
  /// Collection of calorimeter hits
  G4THitsCollection<DD4hep::Simulation::Geant4CalorimeterHit>* m_calorimeterCollection;
  /// Segmentation of the detector used to retrieve the cell Ids
  DD4hep::Geometry::Segmentation m_seg;
  // Variables needed for the calculation of birks law
  const std::string m_material;
  const double m_birk1;
  const double m_birk2;
};
}

#endif /* DETSENSITIVE_BIRKSLAWCALORIMETERSD_H */
