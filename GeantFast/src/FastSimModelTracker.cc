#include "FastSimModelTracker.hh"
#include "PrimaryParticleInformation.hh"
#include "Smearer.hh"
#include "Output.hh"

#include "G4Track.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"

#include "G4PathFinder.hh"
#include "G4FieldTrack.hh"
#include "G4FieldTrackUpdator.hh"
#include "G4SystemOfUnits.hh"

FastSimModelTracker::FastSimModelTracker(G4String aModelName, G4Region* aEnvelope, DetectorParametrisation::Parametrisation aType)
   : G4VFastSimulationModel(aModelName, aEnvelope), fCalculateParametrisation(), fParametrisation(aType)
{}

FastSimModelTracker::FastSimModelTracker(G4String aModelName, G4Region* aEnvelope)
   : G4VFastSimulationModel(aModelName, aEnvelope), fCalculateParametrisation(), fParametrisation(DetectorParametrisation::eCMS)
{}

FastSimModelTracker::FastSimModelTracker(G4String aModelName)
   : G4VFastSimulationModel(aModelName), fCalculateParametrisation(), fParametrisation(DetectorParametrisation::eCMS)
{}

FastSimModelTracker::~FastSimModelTracker()
{}

G4bool FastSimModelTracker::IsApplicable(const G4ParticleDefinition& aParticleType)
{
   return aParticleType.GetPDGCharge() != 0;
}

G4bool FastSimModelTracker::ModelTrigger(const G4FastTrack& /*aFastTrack*/)
{
   return true;
}

void FastSimModelTracker::DoIt(const G4FastTrack& aFastTrack,
                                  G4FastStep& aFastStep)
{
// ================
   G4ThreeVector spin        = aFastTrack.GetPrimaryTrack()->GetPolarization() ;
   G4FieldTrack  theFieldTrack = G4FieldTrack( aFastTrack.GetPrimaryTrack()->GetPosition(),
                                               aFastTrack.GetPrimaryTrack()->GetMomentumDirection(),
                                               0.0,
                                               aFastTrack.GetPrimaryTrack()->GetKineticEnergy(),
                                               aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetDefinition()->GetPDGMass(),
                                               0.0,    // UNUSED: aFastTrack.GetPrimaryTrack().GetVelocity(),
                                               aFastTrack.GetPrimaryTrack()->GetGlobalTime(), // Lab.
                                               aFastTrack.GetPrimaryTrack()->GetProperTime(), // Part.
                                               &spin) ;

   G4double retSafety= -1.0;
   ELimited retStepLimited;
   G4FieldTrack endTrack('a') ;
   G4double  currentMinimumStep= 10*m; // change that to sth connected to particle momentum !!!!!
   G4PathFinder* fPathFinder=  G4PathFinder::GetInstance();
   /*G4double lengthAlongCurve = */ fPathFinder->ComputeStep( theFieldTrack,
                                                              currentMinimumStep,
                                                              0,
                                                              aFastTrack.GetPrimaryTrack()->GetCurrentStepNumber(),
                                                              retSafety,
                                                              retStepLimited,
                                                              endTrack,
                                                              aFastTrack.GetPrimaryTrack()->GetVolume() ) ;
   aFastStep.ProposePrimaryTrackFinalPosition( endTrack.GetPosition() );

// ================

   // smear according to tracker resolution
   G4ThreeVector Porg = aFastTrack.GetPrimaryTrack()->GetMomentum();
   if ( !aFastTrack.GetPrimaryTrack()->GetParentID() )
   {
      G4double res = fCalculateParametrisation->GetResolution(DetectorParametrisation::eTRACKER, fParametrisation, Porg.mag());
      G4double eff = fCalculateParametrisation->GetEfficiency(DetectorParametrisation::eTRACKER, fParametrisation, Porg.mag());
      G4ThreeVector Psm;
      Psm = Smearer::Instance()->SmearMomentum(aFastTrack.GetPrimaryTrack(), res);
      Output::Instance()->FillHistogram(0,((Psm.mag()/MeV) / (Porg.mag()/MeV)) );
      // setting values of Psm, res and eff
      ((PrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                        (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetTrackerMomentum(Psm);
      ((PrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                        (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetTrackerResolution(res);
      ((PrimaryParticleInformation*)(const_cast<G4PrimaryParticle*>
                                        (aFastTrack.GetPrimaryTrack()->GetDynamicParticle()->GetPrimaryParticle())->GetUserInformation()))->SetTrackerEfficiency(eff);
   }
}
