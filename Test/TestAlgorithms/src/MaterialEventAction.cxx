//
//  MaterialEventAction.cc
//  
//
//  Created by Julia Hrdinka on 19/05/15.
//
//

#include "TestAlgorithms/MaterialEventAction.h"
#include "TestAlgorithms/MaterialPrimaryGeneratorAction.h"
#include "TestAlgorithms/MaterialRunAction.h"
#include "TestAlgorithms/MaterialSteppingAction.h"
// use of stepping action to get and reset accumulated energy

#include "G4RunManager.hh"
#include "G4Event.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MaterialEventAction* MaterialEventAction::fgInstance = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MaterialEventAction* MaterialEventAction::Instance()
{
    // Static acces function via G4RunManager
    
    return fgInstance;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MaterialEventAction::MaterialEventAction()
: G4UserEventAction(),
m_tInX0(),
m_pathlength(),
m_hits()
{
    m_out.open("printhits.dat");
    fgInstance = this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MaterialEventAction::~MaterialEventAction()
{
    fgInstance = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MaterialEventAction::BeginOfEventAction(const G4Event* event)
{
    // Reset accounted energy in stepping action
    MaterialSteppingAction::Instance()->Reset();
    m_out.close();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MaterialEventAction::EndOfEventAction(const G4Event* /*event*/)
{
    G4ThreeVector dir   = MaterialPrimaryGeneratorAction::Instance()->GetDirection();
    G4double    tInX0   = MaterialSteppingAction::Instance()->GetTInX0();
    G4double pathlength = MaterialSteppingAction::Instance()->GetPathlength();
    G4int       hits    = MaterialSteppingAction::Instance()->GetHits();
    m_tInX0.push_back(std::make_pair(tInX0, dir));
    pathlength = pathlength/cm;
    m_pathlength.push_back(std::make_pair(pathlength, dir));
    m_hits.push_back(std::make_pair(hits,dir));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MaterialEventAction::Reset()
{
    //reset cumulative quantities
    m_tInX0.clear();
    m_pathlength.clear();
    m_hits.clear();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

