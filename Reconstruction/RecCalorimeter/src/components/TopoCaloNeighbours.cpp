#include "TopoCaloNeighbours.h"

#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

DECLARE_TOOL_FACTORY(TopoCaloNeighbours)

TopoCaloNeighbours::TopoCaloNeighbours(const std::string& type, const std::string& name,
                                                 const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ICaloReadNeighboursMap>(this);
}

StatusCode TopoCaloNeighbours::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  TFile file(m_fileName.value().c_str(),"READ");
  TTree* tree = nullptr;
  file.GetObject("neighbours",tree);
  ULong64_t readCellId;
  std::vector<uint64_t>* readNeighbours = nullptr;
  tree->SetBranchAddress("cellId",&readCellId);
  tree->SetBranchAddress("neighbours",&readNeighbours);
  for (uint i = 0; i < tree->GetEntries(); i++) {
      tree->GetEntry(i);
      m_map.insert(std::pair<uint64_t, std::vector<uint64_t>>(readCellId, *readNeighbours));
  }
  std::vector<int> counterL;
  counterL.assign(100,0);
  for(const auto& item: m_map) {
    counterL[item.second.size()] ++;
  }
  for(uint iCount = 0; iCount < counterL.size(); iCount++) {
    if (counterL[iCount] != 0) {
      info() << counterL[iCount] << " cells have " << iCount << " neighbours" << endmsg;
    }
  }
  delete tree;
  delete readNeighbours;
  return sc;
}

StatusCode TopoCaloNeighbours::finalize() { return GaudiTool::finalize(); }

std::vector<uint64_t>& TopoCaloNeighbours::neighbours(uint64_t aCellId) {
  return m_map[aCellId];
}
