//
//  Geant4GeoSvc.cxx
//  
//
//  Created by Julia Hrdinka on 26/02/15.
//
//

#include "DetDesServices/Geant4GeoSvc.h"

using namespace Gaudi;

DECLARE_COMPONENT(Geant4GeoSvc)

Geant4GeoSvc::Geant4GeoSvc(const std::string& name, ISvcLocator* svc)
: base_class(name, svc),
m_detector(0),
m_g4Converter(),
m_log(msgSvc(), name)
{
    declareProperty("Geant4GeoConverterTool", m_g4Converter);
    declarePublicTool(m_g4Converter, "Geant4GeoConverterTool/Geant4GeoConverterTool");
}

Geant4GeoSvc::~Geant4GeoSvc()
{}

StatusCode Geant4GeoSvc::initialize()
{
    if (m_g4Converter.retrieve().isFailure())
        m_log << MSG::WARNING << "Retrieving Geant4GeoConverter failed." << endmsg;
    
    return StatusCode::SUCCESS;
}

StatusCode Geant4GeoSvc::finalize()
{
    return StatusCode::SUCCESS;
}

StatusCode Geant4GeoSvc::buildGeometry()
{
    m_g4Converter->convert();
    
    return StatusCode::SUCCESS;
}

StatusCode Geant4GeoSvc::destroyGeometry()
{
    return StatusCode::SUCCESS;
}

void Geant4GeoSvc::setDetector(std::shared_ptr<G4VUserDetectorConstruction> detector)
{
    m_detector = detector;
}

G4VUserDetectorConstruction* Geant4GeoSvc::getDetector()
{
    return (m_detector.get());
}
