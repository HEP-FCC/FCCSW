#include "SimpleSmear.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(SimpleSmear)

SimpleSmear::SimpleSmear( const std::string& type,  const std::string& name,
                          const IInterface* parent ) : GaudiTool(type, name, parent)
{
   declareInterface<ISmearingTool>(this);
   declareProperty("sigma", m_sigma = 0.0);
}

SimpleSmear::~SimpleSmear(){}

StatusCode SimpleSmear::initialize()
{
   info()<<" I SMEAR !!!!! INIT 1"<<endmsg;
   if(GaudiTool::initialize().isFailure())
      return StatusCode::FAILURE;
   info()<<" I SMEAR !!!!! INIT 2"<<endmsg;
   // if (svc<IRndmGenSvc>("RndmGenSvc", m_gaussDist, true).isFailure())
   // {
   //    error() << "Couldn't get RndmGenSvc" << endmsg;
   //    return StatusCode::FAILURE;
   // }
 return StatusCode::SUCCESS;
}

StatusCode SimpleSmear::smearMomentum( G4ThreeVector& aMom )
{
   info()<<" I SMEAR !!!!! MOM"<<endmsg;
   return StatusCode::SUCCESS;
}

StatusCode SimpleSmear::smearEnergy( G4double& aEn )
{
   info()<<" I SMEAR !!!!! EN"<<endmsg;
   return StatusCode::SUCCESS;
}
