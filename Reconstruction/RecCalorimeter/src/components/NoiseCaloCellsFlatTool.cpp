#include "NoiseCaloCellsFlatTool.h"

DECLARE_TOOL_FACTORY(NoiseCaloCellsFlatTool)

NoiseCaloCellsFlatTool::NoiseCaloCellsFlatTool(const std::string& type, const std::string& name,
                                               const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<INoiseCaloCellsTool>(this);
}

StatusCode NoiseCaloCellsFlatTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  // Initialize random service
  if (service("RndmGenSvc", m_randSvc).isFailure()) {
    error() << "Couldn't get RndmGenSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  m_gauss.initialize(m_randSvc, Rndm::Gauss(0., 1.));

  info() << "Sigma of the cell noise: " << m_cellNoise * 1.e3 << " MeV" << endmsg;
  info() << "Filter noise threshold: " << m_filterThreshold << "*sigma" << endmsg;
  return sc;
}

void NoiseCaloCellsFlatTool::addRandomCellNoise(std::unordered_map<uint64_t, double>& aCells) {
  std::for_each(aCells.begin(), aCells.end(),
                [this](std::pair<const uint64_t, double>& p) { p.second += (m_gauss.shoot() * m_cellNoise); });
}

void NoiseCaloCellsFlatTool::filterCellNoise(std::unordered_map<uint64_t, double>& aCells) {
  // Erase a cell if it has energy below a threshold
  double threshold = m_filterThreshold * m_cellNoise;
  auto it = aCells.begin();
  while ((it = std::find_if(it, aCells.end(), [&threshold](std::pair<const uint64_t, double>& p) {
            return bool(p.second < threshold);
          })) != aCells.end()) {
    aCells.erase(it++);
  }
}

StatusCode NoiseCaloCellsFlatTool::finalize() { return GaudiTool::finalize(); }
