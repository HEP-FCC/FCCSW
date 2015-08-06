//
//  MaterialSteppingAction.hh
//  
//
//  Created by Julia Hrdinka on 19/05/15.
//
//

#ifndef MaterialSteppingAction_h
#define MaterialSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"


class G4LogicalVolume;

class MaterialSteppingAction : public G4UserSteppingAction
{
public:
    MaterialSteppingAction();
    virtual ~MaterialSteppingAction();
    
    // static access method
    static MaterialSteppingAction* Instance();
    
    // method from the base class
    virtual void UserSteppingAction(const G4Step*);
    
    // reset accumulated energy
    void Reset();
    
    // get methods
    G4double GetTInX0() const { return m_tInX0; }
    G4double GetPathlength() const { return m_pathlength; }
    G4double GetHits() const { return m_hits; }
    std::vector <G4ThreeVector> GetPos() const { return m_pos; }
    
private:
    static MaterialSteppingAction* fgInstance;
    
    G4double  m_tInX0;
    G4double  m_pathlength;
    G4int     m_counter;
    G4int     m_hits;
    std::vector <G4ThreeVector> m_pos;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

