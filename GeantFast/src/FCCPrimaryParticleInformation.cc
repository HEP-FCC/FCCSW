#include "FCCPrimaryParticleInformation.hh"

FCCPrimaryParticleInformation::FCCPrimaryParticleInformation(G4int aPartID, G4int aPDG, G4ThreeVector aMomentum): fPartID(aPartID), fPDG(aPDG), fMomentumMC(aMomentum), fMomentumTracker(0), fResolutionTracker(0), fEfficiencyTracker(0), fPositionEMCal(0), fEnergyEMCal(0), fResolutionEMCal(0), fEfficiencyEMCal(0), fPositionHCal(0), fEnergyHCal(0), fResolutionHCal(0), fEfficiencyHCal(0), fPerigeeMC(NULL), fPerigeeTracker(NULL), fPerigeeEMCal(NULL), fPerigeeHCal(NULL)
{}

FCCPrimaryParticleInformation::~FCCPrimaryParticleInformation()
{}

void FCCPrimaryParticleInformation::Print() const
{
   G4cout<<"FCCPrimaryParticleInformation: PDG code "<<fPDG<<G4endl
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
   if(fPerigeeMC)
      G4cout<<" MC perigee: "<<fPerigeeMC[0]<<"\t"<<fPerigeeMC[1]<<"\t"<<fPerigeeMC[2]<<"\t"
            <<fPerigeeMC[3]<<"\t"<<fPerigeeMC[4]<<G4endl;
   if(fPerigeeTracker)
      G4cout<<" Tracker perigee: "<<fPerigeeTracker[0]<<"\t"<<fPerigeeTracker[1]<<"\t"<<fPerigeeTracker[2]<<"\t"
            <<fPerigeeTracker[3]<<"\t"<<fPerigeeTracker[4]<<G4endl;
   if(fPerigeeEMCal)
      G4cout<<" EMCal perigee: "<<fPerigeeEMCal[0]<<"\t"<<fPerigeeEMCal[1]<<"\t"<<fPerigeeEMCal[2]<<"\t"
            <<fPerigeeEMCal[3]<<"\t"<<fPerigeeEMCal[4]<<G4endl;
   if(fPerigeeHCal)
      G4cout<<" HCal perigee: "<<fPerigeeHCal[0]<<"\t"<<fPerigeeHCal[1]<<"\t"<<fPerigeeHCal[2]<<"\t"
            <<fPerigeeHCal[3]<<"\t"<<fPerigeeHCal[4]<<G4endl;
}
