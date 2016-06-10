#include "DetSensitive/SegmentationHelper.h"

// DD4hep
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

// Geant
#include "G4NavigationHistory.hh"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

namespace det {
namespace segmentation {
uint64_t cellID(const DD4hep::Geometry::Segmentation& aSeg, const G4Step& aStep, bool aPreStepPoint) {
  DD4hep::Simulation::Geant4VolumeManager volMgr =
    DD4hep::Simulation::Geant4Mapping::instance().volumeManager();
  DD4hep::Geometry::VolumeManager::VolumeID volID =
    volMgr.volumeID(aStep.GetPreStepPoint()->GetTouchable());
  if (aSeg.isValid() )  {
    G4ThreeVector global;
    if (aPreStepPoint)
      global = aStep.GetPreStepPoint()->GetPosition();
    else
      global = 0.5*(aStep.GetPreStepPoint()->GetPosition()+aStep.GetPostStepPoint()->GetPosition());
    G4ThreeVector local  = aStep.GetPreStepPoint()->GetTouchable()->
      GetHistory()->GetTopTransform().TransformPoint(global);
    DD4hep::Geometry::Position loc(local.x()*MM_2_CM, local.y()*MM_2_CM, local.z()*MM_2_CM);
    DD4hep::Geometry::Position glob(global.x()*MM_2_CM, global.y()*MM_2_CM, global.z()*MM_2_CM);
    DD4hep::Geometry::VolumeManager::VolumeID cID = aSeg.cellID(loc,glob,volID);
    return cID;
  }
  return volID;
}
}
}
