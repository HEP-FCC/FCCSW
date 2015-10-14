#include "FastSimModelTracker.h"

// Geant4
#include "G4PathFinder.hh"
#include "G4SystemOfUnits.hh"

FastSimModelTracker::FastSimModelTracker(G4String aModelName, G4Region* aEnvelope, std::string aSmearToolName)
   : G4VFastSimulationModel(aModelName, aEnvelope), m_msgSvc("MessageSvc","FastSimModelTracker"),
     log(&(*m_msgSvc), "FastSimModelTracker"), m_toolSvc("ToolSvc","ToolSvc")
{
   if( m_toolSvc->retrieveTool(aSmearToolName, m_smearTool).isFailure())
      throw GaudiException("Smearing tool "+aSmearToolName+" not found",
                           "FastSimModelTracker", StatusCode::FAILURE);
}

FastSimModelTracker::FastSimModelTracker(G4String aModelName)
   : G4VFastSimulationModel(aModelName), m_msgSvc("MessageSvc","FastSimModelTracker"),
     log(&(*m_msgSvc), "FastSimModelTracker"), m_toolSvc("ToolSvc","ToolSvc")
{}

FastSimModelTracker::~FastSimModelTracker()
{}

G4bool FastSimModelTracker::IsApplicable(const G4ParticleDefinition& aParticleType)
{
   log<<"is applicable"<<endmsg;
   return aParticleType.GetPDGCharge() != 0;
}

G4bool FastSimModelTracker::ModelTrigger(const G4FastTrack& /*aFastTrack*/)
{
   return true;
}

void FastSimModelTracker::DoIt(const G4FastTrack& aFastTrack,
                            G4FastStep& aFastStep)
{
   // Calculate the position of the particle at the end of volume
   G4ThreeVector spin = aFastTrack.GetPrimaryTrack()->GetPolarization() ;
   G4FieldTrack theFieldTrack = G4FieldTrack( aFastTrack.GetPrimaryTrack()->GetPosition(),
                                              aFastTrack.GetPrimaryTrack()->GetMomentumDirection(),
                                              0.0,
                                              aFastTrack.GetPrimaryTrack()->GetKineticEnergy(),
                                              aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetDefinition()->GetPDGMass(),
                                              0.0,
                                              aFastTrack.GetPrimaryTrack()->GetGlobalTime(), // Lab.
                                              aFastTrack.GetPrimaryTrack()->GetProperTime(), // Part.
                                              &spin) ;
   G4double retSafety= -1.0;
   ELimited retStepLimited;
   G4FieldTrack endTrack('a');
   G4double currentMinimumStep= 10*m; // TODO change that to sth connected to particle momentum and geometry
   G4PathFinder* fPathFinder = G4PathFinder::GetInstance();
   fPathFinder->ComputeStep( theFieldTrack,
                             currentMinimumStep,
                             0,
                             aFastTrack.GetPrimaryTrack()->GetCurrentStepNumber(),
                             retSafety,
                             retStepLimited,
                             endTrack,
                             aFastTrack.GetPrimaryTrack()->GetVolume() );
   aFastStep.ProposePrimaryTrackFinalPosition( endTrack.GetPosition() );

   // Smear particle's momentum according to the tracker resolution (set in SimpleSmear)
   G4ThreeVector Porg = aFastTrack.GetPrimaryTrack()->GetMomentum();
   G4ThreeVector Psm = Porg;
   m_smearTool->smearMomentum(Porg);
   G4double Ekinorg = aFastTrack.GetPrimaryTrack()->GetKineticEnergy();
   // Geant doesn't allow increasing energy
   aFastStep.ProposePrimaryTrackFinalKineticEnergyAndDirection(Ekinorg-fabs(Psm.mag()-Porg.mag()), Psm.unit());
}
