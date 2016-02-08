#include "SimG4Fast/FastSimModelTracker.h"

// FCCSW
#include "SimG4Interface/IG4ParticleSmearTool.h"

// Gaudi
#include "GaudiKernel/IToolSvc.h"

// Geant4
#include "G4PathFinder.hh"
#include "G4SystemOfUnits.hh"

namespace sim {
FastSimModelTracker::FastSimModelTracker(const std::string& aModelName, G4Region* aEnvelope, const std::string& aSmearToolName):
  G4VFastSimulationModel(aModelName, aEnvelope),
  m_toolSvc("ToolSvc","ToolSvc") {
  if( m_toolSvc->retrieveTool(aSmearToolName, m_smearTool, 0, false).isFailure())
    throw GaudiException("Smearing tool "+aSmearToolName+" not found",
                         "FastSimModelTracker", StatusCode::FAILURE);
}

FastSimModelTracker::FastSimModelTracker(const std::string& aModelName)
  : G4VFastSimulationModel(aModelName), m_toolSvc("ToolSvc","ToolSvc") {}

FastSimModelTracker::~FastSimModelTracker() {}

G4bool FastSimModelTracker::IsApplicable(const G4ParticleDefinition& aParticleType) {
  return aParticleType.GetPDGCharge() != 0;
}

G4bool FastSimModelTracker::ModelTrigger(const G4FastTrack& /*aFastTrack*/) {
  return true;
}

void FastSimModelTracker::DoIt(const G4FastTrack& aFastTrack,
                               G4FastStep& aFastStep) {
  // Calculate the position of the particle at the end of volume
  const G4Track* track = aFastTrack.GetPrimaryTrack();
  G4ThreeVector spin = track->GetPolarization() ;
  G4FieldTrack theFieldTrack = G4FieldTrack( track->GetPosition(),
                                             track->GetMomentumDirection(),
                                             0.0,
                                             track->GetKineticEnergy(),
                                             track->GetDynamicParticle()->GetDefinition()->GetPDGMass(),
                                             0.0,
                                             track->GetGlobalTime(), // Lab.
                                             track->GetProperTime(), // Part.
                                             &spin) ;
  G4double retSafety= -1.0;
  ELimited retStepLimited;
  G4FieldTrack endTrack('a');
  G4double currentMinimumStep= 10*m; // TODO change that to sth connected to particle momentum and geometry
  G4PathFinder* fPathFinder = G4PathFinder::GetInstance();
  fPathFinder->ComputeStep( theFieldTrack,
                            currentMinimumStep,
                            0,
                            track->GetCurrentStepNumber(),
                            retSafety,
                            retStepLimited,
                            endTrack,
                            track->GetVolume() );
  aFastStep.ProposePrimaryTrackFinalPosition( endTrack.GetPosition() );

  // Smear particle's momentum according to the tracker resolution (set in SimpleSmear)
  G4ThreeVector Psm = track->GetMomentum();
  m_smearTool->smearMomentum(Psm);
  G4ThreeVector DeltaP = track->GetMomentum() - Psm;
  G4double Ekinorg = track->GetKineticEnergy();
  aFastStep.ClearDebugFlag(); // to disable Geant checks on energy
  aFastStep.ProposePrimaryTrackFinalKineticEnergyAndDirection(Ekinorg+DeltaP.mag(), Psm.unit());
}
}
