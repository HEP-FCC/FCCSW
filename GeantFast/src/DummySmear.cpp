#include "DummySmear.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY(DummySmear)

DummySmear::DummySmear( const std::string& type,  const std::string& name,
                          const IInterface* parent ) : GaudiTool(type, name, parent)
{
   declareInterface<ISmearingTool>(this);
}

DummySmear::~DummySmear(){}

StatusCode DummySmear::initialize()
{
   info()<<" I DUMMY !!!!! INIT 1"<<endmsg;
   if(GaudiTool::initialize().isFailure())
      return StatusCode::FAILURE;
   info()<<" I DUMMY !!!!! INIT 2"<<endmsg;
 return StatusCode::SUCCESS;
}

StatusCode DummySmear::smearMomentum( G4ThreeVector& aMom )
{
   info()<<" I DUMMY !!!!! MOM"<<endmsg;
   return StatusCode::SUCCESS;
}

StatusCode DummySmear::smearEnergy( G4double& aEn )
{
   info()<<" I DUMMY !!!!! EN"<<endmsg;
   return StatusCode::SUCCESS;
}
