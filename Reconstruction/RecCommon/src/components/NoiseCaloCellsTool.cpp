#include "NoiseCaloCellsTool.h"

DECLARE_TOOL_FACTORY(NoiseCaloCellsTool)

NoiseCaloCellsTool::NoiseCaloCellsTool(const std::string& type,const std::string& name, const IInterface* parent) 
  : GaudiTool(type, name, parent),
  m_cellNoise(1.)
{
  declareInterface<INoiseCaloCellsTool>(this);
  declareProperty("cellNoise", m_cellNoise);
}

NoiseCaloCellsTool::~NoiseCaloCellsTool()
{
}

StatusCode NoiseCaloCellsTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  if(service( "RndmGenSvc" , m_randSvc ).isFailure()) {
    error() << "Couldn't get RndmGenSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  m_gauss.initialize(m_randSvc, Rndm::Gauss(0,m_cellNoise));

  info() << "NoiseCaloCellsTool initialized" << endmsg;
  info() << "Cell noise " <<  m_cellNoise << endmsg;
  return sc;
}

void NoiseCaloCellsTool::AddNoise(std::vector<fcc::CaloHit*>& aCells) {
  //First implementation: same noise to all cells
  double randomNoise = 0;
  for (auto& ecells : aCells) {
    randomNoise = m_gauss.shoot();
    debug() << "random noise " << randomNoise << endmsg;
    ecells->Core().Energy += randomNoise;
  }
}

StatusCode NoiseCaloCellsTool::finalize() {
  StatusCode sc = GaudiTool::finalize();
  info() << "NoiseCaloCellsTool finalized" << endmsg;
  return sc;
}
