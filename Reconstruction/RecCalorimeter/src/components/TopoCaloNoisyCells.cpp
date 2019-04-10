#include "TopoCaloNoisyCells.h"

#include "TBranch.h"
#include "TFile.h"
#include "TTree.h"

DECLARE_TOOL_FACTORY(TopoCaloNoisyCells)

TopoCaloNoisyCells::TopoCaloNoisyCells(const std::string& type, const std::string& name, const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ICaloReadCellNoiseMap>(this);
}

StatusCode TopoCaloNoisyCells::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  TFile file(m_fileName.value().c_str(), "READ");
  TTree* tree = nullptr;
  file.GetObject("noisyCells", tree);
  ULong64_t readCellId;
  double readNoisyCells;
  double readNoisyCellsOffset;
  tree->SetBranchAddress("cellId", &readCellId);
  tree->SetBranchAddress("noiseLevel", &readNoisyCells);
  tree->SetBranchAddress("noiseOffset", &readNoisyCellsOffset);
  for (uint i = 0; i < tree->GetEntries(); i++) {
    tree->GetEntry(i);
    m_map.insert(std::pair<uint64_t, std::pair<double, double>>(readCellId, std::make_pair(readNoisyCells, readNoisyCellsOffset)));
  }
  delete tree;
  return sc;
}

StatusCode TopoCaloNoisyCells::finalize() { return GaudiTool::finalize(); }

double TopoCaloNoisyCells::noiseRMS(uint64_t aCellId) { return m_map[aCellId].first; }
double TopoCaloNoisyCells::noiseOffset(uint64_t aCellId) { return m_map[aCellId].second; }
