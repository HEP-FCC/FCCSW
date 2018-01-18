#include "DetCommon/DetUtils.h"

// DD4hep
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

// Geant
#include "G4NavigationHistory.hh"

// ROOT
#include "TGeoBBox.h"

namespace det {
namespace utils {
DD4hep::XML::Component getNodeByStrAttr(const DD4hep::XML::Handle_t& mother, const std::string& nodeName,
                                        const std::string& attrName, const std::string& attrValue) {
  for (DD4hep::XML::Collection_t xCompColl(mother, nodeName.c_str()); nullptr != xCompColl; ++xCompColl) {
    if (xCompColl.attr<std::string>(attrName.c_str()) == attrValue) {
      return static_cast<DD4hep::XML::Component>(xCompColl);
    }
  }
  // in case there was no xml daughter with matching name
  return DD4hep::XML::Component(nullptr);
}

double getAttrValueWithFallback(const DD4hep::XML::Component& node, const std::string& attrName,
                                const double& defaultValue) {
  if (node.hasAttr(_Unicode(attrName.c_str()))) {
    return node.attr<double>(attrName.c_str());
  } else {
    return defaultValue;
  }
}

uint64_t cellID(const DD4hep::Geometry::Segmentation& aSeg, const G4Step& aStep, bool aPreStepPoint) {
  DD4hep::Simulation::Geant4VolumeManager volMgr = DD4hep::Simulation::Geant4Mapping::instance().volumeManager();
  DD4hep::Geometry::VolumeManager::VolumeID volID = volMgr.volumeID(aStep.GetPreStepPoint()->GetTouchable());
  if (aSeg.isValid()) {
    G4ThreeVector global;
    if (aPreStepPoint) {
      global = aStep.GetPreStepPoint()->GetPosition();
    } else {
      global = 0.5 * (aStep.GetPreStepPoint()->GetPosition() + aStep.GetPostStepPoint()->GetPosition());
    }
    G4ThreeVector local =
        aStep.GetPreStepPoint()->GetTouchable()->GetHistory()->GetTopTransform().TransformPoint(global);
    DD4hep::Geometry::Position loc(local.x() * MM_2_CM, local.y() * MM_2_CM, local.z() * MM_2_CM);
    DD4hep::Geometry::Position glob(global.x() * MM_2_CM, global.y() * MM_2_CM, global.z() * MM_2_CM);
    DD4hep::Geometry::VolumeManager::VolumeID cID = aSeg.cellID(loc, glob, volID);
    return cID;
  }
  return volID;
}

std::vector<uint64_t> neighbours(DD4hep::DDSegmentation::BitField64& aDecoder,
                                 const std::vector<std::string>& aFieldNames,
                                 const std::vector<std::pair<int, int>>& aFieldExtremes,
                                 uint64_t aCellId,
				 int aSteps
				 ) {
  std::vector<uint64_t> neighbours;
  aDecoder.setValue(aCellId);
  for (uint itField = 0; itField < aFieldNames.size(); itField++) {
    const auto& field = aFieldNames[itField];
    int id = aDecoder[field];
    for (int step=1; step<=aSteps; step++) {
      if (id > aFieldExtremes[itField].first + step) {
	aDecoder[field] = id - step;
	neighbours.emplace_back(aDecoder.getValue());
      }
      if(id < aFieldExtremes[itField].second - step){
     	aDecoder[field] = id + step;
	neighbours.emplace_back(aDecoder.getValue());
      }
    }
    aDecoder[field] = id;
  }
  return std::move(neighbours);
}

std::vector<std::pair<int, int>> bitfieldExtremes(DD4hep::DDSegmentation::BitField64& aDecoder,
                                                  const std::vector<std::string>& aFieldNames) {
  std::vector<std::pair<int, int>> extremes;
  int width = 0;
  for (const auto& field : aFieldNames) {
    width = aDecoder[field].width();
    if (aDecoder[field].isSigned()) {
      extremes.emplace_back(std::make_pair(-(1 << (width - 1)), (1 << (width - 1)) - 1));
    } else {
      extremes.emplace_back(std::make_pair(0, (1 << width) - 1));
    }
  }
  return std::move(extremes);
}

CLHEP::Hep3Vector envelopeDimensions(uint64_t aVolumeId) {
  DD4hep::Geometry::VolumeManager volMgr = DD4hep::Geometry::LCDD::getInstance().volumeManager();
  auto pvol = volMgr.lookupPlacement(aVolumeId);
  auto solid = pvol.volume().solid();
  // get the envelope of the shape
  TGeoBBox* box = (dynamic_cast<TGeoBBox*>(solid.ptr()));
  // get half-widths
  return CLHEP::Hep3Vector(box->GetDX(), box->GetDY(), box->GetDZ());
}

std::array<uint, 2> numberOfCells(uint64_t aVolumeId, const DD4hep::DDSegmentation::CartesianGridXY& aSeg) {
  // // get half-widths
  auto halfSizes = envelopeDimensions(aVolumeId);
  // get segmentation cell widths
  double xCellSize = aSeg.gridSizeX();
  double yCellSize = aSeg.gridSizeY();
  // calculate number of cells, the middle cell is centred at 0 (no offset)
  uint cellsX = ceil((halfSizes.x() - xCellSize / 2.) / xCellSize) * 2 + 1;
  uint cellsY = ceil((halfSizes.y() - yCellSize / 2.) / yCellSize) * 2 + 1;
  return {cellsX, cellsY};
}

std::array<uint, 3> numberOfCells(uint64_t aVolumeId, const DD4hep::DDSegmentation::CartesianGridXYZ& aSeg) {
  // // get half-widths
  auto halfSizes = envelopeDimensions(aVolumeId);
  // get segmentation cell widths
  double xCellSize = aSeg.gridSizeX();
  double yCellSize = aSeg.gridSizeY();
  double zCellSize = aSeg.gridSizeZ();
  // calculate number of cells, the middle cell is centred at 0 (no offset)
  uint cellsX = ceil((halfSizes.x() - xCellSize / 2.) / xCellSize) * 2 + 1;
  uint cellsY = ceil((halfSizes.y() - yCellSize / 2.) / yCellSize) * 2 + 1;
  uint cellsZ = ceil((halfSizes.z() - zCellSize / 2.) / zCellSize) * 2 + 1;
  return {cellsX, cellsY, cellsZ};
}

CLHEP::Hep3Vector tubeDimensions(uint64_t aVolumeId) {
  DD4hep::Geometry::VolumeManager volMgr = DD4hep::Geometry::LCDD::getInstance().volumeManager();
  auto pvol = volMgr.lookupPlacement(aVolumeId);
  auto solid = pvol.volume().solid();
  // get the envelope of the shape
  TGeoConeSeg* tube = (dynamic_cast<TGeoConeSeg*>(solid.ptr()));
  if (tube == nullptr) {
    return CLHEP::Hep3Vector(0, 0, 0);
  }
  // get half-widths
  return CLHEP::Hep3Vector(tube->GetRmin1(), tube->GetRmax1(), tube->GetDZ());
}

std::array<uint, 2> numberOfCells(uint64_t aVolumeId, const DD4hep::DDSegmentation::GridPhiEta& aSeg) {
  // get half-widths,
  auto tubeSizes = tubeDimensions(aVolumeId);
  // get segmentation number of bins in phi
  uint phiCellNumber = aSeg.phiBins();
  // get segmentation cell width in eta
  double etaCellSize = aSeg.gridSizeEta();
  // eta segmentation calculate maximum eta from the inner radius (no offset is taken into account)
  double maxEta = CLHEP::Hep3Vector(tubeSizes.x(), 0, tubeSizes.z()).eta();
  uint cellsEta = ceil((maxEta - etaCellSize / 2.) / etaCellSize) * 2 + 1;
  return {phiCellNumber, cellsEta};
}

std::array<uint, 2> numberOfCells(uint64_t aVolumeId, const DD4hep::DDSegmentation::PolarGridRPhi& aSeg) {
  // get half-widths,
  auto tubeSizes = tubeDimensions(aVolumeId);
  // get segmentation cell width
  double rCellSize = aSeg.gridSizeR();
  double phiCellSize = aSeg.gridSizePhi();
  uint cellsRout = ceil(tubeSizes.y() / rCellSize);
  uint cellsRin = floor(tubeSizes.x() / rCellSize);
  uint cellsR = cellsRout - cellsRin;
  uint cellsPhi = ceil(2 * M_PI / phiCellSize);
  return {cellsR, cellsPhi};
}

unsigned int countPlacedVolumes(TGeoVolume* aHighestVolume, const std::string& aMatchName) {
  int numberOfPlacedVolumes = 0;
  TGeoNode* node;
  TGeoIterator next(aHighestVolume);
  while ((node = next())) {
    std::string currentNodeName = node->GetName();
    if (currentNodeName.find(aMatchName) != std::string::npos) {
      ++numberOfPlacedVolumes;
    }
  }
  return numberOfPlacedVolumes;
}
}
}
