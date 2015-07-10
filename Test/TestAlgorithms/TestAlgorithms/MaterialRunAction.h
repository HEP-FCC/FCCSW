//
//  MaterialRunAction.hh
//  
//
//  Created by Julia Hrdinka on 19/05/15.
//
//

#ifndef MaterialRunAction_h
#define MaterialRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
//#include "GaudiKernel/ITHistSvc.h"
#include "TProfile.h"
#include "TFile.h"
#include "g4root.hh"
#include "Algebra/AlgPrimitives.h"

class G4Run;

class MaterialRunAction : public G4UserRunAction
{
public:
    MaterialRunAction();
    virtual ~MaterialRunAction();
    
    static MaterialRunAction* Instance();
    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);
    
    std::vector<Alg::Vector3D> GetDirections()
    {return m_directions;}
    
//    std::vector<std::pair<G4double, G4ThreeVector>> GetTInX0();
    
private:
    static MaterialRunAction* fgInstance;
    std::vector<Alg::Vector3D> m_directions;
//    ITHistSvc*      m_ths;
//    TProfile*       m_tInX0;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
