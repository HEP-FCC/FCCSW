//
//  MaterialEventAction.h
//  
//
//  Created by Julia Hrdinka on 19/05/15.
//
//

#ifndef MaterialEventAction_h
#define MaterialEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

//std
#include <iostream>
#include <fstream>
#include <stdlib.h>

class MaterialSteppingAction;

class MaterialEventAction : public G4UserEventAction
{
public:
    MaterialEventAction();
    virtual ~MaterialEventAction();
    
    // static access method
    static MaterialEventAction* Instance();
    
    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);
    
    void Reset();
    
    // get methods
    std::vector<std::pair<G4double, G4ThreeVector>> GetTInX0() const { return m_tInX0; }
    std::vector<std::pair<G4double, G4ThreeVector>> GetPathlength() const { return m_pathlength; }
    std::vector<std::pair<G4int   , G4ThreeVector>> GetHits() const { return m_hits; }
    
private:
    static MaterialEventAction* fgInstance;
    
    std::vector<std::pair<G4double, G4ThreeVector>> m_tInX0;
    std::vector<std::pair<G4double, G4ThreeVector>> m_pathlength;
    std::vector<std::pair<G4int   , G4ThreeVector>> m_hits;
    
    mutable std::ofstream m_out;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
