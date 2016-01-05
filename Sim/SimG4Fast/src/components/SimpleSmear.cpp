#include "SimpleSmear.h"

// Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h"

//CLHEP
#include "CLHEP/Vector/ThreeVector.h"

DECLARE_COMPONENT(SimpleSmear)

SimpleSmear::SimpleSmear(const std::string& type, const std::string& name,
                         const IInterface* parent) :
GaudiTool(type, name, parent) {
   declareInterface<ISmearingTool>(this);
   declareProperty("sigma", m_sigma = 0.01);
}

SimpleSmear::~SimpleSmear() {}

StatusCode SimpleSmear::initialize() {
   if(GaudiTool::initialize().isFailure()) {
      return StatusCode::FAILURE;
   }
   if(service( "RndmGenSvc" , m_randSvc ).isFailure()) {
      error() << "Couldn't get RndmGenSvc" << endmsg;
      return StatusCode::FAILURE;
   }
   m_gauss.initialize(m_randSvc, Rndm::Gauss(1,m_sigma));
   info() << "Tool used for smearing particles initialized with sigma = "<<m_sigma << endmsg;
   return StatusCode::SUCCESS;
}

StatusCode SimpleSmear::smearMomentum( CLHEP::Hep3Vector& aMom ) {
   double tmp = m_gauss.shoot();
   aMom *= tmp;
   return StatusCode::SUCCESS;
}

StatusCode SimpleSmear::smearEnergy( double& aEn ) {
   double tmp;
   do {
      tmp = m_gauss.shoot();
      aEn *= tmp;
   }
   while(aEn<0);
   return StatusCode::SUCCESS;
}
