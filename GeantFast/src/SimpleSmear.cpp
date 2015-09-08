#include "SimpleSmear.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(SimpleSmear)

SimpleSmear::SimpleSmear( const std::string& type,  const std::string& name,
                          const IInterface* parent ) : GaudiTool(type, name, parent)
{
   declareInterface<ISmearingTool>(this);
   declareProperty("sigma", m_sigma = 0.01);
}

SimpleSmear::~SimpleSmear(){}

StatusCode SimpleSmear::initialize()
{
   if(GaudiTool::initialize().isFailure())
      return StatusCode::FAILURE;
   m_randSvc = svc<IRndmGenSvc>( "RndmGenSvc" , true );
   if(!m_randSvc)
   {
      error() << "Couldn't get RndmGenSvc" << endmsg;
      return StatusCode::FAILURE;
   }
   m_gauss.initialize(m_randSvc, Rndm::Gauss(0,0.01));
   return StatusCode::SUCCESS;
}

StatusCode SimpleSmear::smearMomentum( G4ThreeVector& aMom )
{
   aMom *= m_gauss.shoot();
   return StatusCode::SUCCESS;
}

StatusCode SimpleSmear::smearEnergy( G4double& aEn )
{
   do
   {
      aEn *= m_gauss.shoot();
   }
   while(aEn<0);
   return StatusCode::SUCCESS;
}
