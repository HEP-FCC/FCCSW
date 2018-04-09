#include "SimG4ParticleSmearSimple.h"

// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"

// CLHEP
#include "CLHEP/Vector/ThreeVector.h"

DECLARE_TOOL_FACTORY(SimG4ParticleSmearSimple)

SimG4ParticleSmearSimple::SimG4ParticleSmearSimple(const std::string& type, const std::string& name,
                                                   const IInterface* parent)
    : GaudiTool(type, name, parent) {
  declareInterface<ISimG4ParticleSmearTool>(this);
}

SimG4ParticleSmearSimple::~SimG4ParticleSmearSimple() {}

StatusCode SimG4ParticleSmearSimple::initialize() {
  if (GaudiTool::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  if (service("RndmGenSvc", m_randSvc).isFailure()) {
    error() << "Couldn't get RndmGenSvc" << endmsg;
    return StatusCode::FAILURE;
  }
  m_gauss.initialize(m_randSvc, Rndm::Gauss(1, m_sigma));
  info() << "Tool used for smearing particles initialized with constant sigma = " << m_sigma << endmsg;
  return StatusCode::SUCCESS;
}

StatusCode SimG4ParticleSmearSimple::finalize() { return GaudiTool::finalize(); }

StatusCode SimG4ParticleSmearSimple::smearMomentum(CLHEP::Hep3Vector& aMom, int /*aPdg*/) {
  double tmp = m_gauss.shoot();
  aMom *= tmp;
  return StatusCode::SUCCESS;
}
