//
//  MaterialRunAction.cc
//  
//
//  Created by Julia Hrdinka on 19/05/15.
//
//

#include "TestAlgorithms/MaterialRunAction.h"
#include "TestAlgorithms/MaterialPrimaryGeneratorAction.h"
#include "TestAlgorithms/MaterialEventAction.h"
#include "TestAlgorithms/MaterialSteppingAction.h"
// use of other actions
// - primary generator: to get info for printing about the primary
// - event action: to get and reset accumulated energy sums
// - stepping action: to get info about accounting volume

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

MaterialRunAction* MaterialRunAction::fgInstance = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MaterialRunAction::MaterialRunAction()
: G4UserRunAction(),
  m_directions()
{
    fgInstance = this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MaterialRunAction::~MaterialRunAction()
{
    fgInstance = 0;
}

MaterialRunAction* MaterialRunAction::Instance()
{
    return fgInstance;
}

/*std::vector<std::pair<G4double, G4ThreeVector>> GetTInX0()
{
    return MaterialEventAction::Instance()->GetTInX0();
}*/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MaterialRunAction::BeginOfRunAction(const G4Run* aRun)
{
    G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
    
    //initialize event cumulative quantities
    MaterialEventAction::Instance()->Reset();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MaterialRunAction::EndOfRunAction(const G4Run* aRun)
{
    G4int nofEvents = aRun->GetNumberOfEvent();
    if (nofEvents == 0) return;
    
  //  G4AnalysisManager* man = G4AnalysisManager::Instance();
    
    TFile* file = new TFile("geant4detector.root", "RECREATE");
 //   if(!file->IsOpen()) throw GaudiException("Could not open Root file", name(), sc);
    
    std::vector<std::pair<G4double, G4ThreeVector>> tvector(MaterialEventAction::Instance()->GetTInX0());
    TProfile* tInX0 = new TProfile("G4::tInX0", "G4::tInX0 over Eta", 5000, -10., 10.);
    for (auto& it : tvector)
    {
        tInX0->Fill(it.second.eta(),it.first);
        m_directions.push_back(Alg::Vector3D(it.second.x(),it.second.y(),it.second.z()));
    }
    tInX0->Write();

    
    std::vector<std::pair<G4double, G4ThreeVector>> pvector(MaterialEventAction::Instance()->GetPathlength());
    TProfile* pathlength = new TProfile("G4::Pathlength", "G4::Pathlength over Eta", 5000, -10,10);
    for (auto& it : pvector)
    {
        pathlength->Fill(it.second.eta(),it.first);
    }
    pathlength->Write();
    
    std::vector<std::pair<G4int, G4ThreeVector>> hvector(MaterialEventAction::Instance()->GetHits());
    TProfile* hits = new TProfile("G4::Hits", "G4::Hits over Eta", 5000, -10,10);
    for (auto& it : hvector)
    {
        hits->Fill(it.second.eta(),it.first);
    }
    hits->Write();
    
    file->Print();
    
    // Print
    //
    G4cout
    << "\n--------------------End of Run------------------------------\n"
    << "\n------------------------------------------------------------\n"
    << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

