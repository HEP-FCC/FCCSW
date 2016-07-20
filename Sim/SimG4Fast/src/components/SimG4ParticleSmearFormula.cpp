#include "SimG4ParticleSmearFormula.h"

// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IRndmGen.h"

//ROOT
#include "TFormula.h"

//CLHEP
#include "CLHEP/Vector/ThreeVector.h"

DECLARE_TOOL_FACTORY(SimG4ParticleSmearFormula)

SimG4ParticleSmearFormula::SimG4ParticleSmearFormula(const std::string& type, const std::string& name, const IInterface* parent):
    GaudiTool(type, name, parent),
    m_resolutionEnergy(nullptr),
    m_resolutionMomentum(nullptr) {
  declareInterface<ISimG4ParticleSmearTool>(this);
  declareProperty("resolutionEnergy", m_resolutionEnergyStr = "");
  declareProperty("resolutionMomentum", m_resolutionMomentumStr = "");
}

SimG4ParticleSmearFormula::~SimG4ParticleSmearFormula() {}

StatusCode SimG4ParticleSmearFormula::initialize() {
  if(GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  m_randSvc = service("RndmGenSvc");
  if(!m_randSvc) {
    error() << "Couldn't get RndmGenSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  if (!m_resolutionEnergyStr.empty()) {
    m_resolutionEnergy = std::unique_ptr<TFormula>(new TFormula("edep",m_resolutionEnergyStr.c_str()));
    info() << "Energy-dependent resolutions: "<< m_resolutionEnergy->GetExpFormula() << endmsg;
  } else {
    info() << "No energy-dependent resolutions defined."<< endmsg;
  }
  if (!m_resolutionMomentumStr.empty()) {
    m_resolutionMomentum = std::unique_ptr<TFormula>(new TFormula("pdep",m_resolutionMomentumStr.c_str()));
    info() << "Momentum-dependent resolutions: "<< m_resolutionMomentum->GetExpFormula() << endmsg;
  } else {
    info() << "No momentum-dependent resolutions defined."<< endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode SimG4ParticleSmearFormula::finalize() {
  return GaudiTool::finalize();
}

StatusCode SimG4ParticleSmearFormula::smearMomentum( CLHEP::Hep3Vector& aMom, int /*aPdg*/) {
  if(!m_resolutionMomentum->IsValid()) {
    error()<<"Unable to smear particle's momentum - no resolution given!"<<endmsg;
    return StatusCode::FAILURE;
  }
  m_randSvc->generator(Rndm::Gauss(1,m_resolutionMomentum->Eval(aMom.mag())), m_gauss);
  double tmp = m_gauss->shoot();
  aMom *= tmp;
  return StatusCode::SUCCESS;
}

StatusCode SimG4ParticleSmearFormula::smearEnergy( double& aEn, int /*aPdg*/) {
  if(!m_resolutionEnergy->IsValid()) {
    error()<<"Unable to smear particle's energy - no resolution given!"<<endmsg;
    return StatusCode::FAILURE;
  }
  m_randSvc->generator(Rndm::Gauss(1,m_resolutionEnergy->Eval(aEn)), m_gauss);
  double tmp;
  do {
    tmp = m_gauss->shoot();
    aEn *= tmp;
  }
  while(aEn<0);
  return StatusCode::SUCCESS;
}
