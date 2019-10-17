#include "SimG4Fast/FastSimModelTracker.h"

// FCCSW
#include "SimG4Common/ParticleInformation.h"
#include "SimG4Interface/ISimG4ParticleSmearTool.h"

// Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

// Geant4
#include "G4FieldTrackUpdator.hh"
#include "G4PathFinder.hh"
#include "G4PrimaryParticle.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

namespace sim {

FastSimModelTracker::FastSimModelTracker(const std::string& aModelName,
                                         G4Region* aEnvelope,
                                         ToolHandle<ISimG4ParticleSmearTool>& aSmearTool,
                                         double aMinMomentum,
                                         double aMaxMomentum,
                                         double aMaxEta)
    : G4VFastSimulationModel(aModelName, aEnvelope),
      m_msgSvc("MessageSvc", "FastSimModelTracker"),
      m_log(&(*m_msgSvc), "FastSimModelTracker"),
      m_smearTool(aSmearTool),
      m_minTriggerMomentum(aMinMomentum / Gaudi::Units::MeV),
      m_maxTriggerMomentum(aMaxMomentum / Gaudi::Units::MeV),
      m_maxTriggerEta(aMaxEta) {
  m_log << MSG::INFO << "Tracker smearing configuration:\n"
        << "\tEnvelope name:\t" << aEnvelope->GetName() << "\n"
        << "\tMomentum range:\t" << G4BestUnit(m_minTriggerMomentum, "Energy") << " - "
        << G4BestUnit(m_maxTriggerMomentum, "Energy") << "\n"
        << "\tMaximum pseudorapidity:\t" << m_maxTriggerEta << "\n"
        << endmsg;
}

FastSimModelTracker::FastSimModelTracker(const std::string& aModelName, ToolHandle<ISimG4ParticleSmearTool>& aSmearTool)
    : G4VFastSimulationModel(aModelName),
      m_msgSvc("MessageSvc", "FastSimModelTracker"),
      m_log(&(*m_msgSvc), "FastSimModelTracker"),
      m_smearTool(aSmearTool) {}

FastSimModelTracker::~FastSimModelTracker() {}

G4bool FastSimModelTracker::IsApplicable(const G4ParticleDefinition& aParticleType) {
  return aParticleType.GetPDGCharge() != 0;
}

G4bool FastSimModelTracker::ModelTrigger(const G4FastTrack& aFastTrack) {
  double momentum = aFastTrack.GetPrimaryTrackLocalMomentum().mag();
  double eta = aFastTrack.GetPrimaryTrackLocalDirection().eta();
  // first check pseudorapidity
  if (m_maxTriggerEta > 0) {
    if (eta > m_maxTriggerEta) {
      // do not trigger if eta is larger than threshold (if defined)
      return false;
    }
  }
  // next check momentum
  if (m_minTriggerMomentum == m_maxTriggerMomentum) {
    // if no trigger threshold defined for the model
    return true;
  }
  // check the threshold
  if (momentum >= m_minTriggerMomentum && momentum <= m_maxTriggerMomentum) {
    return true;
  }
  return false;
}

void FastSimModelTracker::DoIt(const G4FastTrack& aFastTrack, G4FastStep& aFastStep) {
  // Calculate the position of the particle at the end of volume
  const G4Track* track = aFastTrack.GetPrimaryTrack();
  G4ThreeVector spin = track->GetPolarization();
  G4FieldTrack aFieldTrack('t');
  G4FieldTrackUpdator::Update(&aFieldTrack, track);
  G4double retSafety = -1.0;
  ELimited retStepLimited;
  G4FieldTrack endTrack('a');
  G4double currentMinimumStep = 10 * m;  // TODO change that to sth connected to particle momentum and geometry
  G4PathFinder* fPathFinder = G4PathFinder::GetInstance();
  fPathFinder->ComputeStep(aFieldTrack,
                           currentMinimumStep,
                           0,
                           track->GetCurrentStepNumber(),
                           retSafety,
                           retStepLimited,
                           endTrack,
                           track->GetVolume());
  aFastStep.ProposePrimaryTrackFinalPosition(endTrack.GetPosition());

  // Smear particle's momentum according to the tracker resolution
  G4ThreeVector Psm = track->GetMomentum();
  m_smearTool->smearMomentum(Psm);
  G4ThreeVector DeltaP = track->GetMomentum() - Psm;
  G4double Ekinorg = track->GetKineticEnergy();
  aFastStep.ClearDebugFlag();  // to disable Geant checks on energy
  aFastStep.ProposePrimaryTrackFinalKineticEnergyAndDirection(Ekinorg + DeltaP.mag(), Psm.unit());
  // Keep track of smeared momentum
  if (track->GetParentID() == 0) {
    ParticleInformation* info =
        dynamic_cast<ParticleInformation*>(track->GetDynamicParticle()->GetPrimaryParticle()->GetUserInformation());
    info->setSmeared(true);
    info->setEndStatus(1);  // how it is defined ???? as in HepMC ?
    info->setEndMomentum(Psm);
    info->setVertexPosition(track->GetVertexPosition());
  }
}
}
