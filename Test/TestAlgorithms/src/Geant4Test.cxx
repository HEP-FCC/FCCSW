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
#include "TestAlgorithms/MaterialPrimaryGeneratorAction.h"
#include "TestAlgorithms/MaterialRunAction.h"
#include "TestAlgorithms/MaterialEventAction.h"
#include "TestAlgorithms/MaterialSteppingAction.h"

#include "G4VVisManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

DECLARE_COMPONENT(Geant4Test)

Geant4Test::Geant4Test(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc)
{}

StatusCode Geant4Test::initialize() {
    
    if (GaudiAlgorithm::initialize().isFailure()){
        return StatusCode::FAILURE;
    }
 //   StatusCode sc = service("THistSvc", m_ths);
 //   if (sc.isFailure()) throw GaudiException("Service [THistSvc] not found", name(), sc);
    
    if (service("GeoSvc", m_geoSvc, true).isFailure()) {
        error() << "Couldn't GeoSvc" << endmsg;
        return StatusCode::FAILURE;
    }
    
//    file = new TFile("geant4detector.root", "RECREATE");
//    if(!file->IsOpen()) throw GaudiException("Could not open Root file", name(), sc);
    
//    m_radlen.open("radlen.dat", std::ofstream::out);
    
    
    return StatusCode::SUCCESS;
}

StatusCode Geant4Test::execute() {
    
    G4RunManager * runManager = new G4RunManager;
    runManager->SetUserInitialization( m_geoSvc->getGeant4Geo()); //constructs detector (calls Construct in Geant4DetectorConstruction)
    
    runManager->SetUserInitialization(new FTFP_BERT);

    
    runManager->SetUserAction(new MaterialPrimaryGeneratorAction("geantino", 1.*MeV, G4ThreeVector(0,0,0), G4ThreeVector(1,0,0)));
    MaterialRunAction* runaction = new MaterialRunAction();
    runManager->SetUserAction(runaction);
    runManager->SetUserAction(new MaterialEventAction);
    runManager->SetUserAction(new MaterialSteppingAction);
    runManager->Initialize();
    G4GDMLParser parser;
    parser.Write("Geant4Detector.gdml", G4TransportationManager::GetTransportationManager()
    ->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume());

//    m_radlen << "radlen: " << G4TransportationManager::GetTransportationManager()
//    ->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume()->GetMaterial()->GetRadlen() << std::endl;
    
    
    
//    G4VisManager* visManager = new G4VisExecutive;
//    visManager->Initialize();

    int numberOfEvent = 100000;
    runManager->BeamOn(numberOfEvent);
    
    delete runManager;
    
 //   G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
 /*   if (visManager) {
        std::cout << "visManager" << std::endl;
        G4UImanager::GetUIpointer()->ApplyCommand("/control/Test/execute visDetector.mac");
        G4UIsession* session = new G4UIterminal(new G4UItcsh);
        session->SessionStart();
        delete session;
    }
    delete visManager;*/
    
//    m_radlen.close();
    return StatusCode::SUCCESS;
}


StatusCode Geant4Test::finalize() {
    
    if (GaudiAlgorithm::finalize().isFailure())
        return StatusCode::FAILURE;
    
//    file->Print();
    
    return StatusCode::SUCCESS;
    
}
