#include "PrimaryParticleInformation.hh"

PrimaryParticleInformation::PrimaryParticleInformation(G4int aPartID, G4int aPDG, G4ThreeVector aMomentum): fPartID(aPartID), fPDG(aPDG), fMomentumMC(aMomentum), fMomentumTracker(0), fResolutionTracker(0), fEfficiencyTracker(0), fPositionEMCal(0), fEnergyEMCal(0), fResolutionEMCal(0), fEfficiencyEMCal(0), fPositionHCal(0), fEnergyHCal(0), fResolutionHCal(0), fEfficiencyHCal(0)
{}

PrimaryParticleInformation::~PrimaryParticleInformation()
{}

void PrimaryParticleInformation::Print() const
{
   G4cout<<"PrimaryParticleInformation: PDG code "<<fPDG<<G4endl
          <<"Particle unique ID:  "<<fPartID<<G4endl
         <<"MC momentum: "<<fMomentumMC<<G4endl
         <<"Tracker momentum: "<<fMomentumTracker<<G4endl
         <<"Tracker resolution: "<<fResolutionTracker<<G4endl
         <<"Tracker efficiency: "<<fEfficiencyTracker<<G4endl
         <<"EMCal energy: "<<fEnergyEMCal<< " at "<<fPositionEMCal<<G4endl
         <<"EMCal resolution: "<<fResolutionEMCal<<G4endl
         <<"EMCal efficiency: "<<fEfficiencyEMCal<<G4endl
         <<"HCal energy: "<<fEnergyHCal<< " at "<<fPositionHCal<<G4endl
         <<"HCal resolution: "<<fResolutionHCal<<G4endl
         <<"HCal efficiency: "<<fEfficiencyHCal<<G4endl
       ;
}
