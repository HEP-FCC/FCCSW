#ifndef DETSENSITIVE_FULLPARTICLEABSORPTIONSD_H
#define DETSENSITIVE_FULLPARTICLEABSORPTIONSD_H

// DD4hep
#include "DDG4/Geant4Hits.h"
#include "DDSegmentation/Segmentation.h"

// Geant
#include "G4THitsCollection.hh"
#include "G4VSensitiveDetector.hh"

/** FullParticleAbsD DetectorDescription/DetSensitive/src/FullParticleAbsSD.h FullParticleAbsSD.h
 *
 *  Sensitive detector to fully stop the incoming particles.
 *  The position of the hit is set to G4Step::GetPreStepPoint() position.
 *  New hit is created for each incoming particle, the energy is stored and the track is removed.
 *  No timing information is saved.
 *
 *  @author    Coralie Neubueser
 */

namespace det {
class FullParticleAbsorptionSD : public G4VSensitiveDetector {
public:
  /** Constructor.
   *  @param aDetectorName Name of the detector
   *  @param aReadoutName Name of the readout (used to name the collection)
   *  @param aSeg Segmentation of the detector (used to retrieve the cell ID)
   */
  FullParticleAbsorptionSD(const std::string& aDetectorName,
                    const std::string& aReadoutName,
                    const DD4hep::Geometry::Segmentation& aSeg);
  /// Destructor
  virtual ~FullParticleAbsorptionSD();
  /** Initialization.
   *  Creates the hit collection with the name passed in the constructor.
   *  The hit collection is registered in Geant.
   *  @param aHitsCollections Geant hits collection.
   */
  virtual void Initialize(G4HCofThisEvent* aHitsCollections) final;
  /** Process hit once the particle hit the sensitive volume.
   *  Gets the kinetic energy from the particle track, calculates the position and cellID,
   *  saves that into the hit collection.
   *  New hit is created for first track of each particle within sensitive detector.
   *  @param aStep Step in which particle deposited the energy.
   */
  virtual bool ProcessHits(G4Step* aStep, G4TouchableHistory*) final;

private:
  /// Collection of calorimeter hits
  G4THitsCollection<DD4hep::Simulation::Geant4CalorimeterHit>* m_calorimeterCollection;
  /// Segmentation of the detector used to retrieve the cell Ids
  DD4hep::Geometry::Segmentation m_seg;
};
}

#endif /* DETSENSITIVE_FULLPARTICLEPABSORPTIONSD_H */
