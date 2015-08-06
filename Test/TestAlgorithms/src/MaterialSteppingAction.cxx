//
//  MaterialSteppingAction.cxx
//  
//
//  Created by Julia Hrdinka on 18/05/15.
//
//

#include "TestAlgorithms/MaterialSteppingAction.h"

#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include <G4VSensitiveDetector.hh>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MaterialSteppingAction* MaterialSteppingAction::fgInstance = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MaterialSteppingAction* MaterialSteppingAction::Instance()
{
    // Static acces function via G4RunManager
    
    return fgInstance;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MaterialSteppingAction::MaterialSteppingAction()
: G4UserSteppingAction(),
m_tInX0(0.),
m_pathlength(0.),
m_counter(0)
{
    fgInstance = this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MaterialSteppingAction::~MaterialSteppingAction()
{
    fgInstance = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MaterialSteppingAction::UserSteppingAction(const G4Step* step)
{
    // get volume of the current step
    G4String material = step->GetPreStepPoint()->GetTouchableHandle()
    ->GetVolume()->GetLogicalVolume()->GetMaterial()->GetName();
    G4double steplength = step->GetStepLength();
    if (material!="Air" && material!="Vacuum") {
       // steplength = step->GetStepLength();
        G4double radlength  = step->GetPreStepPoint()->GetTouchableHandle()
        ->GetVolume()->GetLogicalVolume()->GetMaterial()->GetRadlen();
    //    m_tInX0 = radlength;
        m_tInX0 += steplength/radlength;
    //    steplength = steplength/cm;
        m_pathlength += steplength;
        
  /*      G4cout << " Mat: " << material << " A: " << step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetMaterial()->GetA() << " Z: " << step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetMaterial()->GetZ() << " lambda: "<< step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetMaterial()->GetNuclearInterLength()  <<   " RL: " << m_tInX0 << G4endl;*/
    }
    G4VSensitiveDetector* sensdet= step->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume()->GetSensitiveDetector();
    if (sensdet) {
        m_hits++;
        m_pos.push_back(step->GetPreStepPoint()->GetPosition());
    }
/*    if(sensdet && sensdet->Hit(g4step)) {
      m_hits++;
        G4cout << "HIT" << std::endl;
    }*/
    G4ThreeVector position = step->GetPreStepPoint()->GetPosition();
    G4double r = sqrt(position.x()*position.x()+position.y()*position.y());
  //  G4cout << "R: " << r << " Point: " << step->GetPreStepPoint() << " Mat: " << material << " SL: " << steplength << " PL: " << m_pathlength << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MaterialSteppingAction::Reset()
{
    m_tInX0         = 0.;
    m_pathlength    = 0.;
    m_counter       = 0;
    m_hits          = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


