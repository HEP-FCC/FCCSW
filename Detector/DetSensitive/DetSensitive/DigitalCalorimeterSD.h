#ifndef DETSENSITIVE_DIGITALCALORIMETERSD_H
#define DETSENSITIVE_DIGITALCALORIMETERSD_H

// DD4hep
#include "DDG4/Geant4Hits.h"
#include "DDSegmentation/Segmentation.h"

// Geant
#include "G4VSensitiveDetector.hh"
#include "G4THitsCollection.hh"

#include <fstream>
#include <string>

#include "TFile.h"
#include "TH1F.h"

/** DigitalCalorimeterSD DetectorDescription/DetSensitive/src/DigitalCalorimeterSD.h DigitalCalorimeterSD.h
 *
 *  Simple sensitive detector for calorimeter.
 *  It is based on DD4hep::Simulation::Geant4GenericSD<Calorimeter> (but it is not identical).
 *  In particular, the position of the hit is set to G4Step::GetPreStepPoint() position.
 *  New hit is created for each energy deposit.
 *  No timing information is saved.
 *
 *  @author    Anna Zaborowska
 */

struct Strixel
{
  int layer;
  int x;
  int y;
  int z;

  bool operator<(const Strixel &other) const
  {
    if(layer<other.layer) {
      return true;
    } else if (layer==other.layer && x<other.x) {
      return true;
    } else if (layer==other.layer && x==other.x && y<other.y) {
      return true;
    } else if (layer==other.layer && x==other.x && y==other.y && z<other.z) {
      return true;
    } else return false;
  }

  bool operator==(const Strixel &other) const
  {
    return(layer == other.layer &&
            x == other.x &&
            y == other.y &&
            z == other.z);
  }
};

namespace std{

  template <>
  struct hash<Strixel>
  {
    std::size_t operator()(const Strixel& s) const
    {
      using std::size_t;
      using std::hash;
      using std::string;

      // Compute individual hash values for first,
      // second and third and combine them using XOR
      // and bit shifting:      

      return hash<string>()(
              std::to_string(s.layer) +","+
              std::to_string(s.x) +","+
              std::to_string(s.y) +","+
              std::to_string(s.z)
              );
    }
  };

}

struct Pad
{
  int layer;
  int r;
  int z;

  bool operator<(const Pad &other) const
  {
    if(layer<other.layer) {
      return true;
    } else if (layer==other.layer && r<other.r) {
      return true;
    } else if (layer==other.layer && r==other.r && z<other.z) {
      return true;
    }  else return false;
  }

  bool operator==(const Pad &other) const
  {
    return(layer == other.layer &&
            r == other.r &&
            z == other.z);
  }
}; 


namespace std{

  template <>
  struct hash<Pad>
  {
    std::size_t operator()(const Pad& p) const
    {
      using std::size_t;
      using std::hash;
      using std::string;

      // Compute individual hash values for first,
      // second and third and combine them using XOR
      // and bit shifting:      

      return hash<string>()(
              std::to_string(p.layer) +","+
              std::to_string(p.r) +","+
              std::to_string(p.z)
              );
    }
  };

}    

namespace det {
class DigitalCalorimeterSD : public G4VSensitiveDetector
{
  public:
  /** Constructor.
   *  @param aDetectorName Name of the detector
   *  @param aReadoutName Name of the readout (used to name the collection)
   *  @param aSeg Segmentation of the detector (used to retrieve the cell ID)
   */
  DigitalCalorimeterSD(const std::string& aDetectorName,
    const std::string& aReadoutName,
    const DD4hep::Geometry::Segmentation& aSeg);
  /// Destructor
  virtual ~DigitalCalorimeterSD();
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

  virtual void EndOfEvent(G4HCofThisEvent* aHitsCollections) final;

  virtual bool IsAllowedCellID(unsigned long cid) final;
  virtual void AddCellIDMask(unsigned long cid) final;
  virtual void UpdateCellIDMask() final;

private:
  /// Collection of calorimeter hits
  G4THitsCollection<DD4hep::Simulation::Geant4CalorimeterHit>* m_calorimeterCollection;
  G4THitsCollection<DD4hep::Simulation::Geant4CalorimeterHit>* m_tempCollection;
  /// Segmentation of the detector used to retrieve the cell Ids
  DD4hep::Geometry::Segmentation m_seg;

  std::string m_pixelsOverThresholdFileName;
  std::ofstream m_pixelsOverThresholdFile;

  std::string m_pixelsOverThresholdPerLayerFileName;
  std::ofstream m_pixelsOverThresholdPerLayerFile; 

  std::string m_mipsPerPixelFileName;
  std::ofstream m_mipsPerPixelFile; 

  std::string m_padMultiplicityFileName; 
  std::ofstream m_padMultiplicityFile;

  bool m_headerPrinted;
  int m_incidentParticles;

  std::map<uint64_t, int> m_CellIDsMaskedFromPreviousEvent;

  //TFile* m_rootFile;
  //TH1F* m_padMultiplicity;
};
}

#endif /* DETSENSITIVE_DIGITALCALORIMETERSD_H */
