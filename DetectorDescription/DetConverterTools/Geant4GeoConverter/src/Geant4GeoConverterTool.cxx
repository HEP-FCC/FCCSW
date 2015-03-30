//
//  Geant4GeoConverterTool.cxx
//  
//
//  Created by Julia Hrdinka on 10/11/14.
//
//

#include "Geant4GeoConverter/Geant4GeoConverterTool.h"


DECLARE_COMPONENT(Geant4GeoConverterTool)

// Standard Constructor
Geant4GeoConverterTool::Geant4GeoConverterTool(const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent)
: AlgTool( type, name, parent ),
  m_log(msgSvc(), name)
{
    // Declare additional interface
    declareInterface<IGeoConverterTool>(this);
    
    if (service("DD4HepDetDesSvc", m_DD4HepSvc).isFailure()) m_log << MSG::ERROR << "Couldn't get DD4HepDetDesSvc" << endmsg;
    if (service("Geant4GeoSvc", m_g4GeoSvc, true).isFailure()) m_log << "Couldn't get Geant4GeoSvc" << endmsg;
 
}

StatusCode Geant4GeoConverterTool::convert(){
    
    std::shared_ptr<G4VUserDetectorConstruction> detector(new DD4hep::Simulation::Geant4DetectorConstruction(*(m_DD4HepSvc->lcdd())));
    m_g4GeoSvc->setDetector(detector);

    return StatusCode::SUCCESS;
}
