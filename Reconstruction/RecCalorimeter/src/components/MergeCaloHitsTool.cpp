#include "MergeCaloHitsTool.h"

// our EDM
#include "datamodel/CaloHit.h"

DECLARE_TOOL_FACTORY(MergeCaloHitsTool)

MergeCaloHitsTool::MergeCaloHitsTool(const std::string& type,const std::string& name, const IInterface* parent) 
  : GaudiTool(type, name, parent)
{
  declareInterface<IMergeCaloHitsTool>(this);

}

MergeCaloHitsTool::~MergeCaloHitsTool()
{
}

StatusCode MergeCaloHitsTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  return sc;

}

std::vector<fcc::CaloHit*> MergeCaloHitsTool::mergeOneCollection(const fcc::CaloHitCollection& aHits) {

  //Vector of merged hits
  std::vector<fcc::CaloHit*> mergedHitsVector;
  mergedHitsVector.reserve(aHits.size());
  //Initialize a map: cellID, index in the vector of cells
  std::unordered_map<uint64_t, fcc::CaloHit*> cellIdMap;
 
  //Loop through all hits and merge energy for hits with same cellID
  mergeHits(aHits, cellIdMap, mergedHitsVector);

  return mergedHitsVector;

}

std::vector<fcc::CaloHit*> MergeCaloHitsTool::mergeTwoVectors(const std::vector<fcc::CaloHit*>& aHits1, const std::vector<fcc::CaloHit*>& aHits2) {

  //Vector of merged hits
  std::vector<fcc::CaloHit*> mergedHitsVector;
  mergedHitsVector.reserve(aHits2.size()); //aHits2 - noise per each cell
  //Prepare a map: cellID, index in the vector of cells
  std::unordered_map<uint64_t, fcc::CaloHit*> cellIdMap;

  //Merge second vector of hits, prepare map
  mergeHits(aHits2, cellIdMap, mergedHitsVector);
  //Merge with first vector of hits
  mergeHits(aHits1, cellIdMap, mergedHitsVector);

  return mergedHitsVector;
}

void MergeCaloHitsTool::mergeHits(const std::vector<fcc::CaloHit*>& aHits, std::unordered_map<uint64_t, fcc::CaloHit*>& cellIdMap, std::vector<fcc::CaloHit*>& mergedHitsVector) {

  //Loop through hits in the vector and merge
  //Time and Bits: not used, a copy of the last entry at the moment
  for (const auto& ehit : aHits) {
    uint64_t cellID = ehit->core().cellId;
    auto cell = cellIdMap[cellID];
    if (cell != nullptr) {
      cell->core().energy += ehit->core().energy;
      cell->core().time = ehit->core().time;
      cell->core().bits = ehit->core().bits;
    }
    else {
      addHitToVectorAndMap(*ehit, cellIdMap, mergedHitsVector);
    }
  }

}


void MergeCaloHitsTool::mergeHits(const fcc::CaloHitCollection& aHits, std::unordered_map<uint64_t, fcc::CaloHit*>& cellIdMap, std::vector<fcc::CaloHit*>& mergedHitsVector) {

  //Loop through hits in the vector and merge
  //Time and Bits: not used, a copy of the last entry at the moment
  for (const auto& ehit : aHits) {
    uint64_t cellID = ehit.core().cellId;
    auto cell = cellIdMap[cellID];
    if (cell != nullptr) {
      cell->core().energy += ehit.core().energy;
      cell->core().time = ehit.core().time;
      cell->core().bits = ehit.core().bits;
    }
    else {
      addHitToVectorAndMap(ehit, cellIdMap, mergedHitsVector);
    }
  }

}

void MergeCaloHitsTool::addHitToVectorAndMap(const fcc::CaloHit& ehit, std::unordered_map<uint64_t, fcc::CaloHit*>& cellIdMap,std::vector<fcc::CaloHit*>& mergedHitsVector) {

  fcc::CaloHit *newHit = new fcc::CaloHit();
  newHit->core().energy = ehit.core().energy;
  newHit->core().time = ehit.core().time;
  newHit->core().bits = ehit.core().bits;
  newHit->core().cellId = ehit.core().cellId;
  mergedHitsVector.push_back(newHit);
  cellIdMap[ehit.core().cellId] = newHit;  

}

StatusCode MergeCaloHitsTool::finalize() {
  StatusCode sc = GaudiTool::finalize();
  return sc;
}
