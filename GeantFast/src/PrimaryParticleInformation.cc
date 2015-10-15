#include "PrimaryParticleInformation.hh"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

PrimaryParticleInformation::PrimaryParticleInformation(G4int aPartID, G4int aPDG, G4ThreeVector aMomentum): fPartID(aPartID), fPDG(aPDG), fMomentumMC(aMomentum), fMomentumTracker(0), fResolutionTracker(0), fEfficiencyTracker(0), fPositionEMCal(0), fEnergyEMCal(0), fResolutionEMCal(0), fEfficiencyEMCal(0), fPositionHCal(0), fEnergyHCal(0), fResolutionHCal(0), fEfficiencyHCal(0)
{}

PrimaryParticleInformation::~PrimaryParticleInformation()
{}

void PrimaryParticleInformation::Print() const
{
   ServiceHandle<IMessageSvc> msgh("MessageSvc","DetectorConstruction");
   MsgStream log(&(*msgh), "DetectorConstruction");
   log << MSG::INFO <<"PrimaryParticleInformation: PDG code "<<fPDG<<endreq
          <<"Particle unique ID:  "<<fPartID<<endreq
         <<"MC momentum: "<<fMomentumMC<<endreq
         <<"Tracker momentum: "<<fMomentumTracker<<endreq
         <<"Tracker resolution: "<<fResolutionTracker<<endreq
         <<"Tracker efficiency: "<<fEfficiencyTracker<<endreq
         <<"EMCal energy: "<<fEnergyEMCal<< " at "<<fPositionEMCal<<endreq
         <<"EMCal resolution: "<<fResolutionEMCal<<endreq
         <<"EMCal efficiency: "<<fEfficiencyEMCal<<endreq
         <<"HCal energy: "<<fEnergyHCal<< " at "<<fPositionHCal<<endreq
         <<"HCal resolution: "<<fResolutionHCal<<endreq
         <<"HCal efficiency: "<<fEfficiencyHCal<<endreq;
}
