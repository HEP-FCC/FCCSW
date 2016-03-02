#include "G4ParticleSmearSimple.h"

// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"

//ROOT
#include "TFormula.h"

//CLHEP
#include "CLHEP/Vector/ThreeVector.h"

DECLARE_TOOL_FACTORY(G4ParticleSmearSimple)

G4ParticleSmearSimple::G4ParticleSmearSimple(const std::string& type, const std::string& name, const IInterface* parent):
    GaudiTool(type, name, parent),
    m_resolutionEnergy(nullptr),
    m_resolutionMomentum(nullptr),
    m_resolutionTransMomentum(nullptr) {
  declareInterface<IG4ParticleSmearTool>(this);
  declareProperty("sigma", m_sigma = 0.01);
  declareProperty("resolutionEnergy", m_resolutionEnergyStr = "");
  declareProperty("resolutionMomentum", m_resolutionMomentumStr = "");
  declareProperty("resolutionTransverseMomentum", m_resolutionTransMomentumStr = "");
}

G4ParticleSmearSimple::~G4ParticleSmearSimple() {}

StatusCode G4ParticleSmearSimple::initialize() {
  if(GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if(service( "RndmGenSvc" , m_randSvc ).isFailure()) {
    error() << "Couldn't get RndmGenSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  // to be removed
  m_gauss.initialize(m_randSvc, Rndm::Gauss(1,m_sigma));
  info() << "Tool used for smearing particles initialized with sigma = "<<m_sigma << endmsg;

  if (!m_resolutionEnergyStr.empty()) {
    m_resolutionEnergy = std::unique_ptr<TFormula>(new TFormula("endep",m_resolutionEnergyStr.c_str()));
    info() << "Energy-dependent resolutions: "<< m_resolutionEnergyStr << endmsg;
  } else {
    info() << "No energy-dependent resolutions defined."<< endmsg;
  }
  if (!m_resolutionMomentumStr.empty()) {
    info() << "Momentum-dependent resolutions: "<< m_resolutionMomentumStr << endmsg;
  } else {
    info() << "No momentum-dependent resolutions defined."<< endmsg;
  }
  if (!m_resolutionTransMomentumStr.empty()) {
    info() << "Transverse-momentum-dependent resolutions: "<< m_resolutionTransMomentumStr << endmsg;
  } else {
    info() << "No transverse-momentum-dependent resolutions defined."<< endmsg;
  }
  return StatusCode::SUCCESS;
}

StatusCode G4ParticleSmearSimple::finalize() {
  return GaudiTool::finalize();
}

StatusCode G4ParticleSmearSimple::smearMomentum( CLHEP::Hep3Vector& aMom, int /*aPdg*/) {
  double tmp = m_gauss.shoot();
  aMom *= tmp;
  return StatusCode::SUCCESS;
}

StatusCode G4ParticleSmearSimple::smearEnergy( double& aEn, int /*aPdg*/) {
  double tmp;
  do {
    tmp = m_gauss.shoot();
    aEn *= tmp;
  }
  while(aEn<0);
  return StatusCode::SUCCESS;
}
