#include "G4ParticleSmearTklayout.h"

// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"

//CLHEP
#include "CLHEP/Vector/ThreeVector.h"

DECLARE_TOOL_FACTORY(G4ParticleSmearTklayout)

G4ParticleSmearTklayout::G4ParticleSmearTklayout(const std::string& type, const std::string& name, const IInterface* parent):
  GaudiTool(type, name, parent) {
  declareInterface<IG4ParticleSmearTool>(this);
  declareProperty("resolutions", m_fileName = "");
}

G4ParticleSmearTklayout::~G4ParticleSmearTklayout() {}

StatusCode G4ParticleSmearTklayout::initialize() {
  if(GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if(service( "RndmGenSvc" , m_randSvc ).isFailure()) {
    error() << "Couldn't get RndmGenSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  // m_gauss.initialize(m_randSvc, Rndm::Gauss(1,m_sigma));
  // info() << "Tool used for smearing particles initialized with sigma = "<<m_sigma << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode G4ParticleSmearTklayout::finalize() {
  return GaudiTool::finalize();
}

StatusCode G4ParticleSmearTklayout::smearMomentum( CLHEP::Hep3Vector& aMom, int /*aPdg*/) {
  double tmp = m_gauss.shoot();
  aMom *= tmp;
  return StatusCode::SUCCESS;
}

StatusCode G4ParticleSmearTklayout::smearEnergy( double& aEn, int /*aPdg*/) {
  double tmp;
  do {
    tmp = m_gauss.shoot();
    aEn *= tmp;
  }
  while(aEn<0);
  return StatusCode::SUCCESS;
}
