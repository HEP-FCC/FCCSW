//
//  MaterialPrimaryGeneratorAction.hh
//  
//
//  Created by Julia Hrdinka on 19/05/15.
//
//

#ifndef MaterialPrimaryGeneratorAction_h
#define MaterialPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4ParticleGun;
class G4Event;

class MaterialPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    MaterialPrimaryGeneratorAction(
                                 const G4String& particleName = "geantino",
                                 G4double energy = 1.*MeV,
                                 G4ThreeVector position= G4ThreeVector(0,0,0),
                                 G4ThreeVector momentumDirection = G4ThreeVector(0,0,1));
    ~MaterialPrimaryGeneratorAction();
    static MaterialPrimaryGeneratorAction* Instance();
    // methods
    virtual void GeneratePrimaries(G4Event*);
    G4ThreeVector GetDirection() {return m_dir;}
    
private:
    static MaterialPrimaryGeneratorAction* fgInstance;
    // data members
    G4ParticleGun*  fParticleGun; //pointer a to G4 service class
    G4ThreeVector   m_dir;
};

#endif

