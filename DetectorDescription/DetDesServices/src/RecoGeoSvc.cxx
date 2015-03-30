//
//  RecoGeoSvc.cxx
//  
//
//  Created by Julia Hrdinka on 24/02/15.
//
//

#include "DetDesServices/RecoGeoSvc.h"

using namespace Gaudi;

DECLARE_COMPONENT(RecoGeoSvc)

RecoGeoSvc::RecoGeoSvc(const std::string& name, ISvcLocator* svc)
: base_class(name, svc),
  m_worldVolume(0),
  m_recoConverter(),
  m_log(msgSvc(), name)
{
    declareProperty("RecoConverterTool", m_recoConverter);
    declarePublicTool(m_recoConverter, "RecoGeoConverterTool/RecoGeoConverterTool");
}

RecoGeoSvc::~RecoGeoSvc()
{}

StatusCode RecoGeoSvc::initialize()
{
    if (m_recoConverter.retrieve().isFailure())
        m_log << MSG::WARNING << "Retrieving RecoGeoConverter failed." << endmsg;
    
    return StatusCode::SUCCESS;
}

StatusCode RecoGeoSvc::finalize()
{
    return StatusCode::SUCCESS;
}

StatusCode RecoGeoSvc::buildGeometry()
{
    m_recoConverter->convert();
    
    return StatusCode::SUCCESS;
}

StatusCode RecoGeoSvc::destroyGeometry()
{
    return StatusCode::SUCCESS;
}

void RecoGeoSvc::setWorldVolume(std::shared_ptr<const Reco::ContainerVolume> worldVolume)
{
    m_worldVolume = worldVolume;
}

const Reco::ContainerVolume* RecoGeoSvc::getWorldVolume()
{
    return (m_worldVolume.get());
}