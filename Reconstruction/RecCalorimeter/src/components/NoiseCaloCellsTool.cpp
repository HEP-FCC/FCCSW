#include "NoiseCaloCellsTool.h"

DECLARE_TOOL_FACTORY(NoiseCaloCellsTool)

NoiseCaloCellsTool::NoiseCaloCellsTool(const std::string& type,const std::string& name, const IInterface* parent) 
  : GaudiTool(type, name, parent)
{
  declareInterface<INoiseCaloCellsTool>(this);
  declareProperty("cellNoise", m_cellNoise=50.0);
  //remove cells with energy bellow filterThreshold (threshold is multiplied by a cell noise sigma)
  declareProperty("filterNoiseThreshold", m_filterThreshold=3);
}

NoiseCaloCellsTool::~NoiseCaloCellsTool()
{
}

StatusCode NoiseCaloCellsTool::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  //Initialize random service
  if(service( "RndmGenSvc" , m_randSvc ).isFailure()) {
    error() << "Couldn't get RndmGenSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  m_gauss.initialize(m_randSvc, Rndm::Gauss(0,m_cellNoise));

  info() << "Sigma of the cell noise: " <<  m_cellNoise << " MeV" << endmsg;
  info() << "Filter noise threshold: " <<  m_filterThreshold << "*sigma" << endmsg;
  return sc;
}

void NoiseCaloCellsTool::createRandomCellNoise(std::vector<fcc::CaloHit*>& aCells) {
  double randomNoise = 0;
  for (auto& ecell : aCells) {
    randomNoise = m_gauss.shoot();
    ecell->Core().Energy = randomNoise;
    ecell->Core().Time = 0;
    ecell->Core().Bits = 0;
  }
}

void NoiseCaloCellsTool::filterCellNoise(std::vector<fcc::CaloHit*>& aCells) {
  //Erase a cell if it has energy bellow a threshold from the vector
  for (auto ecell = aCells.begin(); ecell!=aCells.end();) {
    if ( (*ecell)->Core().Energy<m_filterThreshold*m_cellNoise ) {
      aCells.erase(ecell);
    }
    else {
      ++ecell;
    }
  }
}


StatusCode NoiseCaloCellsTool::finalize() {
  StatusCode sc = GaudiTool::finalize();
  return sc;
}
