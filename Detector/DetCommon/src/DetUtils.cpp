#include "DetCommon/DetUtils.h"

// DD4hep
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

// Geant
#include "G4NavigationHistory.hh"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

namespace det {
namespace utils {
DD4hep::XML::Component getNodeByStrAttr(const DD4hep::XML::Handle_t& mother, const std::string& nodeName, const std::string& attrName,
                                             const std::string& attrValue) {
  for (DD4hep::XML::Collection_t xCompColl(mother, nodeName.c_str()); nullptr != xCompColl; ++xCompColl) {
    if (xCompColl.attr<std::string>(attrName.c_str()) == attrValue) {
      return static_cast<DD4hep::XML::Component>(xCompColl);
    }
  }
  // in case there was no xml daughter with matching name
  return DD4hep::XML::Component(nullptr);
}

double getAttrValueWithFallback(const DD4hep::XML::Component& node, const std::string& attrName, const double& defaultValue) {
  if (node.hasAttr(attrName.c_str())) {
    return node.attr<double>(attrName.c_str());
  } else {
    return defaultValue;
  }
}

uint64_t cellID(const DD4hep::Geometry::Segmentation& aSeg, const G4Step& aStep, bool aPreStepPoint) {
  DD4hep::Simulation::Geant4VolumeManager volMgr =
    DD4hep::Simulation::Geant4Mapping::instance().volumeManager();
  DD4hep::Geometry::VolumeManager::VolumeID volID =
    volMgr.volumeID(aStep.GetPreStepPoint()->GetTouchable());
  if (aSeg.isValid() )  {
    G4ThreeVector global;
    if (aPreStepPoint) {
      global = aStep.GetPreStepPoint()->GetPosition();
    } else {
      global = 0.5*(aStep.GetPreStepPoint()->GetPosition()+aStep.GetPostStepPoint()->GetPosition());
    }
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
