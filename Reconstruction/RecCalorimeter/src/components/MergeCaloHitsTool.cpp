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

std::vector<fcc::CaloHit*> MergeCaloHitsTool::MergeOneCollection(const fcc::CaloHitCollection& aHits) {

  //Vector of merged hits
  std::vector<fcc::CaloHit*>  mergedHits;

  //Prepare a map: cellID, index in the vector of cells
  std::unordered_map<uint64_t, unsigned> cellIdMap;
 
  //Loop through all hits and merge energy for hits with same cellID
  //Time and Bits: not used, a copy of the last entry at the moment
  for (const auto& ehit : aHits) {
    uint64_t cellID = ehit.Core().Cellid;
    auto it = cellIdMap.find(cellID);
    if (it != end(cellIdMap)) {
      mergedHits.at(it->second)->Core().Energy += ehit.Core().Energy;
      mergedHits.at(it->second)->Core().Time = ehit.Core().Time;
      mergedHits.at(it->second)->Core().Bits = ehit.Core().Bits;
    }
    else {
      AddHitToVectorAndMap(ehit, cellIdMap, mergedHits);
    }
  }

  return mergedHits;

}

std::vector<fcc::CaloHit*> MergeCaloHitsTool::MergeTwoVectors(const std::vector<fcc::CaloHit*>& aHits1, const std::vector<fcc::CaloHit*>& aHits2) {

 //Prepare a map: cellID, index in the vector of cells
  std::unordered_map<uint64_t, unsigned> cellIdMap;

  //Vector of hits and map created from the second vector (unique cellIDs expected)
  std::vector<fcc::CaloHit*> mergedHits = PrepareMergedVectorAndMap(aHits2, cellIdMap);

  //Loop through first vector of hits and merge energy with hits from the second one
  //Time and Bits: not used, a copy of the last entry at the moment
  for (const auto& ehit : aHits1) {
    uint64_t cellID = ehit->Core().Cellid;
    auto it = cellIdMap.find(cellID);
    if (it != end(cellIdMap)) {
      mergedHits.at(it->second)->Core().Energy += ehit->Core().Energy;
      mergedHits.at(it->second)->Core().Time = ehit->Core().Time;
      mergedHits.at(it->second)->Core().Bits = ehit->Core().Bits;
    }
    else {
      AddHitToVectorAndMap(*ehit, cellIdMap, mergedHits);
    }
  }

  return mergedHits;

}

std::vector<fcc::CaloHit*> MergeCaloHitsTool::PrepareMergedVectorAndMap(const std::vector<fcc::CaloHit*>& aHits, std::unordered_map<uint64_t, unsigned>& cellIdMap) {
  //Vector of merged hits
  std::vector<fcc::CaloHit*>  mergedHits;
  //Loop through vector and add hits to the vector, create the map
  for (const auto& ehit : aHits) {
    AddHitToVectorAndMap(*ehit, cellIdMap, mergedHits);
  }
  return mergedHits;

}

void MergeCaloHitsTool::AddHitToVectorAndMap(const fcc::CaloHit& ehit, std::unordered_map<uint64_t, unsigned>& cellIdMap,std::vector<fcc::CaloHit*>& mergedHits) {

  fcc::CaloHit *newHit = new fcc::CaloHit();
  newHit->Core().Energy = ehit.Core().Energy;
  newHit->Core().Time = ehit.Core().Time;
  newHit->Core().Bits = ehit.Core().Bits;
  newHit->Core().Cellid = ehit.Core().Cellid;
  mergedHits.push_back(newHit);
  cellIdMap[ehit.Core().Cellid] = mergedHits.size()-1;  

}

StatusCode MergeCaloHitsTool::finalize() {
  StatusCode sc = GaudiTool::finalize();
  return sc;
}
