#ifndef DETSENSITIVE_MERGETRACKERSD_H
#define DETSENSITIVE_MERGETRACKERSD_H

// DD4hep
#include "DDG4/Geant4Hits.h"

// Geant
#include "G4THitsCollection.hh"
#include "G4VSensitiveDetector.hh"

namespace fcc {

class Geant4PreDigiTrackHit;
}

/** MergeTrackerSD DetectorDescription/DetSensitive/src/MergeTrackerSD.h MergeTrackerSD.h
 *
 *  Simple sensitive detector for tracker which does hits merging within the same particles in same module.
 *  A hit is only created if it exceeds m_energyThreshold.
 *  This reduces the total number of produced hits which saves memory and CPU in the future.
 *  The position of the hit is set to the position from G4Step::GetPreStepPoint() to ensure that the volume ID
 *  is calculated for the correct volume (in case where step is limited by a boundary, post-step point
 *  belongs to the next volume).
 *  A new hit is created as soon as a particle enters a new sensitive volume. In case the particle stays in the volume,
 *  the hit gets updated until it leaves. When it leaves the hit gets saved.
 *  To identify if a new volume is entered, both the copy number and the volume pointer are checked of the pre and the
 *  post step point. For the case a new volume is entered in a step, the post step will already be in the new volume.
 *  This is to make sure the volume are really different, depending on how the geometry is written, the copy number or
 *  the pointer alone may not be sufficient to distinguish.
 *  In the majority of cases hit merging produces the same result (cells hit after digitization) as without hit merging,
 *  because the scattering within the thin silicon sensores is very small.
 *  Rare cases where e.g. a low momentum secondary electron is strongly deflected and would activate additional
 *  cells may occur.
 *
 *  This class also sets a user track information 'det::SelectTrackInformation' with the type
 * "SelectParticle" to all particles which create a hit to be saved in the particle history tool in order to use this
 *  selection criterion please set 'enableHistory' and
 * 	'selectTaggedOnly' to 'true' in the 'SimG4FullSimActions'.
 *
 *  @author julia.hrdinka@cern.ch
 */
namespace det {
class MergeTrackerSD : public G4VSensitiveDetector {
public:
  /** Constructor.
   *  @param aDetectorName Name of the detector
   *  @param aReadoutName Name of the readout (used to name the collection)
   *  @param aSeg Segmentation of the detector (used to retrieve the cell ID)
   */
  MergeTrackerSD(const std::string& aDetectorName, const std::string& aReadoutName, const dd4hep::Segmentation& aSeg);
  /// Destructor
  virtual ~MergeTrackerSD();
  /** Initialization.
   *  Creates the hit collection with the name passed in the constructor.
   *  The hit collection is registered in Geant.
   *  @param aHitsCollections Geant hits collection.
   */
  virtual void Initialize(G4HCofThisEvent* aHitsCollections) final;
  /**
   *  @brief ProcessHits Process hit once the particle hit the sensitive volume.
   *  Geant4 interface method.
   *  Checks if the energy deposit is larger than 0, calculates the position and cellID,
   *  saves that into the hit collection.
   *  One hit is created for all energy deposits of the same particle within the same module. Time information is
   *  averaged over all hits.
   *  @param aStep Step in which particle deposited the energy.
   */
  virtual bool ProcessHits(G4Step* aStep, G4TouchableHistory*) final;
  /**
   *    @brief EndOfEvent Actions to be taken at end of event
   *    Saves the last hit of the event and clears the hit information for the next event.
   *    Geant4 interface method.
   *	@param hc hit collection of this event
   * */
  virtual void EndOfEvent(G4HCofThisEvent* /*hc*/) final;
  /**
   *    @brief SelectForParticleHistory flag G4Tracks which create a hit and an energy deposit > m_energyThreshold to be
   * 	saved in the particle history tool in order to use this selection criterion please set 'enableHistory' and
   * 	'selectTaggedOnly' in the 'SimG4FullSimActions'
   *    @param aStep Step in which particle deposited the energy.
   * */
  void SelectForParticleHistory(G4Step* aStep) const;

private:
  /// Collection of tracker hits
  G4THitsCollection<fcc::Geant4PreDigiTrackHit>* m_trackerCollection;
  /// Segmentation of the detector used to retrieve the cell Ids
  dd4hep::Segmentation m_seg;
  /// Previous trackID
  G4bool m_previousTrackID;
  /// Flag indicating if the medium was left in the previous step
  G4bool m_previousLeft;
  /// Copy number of the volume of the pre step
  G4int m_preCopyNo;
  /// Copy number of the volume of the pre step
  G4int m_postCopyNo;
  /// Volume of pre step
  G4VPhysicalVolume* m_preVolume;
  /// Volume of post step
  G4VPhysicalVolume* m_postVolume;
  /// The pre position of the step
  G4ThreeVector m_preStepPos;
  /// The post position of the step
  G4ThreeVector m_postStepPos;
  /// The deposited energy
  G4double m_depEnergy;
  /// The global time
  G4double m_globTime;
  /// number of hits deposited
  G4int m_nHits;
  /// The track ID
  G4int m_trackID;
  /// The pdg ID
  G4int m_partPDG;
  /// The cell ID
  uint64_t m_cellID;
  /// the energy threshold for particles to be saved in the particle history set to 360eV - 1/10th of turning on a cell
  const double m_energyThreshold = 0.00036;

  /** @brief CreateHit private helper method to create a new hit
  *   This method is called internally if a new step in a module from one particle is created. It sets the internal
  *   caches to the hit information. This does not need to be the final hit, it can still be updated in case another
  *   step in the same material from the same particle is done.
  *   @param aStep The current Geant4 step
  *   @param preVolume The pointer to the current physical volume of the pre step
  *   @param postVolume The pointer to the current physical volume of the post step
  *   @param depEnergy The deposited energy
  *   @param trackID The ID of the current track
  *   */
  void CreateHit(G4Step* aStep, G4VPhysicalVolume* preVolume, G4VPhysicalVolume* postVolume, G4double depEnergy,
                 G4int trackID);

  /** @brief UpdateHit private helper method to update current hit cache
   *  This method is internally called if another step is done in the same module from the same particle. It updates the
   *  current hit cache.
   *  @param aStep The current Geant4 step
   *  @param depEnergy The deposited energy
   * */
  void UpdateHit(G4Step* aStep, G4double depEnergy);

  /** @brief SaveHit private helper method to save the hit information
   *  This method is internally called if the last step of a particle was done in a certain module. It saves the
   * (merged) hit to the hit collection.
   * */
  void SaveHit();
  /** @brief ClearHit private method to rest hit cache
   *  This method is internally called if the last step of a particle was done in a certain module. It resets the hit
   * cache information.
   * */
  void ClearHit();
};
}

#endif /* DETSENSITIVE_MERGETRACKERSD_H */
