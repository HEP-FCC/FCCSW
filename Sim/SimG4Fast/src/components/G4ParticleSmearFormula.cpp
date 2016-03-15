#include "G4ParticleSmearFormula.h"

// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"

//ROOT
#include "TFormula.h"

//CLHEP
#include "CLHEP/Vector/ThreeVector.h"

DECLARE_TOOL_FACTORY(G4ParticleSmearFormula)

G4ParticleSmearFormula::G4ParticleSmearFormula(const std::string& type, const std::string& name, const IInterface* parent):
    GaudiTool(type, name, parent),
    m_resolutionEnergy(nullptr),
    m_resolutionMomentum(nullptr) {
  declareInterface<IG4ParticleSmearTool>(this);
  declareProperty("resolutionEnergy", m_resolutionEnergyStr = "");
  declareProperty("resolutionMomentum", m_resolutionMomentumStr = "");
}

G4ParticleSmearFormula::~G4ParticleSmearFormula() {}

StatusCode G4ParticleSmearFormula::initialize() {
  if(GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if(service( "RndmGenSvc" , m_randSvc ).isFailure()) {
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

StatusCode G4ParticleSmearFormula::finalize() {
  return GaudiTool::finalize();
}

StatusCode G4ParticleSmearFormula::smearMomentum( CLHEP::Hep3Vector& aMom, int /*aPdg*/) {
  if(!m_resolutionMomentum->IsValid()) {
      error()<<"Unable to smear particle's momentum - no resolution given!"<<endmsg;
      return StatusCode::FAILURE;
    }
  m_gauss.initialize(m_randSvc, Rndm::Gauss(1,m_resolutionMomentum->Eval(aMom.mag())));
  double tmp = m_gauss.shoot();
  aMom *= tmp;
  return StatusCode::SUCCESS;
}

StatusCode G4ParticleSmearFormula::smearEnergy( double& aEn, int /*aPdg*/) {
  if(!m_resolutionEnergy->IsValid()) {
      error()<<"Unable to smear particle's energy - no resolution given!"<<endmsg;
      return StatusCode::FAILURE;
    }
  m_gauss.initialize(m_randSvc, Rndm::Gauss(1,m_resolutionEnergy->Eval(aEn)));
  double tmp;
  do {
    tmp = m_gauss.shoot();
    aEn *= tmp;
  }
  while(aEn<0);
  return StatusCode::SUCCESS;
}
