#include "DetCommon/DetUtils.h"

// DD4hep
#include "DDG4/Geant4Mapping.h"
#include "DDG4/Geant4VolumeManager.h"

// Geant
#include "G4NavigationHistory.hh"

// ROOT
#include "TGeoBBox.h"

#ifdef HAVE_GEANT4_UNITS
#define MM_2_CM 1.0
#else
#define MM_2_CM 0.1
#endif


namespace det {
namespace utils {
dd4hep::xml::Component getNodeByStrAttr(const dd4hep::xml::Handle_t& mother, const std::string& nodeName,
                                        const std::string& attrName, const std::string& attrValue) {
  for (dd4hep::xml::Collection_t xCompColl(mother, nodeName.c_str()); nullptr != xCompColl; ++xCompColl) {
    if (xCompColl.attr<std::string>(attrName.c_str()) == attrValue) {
      return static_cast<dd4hep::xml::Component>(xCompColl);
    }
  }
  // in case there was no xml daughter with matching name
  return dd4hep::xml::Component(nullptr);
}

double getAttrValueWithFallback(const dd4hep::xml::Component& node, const std::string& attrName,
                                const double& defaultValue) {
  if (node.hasAttr(_Unicode(attrName.c_str()))) {
    return node.attr<double>(attrName.c_str());
  } else {
    return defaultValue;
  }
}

uint64_t cellID(const dd4hep::Segmentation& aSeg, const G4Step& aStep, bool aPreStepPoint) {
  dd4hep::sim::Geant4VolumeManager volMgr = dd4hep::sim::Geant4Mapping::instance().volumeManager();
  dd4hep::VolumeID volID = volMgr.volumeID(aStep.GetPreStepPoint()->GetTouchable());
  if (aSeg.isValid()) {
    G4ThreeVector global;
    if (aPreStepPoint) {
      global = aStep.GetPreStepPoint()->GetPosition();
    } else {
      global = 0.5 * (aStep.GetPreStepPoint()->GetPosition() + aStep.GetPostStepPoint()->GetPosition());
    }
    G4ThreeVector local =
        aStep.GetPreStepPoint()->GetTouchable()->GetHistory()->GetTopTransform().TransformPoint(global);
    dd4hep::Position loc(local.x() * MM_2_CM, local.y() * MM_2_CM, local.z() * MM_2_CM);
    dd4hep::Position glob(global.x() * MM_2_CM, global.y() * MM_2_CM, global.z() * MM_2_CM);
    dd4hep::VolumeID cID = aSeg.cellID(loc, glob, volID);
    return cID;
  }
  return volID;
}

std::vector<std::vector<uint>> combinations(int N, int K) {
  std::vector<std::vector<uint>> indexes;
  std::string bitmask(K, 1);  // K leading 1's
  bitmask.resize(N, 0);       // N-K trailing 0's
  // permute bitmask
  do {
    std::vector<uint> tmp;
    for (int i = 0; i < N; ++i) {  // [0..N-1] integers
      if (bitmask[i]) {
        tmp.push_back(i);
      }
    }
    indexes.push_back(tmp);
  } while (std::prev_permutation(bitmask.begin(), bitmask.end()));
  return std::move(indexes);
}

std::vector<std::vector<int>> permutations(int K) {
  std::vector<std::vector<int>> indexes;
  int N = pow(2, K);  // number of permutations with repetition of 2 numbers (0,1)
  for (int i = 0; i < N; i++) {
    // permutation = binary representation of i
    std::vector<int> tmp;
    tmp.assign(K, 0);
    uint res = i;
    // dec -> bin
    for (int j = 0; j < K; j++) {
      tmp[K - 1 - j] = -1 + 2 * (res % 2);
      res = floor(res / 2);
    }
    indexes.push_back(tmp);
  }
  return std::move(indexes);
}

int cyclicNeighbour(int aCyclicId, std::pair<int, int> aFieldExtremes) {
  if (aCyclicId < aFieldExtremes.first) {
    return aFieldExtremes.second + aCyclicId;
  } else if (aCyclicId > aFieldExtremes.second) {
    return aCyclicId % (aFieldExtremes.second + 1);
  }
  return aCyclicId;
}

std::vector<uint64_t> neighbours(const dd4hep::DDSegmentation::BitFieldCoder& aDecoder,
                                 const std::vector<std::string>& aFieldNames,
                                 const std::vector<std::pair<int, int>>& aFieldExtremes, uint64_t aCellId,
                                 const std::vector<bool>& aFieldCyclic, bool aDiagonal) {
  std::vector<uint64_t> neighbours;
  dd4hep::DDSegmentation::CellID cID = aCellId;
  for (uint itField = 0; itField < aFieldNames.size(); itField++) {
    const auto& field = aFieldNames[itField];
    dd4hep::DDSegmentation::CellID id = aDecoder.get(cID,field);
    if (aFieldCyclic[itField]) {
      aDecoder[field].set(cID, cyclicNeighbour(id - 1, aFieldExtremes[itField]));
      neighbours.emplace_back(cID);
      aDecoder[field].set(cID, cyclicNeighbour(id + 1, aFieldExtremes[itField]));
      neighbours.emplace_back(cID);
    } else {
      if (id > aFieldExtremes[itField].first) {
        aDecoder.set(cID, field, id - 1);
        neighbours.emplace_back(cID);
      }
      if (id < aFieldExtremes[itField].second) {
        aDecoder.set(cID, field, id + 1);
        neighbours.emplace_back(cID);
      }
    }
    aDecoder.set(cID, field, id);
  }
  if (aDiagonal) {
    std::vector<int> fieldIds;  // initial IDs
    fieldIds.assign(aFieldNames.size(), 0);
    // for each field get current Id
    for (uint iField = 0; iField < aFieldNames.size(); iField++) {
      const auto& field = aFieldNames[iField];
      fieldIds[iField] = aDecoder.get(cID, field);
    }
    for (uint iLength = aFieldNames.size(); iLength > 1; iLength--) {
      // get all combinations for a given length
      const auto& indexes = combinations(aFieldNames.size(), iLength);
      for (uint iComb = 0; iComb < indexes.size(); iComb++) {
        // for current combination get all permutations of +- 1 operation on IDs
        const auto& calculation = permutations(iLength);
        // do the increase/decrease of bitfield
        for (uint iCalc = 0; iCalc < calculation.size(); iCalc++) {
          // set new Ids for each field combination
          bool add = true;
          for (uint iField = 0; iField < indexes[iComb].size(); iField++) {
            if (aFieldCyclic[indexes[iComb][iField]]) {
              aDecoder[aFieldNames[indexes[iComb][iField]]].set(cID, cyclicNeighbour(fieldIds[indexes[iComb][iField]] + calculation[iCalc][iField],
										     aFieldExtremes[indexes[iComb][iField]]) );
            } else if ((calculation[iCalc][iField] > 0 &&
                        fieldIds[indexes[iComb][iField]] < aFieldExtremes[indexes[iComb][iField]].second) ||
                       (calculation[iCalc][iField] < 0 &&
                        fieldIds[indexes[iComb][iField]] > aFieldExtremes[indexes[iComb][iField]].first)) {
              aDecoder[aFieldNames[indexes[iComb][iField]]].set(cID, fieldIds[indexes[iComb][iField]] + calculation[iCalc][iField]);
            } else {
              add = false;
            }
          }
          // add new cellId to neighbours (unless it's beyond extrema)
          if (add) {
            neighbours.emplace_back(cID);
          }
          // reset ids
          for (uint iField = 0; iField < indexes[iComb].size(); iField++) {
            aDecoder[aFieldNames[indexes[iComb][iField]]].set(cID, fieldIds[indexes[iComb][iField]]);
          }
        }
      }
    }
  }
  return std::move(neighbours);
}

std::vector<std::pair<int, int>> bitfieldExtremes(const dd4hep::DDSegmentation::BitFieldCoder& aDecoder,
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
  dd4hep::VolumeManager volMgr = dd4hep::Detector::getInstance().volumeManager();
  auto pvol = volMgr.lookupVolumePlacement(aVolumeId);
  auto solid = pvol.volume().solid();
  // get the envelope of the shape
  TGeoBBox* box = (dynamic_cast<TGeoBBox*>(solid.ptr()));
  // get half-widths
  return CLHEP::Hep3Vector(box->GetDX(), box->GetDY(), box->GetDZ());
}

std::array<uint, 2> numberOfCells(uint64_t aVolumeId, const dd4hep::DDSegmentation::CartesianGridXY& aSeg) {
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

std::array<uint, 3> numberOfCells(uint64_t aVolumeId, const dd4hep::DDSegmentation::CartesianGridXYZ& aSeg) {
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
  dd4hep::VolumeManager volMgr = dd4hep::Detector::getInstance().volumeManager();
  auto pvol = volMgr.lookupVolumePlacement(aVolumeId);
  auto solid = pvol.volume().solid();

  // get the envelope of the shape
  TGeoTubeSeg* tube = (dynamic_cast<TGeoTubeSeg*>(solid.ptr()));
  if (tube == nullptr) {
    return CLHEP::Hep3Vector(0, 0, 0);
  }
  // get half-widths
  return CLHEP::Hep3Vector(tube->GetRmin(), tube->GetRmax(), tube->GetDZ());
}

CLHEP::Hep3Vector coneDimensions(uint64_t aVolumeId) {
  dd4hep::VolumeManager volMgr = dd4hep::Detector::getInstance().volumeManager();
  auto pvol = volMgr.lookupVolumePlacement(aVolumeId);
  auto solid = pvol.volume().solid();
  // get the envelope of the shape
  TGeoCone* cone = (dynamic_cast<TGeoCone*>(solid.ptr()));
  if (cone == nullptr) {
    return CLHEP::Hep3Vector(0, 0, 0);
  }
  // get half-widths
  return CLHEP::Hep3Vector(cone->GetRmin1(), cone->GetRmax1(), cone->GetDZ());
}

std::array<double, 2> tubeEtaExtremes(uint64_t aVolumeId) {
  auto sizes = tubeDimensions(aVolumeId);
  if (sizes.mag() == 0) {
    // if it is not a cylinder maybe it is a cone (same calculation for extremes)
    sizes = coneDimensions(aVolumeId);
    if (sizes.mag() == 0) {
      return {0, 0};
    }
  }
  // eta segmentation calculate maximum eta from the inner radius (no offset is taken into account)
  double maxEta = 0;
  double minEta = 0;
  // check if it is a cylinder centred at z=0
  dd4hep::VolumeManager volMgr = dd4hep::Detector::getInstance().volumeManager();
  auto detelement = volMgr.lookupDetElement(aVolumeId);
  const auto& transformMatrix = detelement.nominal().worldTransformation();
  double outGlobal[3];
  double inLocal[] = {0, 0, 0};  // to get middle of the volume
  transformMatrix.LocalToMaster(inLocal, outGlobal);
  if (outGlobal[2] < 1e-10) {
    // this assumes cylinder centred at z=0
    maxEta = CLHEP::Hep3Vector(sizes.x(), 0, sizes.z()).eta();
    minEta = -maxEta;
  } else {
    maxEta = CLHEP::Hep3Vector(sizes.x(), 0, std::max(sizes.z() + outGlobal[2], -sizes.z() + outGlobal[2])).eta();
    minEta = CLHEP::Hep3Vector(sizes.y(), 0, std::min(sizes.z() + outGlobal[2], -sizes.z() + outGlobal[2])).eta();
  }
  return {minEta, maxEta};
}

std::array<double, 2> envelopeEtaExtremes (uint64_t aVolumeId) {
  dd4hep::VolumeManager volMgr = dd4hep::Detector::getInstance().volumeManager();
  auto detelement = volMgr.lookupDetElement(aVolumeId);
  const auto& transformMatrix = detelement.nominal().worldTransformation();
  // calculate values of eta in all possible corners of the envelope
  auto dim = envelopeDimensions(aVolumeId);
  double minEta = 0;
  double maxEta = 0;
  for (uint i = 0; i < 8; i++) {
    // coefficients to get all combinations of corners
    int iX = -1 + 2 * ((i / 2) % 2);
    int iY = -1 + 2 * (i % 2);
    int iZ = -1 + 2 * (i / 4);
    double outDimGlobal[3];
    double inDimLocal[] = {iX * dim.x(), iY * dim.y(), iZ * dim.z()};
    transformMatrix.LocalToMaster(inDimLocal, outDimGlobal);
    double eta = CLHEP::Hep3Vector(outDimGlobal[0], outDimGlobal[1], outDimGlobal[2]).eta();
    if (i == 0) {
      minEta = eta;
      maxEta = eta;
    }
    if (eta < minEta) {
      minEta = eta;
    }
    if (eta > maxEta) {
      maxEta = eta;
    }
  }
  return {minEta, maxEta};
}

std::array<double, 2> volumeEtaExtremes(uint64_t aVolumeId) {
  // try if volume is a cylinder/disc
  auto etaExtremes = tubeEtaExtremes(aVolumeId);
  if (etaExtremes[0] != 0 or etaExtremes[1] != 0) {
    return etaExtremes;
  } else {
    return envelopeEtaExtremes(aVolumeId);
  }
}

std::array<uint, 3> numberOfCells(uint64_t aVolumeId, const dd4hep::DDSegmentation::FCCSWGridPhiEta& aSeg) {
  // get segmentation number of bins in phi
  uint phiCellNumber = aSeg.phiBins();
  // get segmentation cell width in eta
  double etaCellSize = aSeg.gridSizeEta();
  // get min and max eta of the volume
  auto etaExtremes = volumeEtaExtremes(aVolumeId);
  // calculate the number of eta volumes
  // max - min = full eta range, - size = not counting the middle cell centred at 0, + 1 to account for that cell
  uint cellsEta = ceil(( etaExtremes[1] - etaExtremes[0] - etaCellSize ) / 2 / etaCellSize) * 2 + 1;
  uint minEtaID = int(floor((etaExtremes[0] + 0.5 * etaCellSize - aSeg.offsetEta()) / etaCellSize));
  return {phiCellNumber, cellsEta, minEtaID};
}

std::array<uint, 2> numberOfCells(uint64_t aVolumeId, const dd4hep::DDSegmentation::PolarGridRPhi& aSeg) {
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
