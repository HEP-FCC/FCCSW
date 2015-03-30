//
//  Geant4Test.cxx
//  
//
//  Created by Julia Hrdinka on 26/02/15.
//
//

#include "TestAlgorithms/Geant4Test.h"
//Geant4
#include "G4RunManager.hh"
#include "FTFP_BERT.hh"
#include "G4GDMLParser.hh"
#include "G4TransportationManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "TestAlgorithms/B1PrimaryGeneratorAction.h"

DECLARE_COMPONENT(Geant4Test)

Geant4Test::Geant4Test(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc)
{}

StatusCode Geant4Test::initialize() {
    
    if (GaudiAlgorithm::initialize().isFailure()){
        return StatusCode::FAILURE;
    }
    
    if (service("Geant4GeoSvc", m_g4GeoSvc, true).isFailure()) {
        error() << "Couldn't get Geant4GeoSvc" << endmsg;
        return StatusCode::FAILURE;
    }
    
    return StatusCode::SUCCESS;
}


StatusCode Geant4Test::execute() {
    
    m_g4GeoSvc->buildGeometry();
    G4RunManager * runManager = new G4RunManager;
    runManager->SetUserInitialization( m_g4GeoSvc->getDetector()); //constructs detector (calls Construct in Geant4DetectorConstruction)
    
    runManager->SetUserInitialization(new FTFP_BERT);
    
    runManager->Initialize();
    runManager->SetUserAction(new B1PrimaryGeneratorAction);
    G4GDMLParser parser;
    parser.Write("Geant4Detector.gdml", G4TransportationManager::GetTransportationManager()
     ->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume());

    return StatusCode::SUCCESS;
}


StatusCode Geant4Test::finalize() {
    
    if (GaudiAlgorithm::finalize().isFailure())
        return StatusCode::FAILURE;
    
    return StatusCode::SUCCESS;
    
}
