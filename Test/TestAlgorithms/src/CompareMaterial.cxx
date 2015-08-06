//
//  CompareMaterial.cxx
//  
//
//  Created by Julia Hrdinka on 02/06/15.
//
//

#include "TestAlgorithms/CompareMaterial.h"
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

DECLARE_COMPONENT(CompareMaterial)

CompareMaterial::CompareMaterial(const std::string& name, ISvcLocator* svcLoc):
GaudiAlgorithm(name, svcLoc),
m_recoGeoSvc(0),
m_toolsvc(0),
m_printhits(0),
m_directions()
{}

StatusCode CompareMaterial::initialize() {
    
    if (GaudiAlgorithm::initialize().isFailure()){
        return StatusCode::FAILURE;
    }
    
    if (service("GeoSvc", m_geoSvc, true).isFailure()) {
        error() << "Couldn't GeoSvc" << endmsg;
        return StatusCode::FAILURE;
    }
    
    if (service("ClassicalRecoGeoSvc", m_recoGeoSvc, true).isFailure()) {
        error() << "Couldn't get RecoGeoSvc" << endmsg;
        return StatusCode::FAILURE;
    }
    
    if (service("ToolSvc", m_toolsvc).isFailure()) {
        error() << "Couldn't get ToolSvc" << endmsg;
        return StatusCode::FAILURE;
    }
    
    if (m_toolsvc->retrieveTool("PrintHits", m_printhits).isFailure()) {
        error() << "Couldn't get PrintHits Tool" << endmsg;
    }
    
    return StatusCode::SUCCESS;
}

StatusCode CompareMaterial::runGeant4() {

    G4RunManager * runManager = new G4RunManager;
    runManager->SetUserInitialization( m_geoSvc->getGeant4Geo()); //constructs detector (calls Construct in Geant4DetectorConstruction)
    runManager->SetUserInitialization(new FTFP_BERT);
    
    //    runManager->SetUserAction(new MaterialPrimaryGeneratorAction("geantino", 1.*MeV, G4ThreeVector(0,0,0), G4ThreeVector(1,0,0)));
    runManager->SetUserAction(new MaterialPrimaryGeneratorAction);
    MaterialRunAction* runaction = new MaterialRunAction();
    runManager->SetUserAction(runaction);
    runManager->SetUserAction(new MaterialEventAction);
    runManager->SetUserAction(new MaterialSteppingAction);
    runManager->Initialize();
    int numberOfEvent = 100000;
    runManager->BeamOn(numberOfEvent);
    m_directions = (MaterialRunAction::Instance()->GetDirections());
    delete runManager;
    
    return StatusCode::SUCCESS;

}


StatusCode CompareMaterial::execute() {
    
   // for (auto& it : m_directions) std::cout << it << std::endl;
    //Geant4
    std::cout << "Test1" << std::endl;
    std::shared_ptr<const Reco::ContainerVolume> worldVolume(m_recoGeoSvc->getRecoGeo()->clone());
    std::cout << "Test2" << std::endl;
    if (worldVolume) {
        std::cout << "Retrieved worlvolume" << std::endl;
        std::cout << "Test3" << std::endl;
        runGeant4();
        m_printhits->printMaterial(worldVolume, Alg::Point3D(0.,0.,0.), m_directions);
    }
  

    // m_printhits->printMaterial(worldVolume, Alg::Point3D(0.,0.,0.), 1000);

    //   m_particles.put(m_particlecoll);

    return StatusCode::SUCCESS;
}


StatusCode CompareMaterial::finalize() {
    
    if (GaudiAlgorithm::finalize().isFailure())
        return StatusCode::FAILURE;
    
    return StatusCode::SUCCESS;
    
}
